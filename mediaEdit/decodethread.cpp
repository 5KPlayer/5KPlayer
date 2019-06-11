#include "decodethread.h"
#include <QBuffer>
#include <QImage>
#include <QDebug>
#include <QTime>
#include <memory>
#include <QMutex>
#include "globalarg.h"

QSize Task::_pictureSize;
QList<AVFormatContext*> Task::formatList;
int Task::_video_index = -1;
int Task::rotate = 0;
bool Task::turn = false;
QString Task::HWDeviceName = "";
static AVPixelFormat HWPixFmt = AV_PIX_FMT_NONE;
QMutex mutex;

GetOneFrameThread::GetOneFrameThread(int64_t seek_time, QString file_path) : Task(TaskDecodeOneFrame)
{
    _seek_time = seek_time;
    _file_path = file_path;
    pCodecCtx = NULL;
    ifmt_ctx = NULL;
    pCodec = NULL;
    video_index = Task::videoIndex();
    setAutoDelete(true);
}

GetOneFrameThread::~GetOneFrameThread()
{

}

void GetOneFrameThread::threadRun()
{
    av_register_all();
    int rotate = Task::rotate;
    ifmt_ctx = Task::pop();
    if(ifmt_ctx == NULL)
    {
        ifmt_ctx = avformat_alloc_context();
        std::shared_ptr<char> filePath(new char[strlen(_file_path.toStdString().c_str()) + 1],
                [](char* p) {delete[] p; });

        strcpy(filePath.get(), _file_path.toStdString().c_str());
        if(avformat_open_input(&ifmt_ctx,filePath.get(),NULL,NULL) != 0) //打开文件
        {
            qDebug() << "open file failed";
            avformat_free_context(ifmt_ctx);
            ifmt_ctx = NULL;
            emit decodeFailed(0,FRAME());
            emit threadFinished(this);
            return;
        }
        if(avformat_find_stream_info(ifmt_ctx,NULL) < 0){
            emit decodeFailed(0,FRAME());
            emit threadFinished(this);
            Task::push(ifmt_ctx);
            return;
        }
    }
    if(video_index < 0)
    {
        qDebug() << "Cant find stream info";
        emit decodeFailed(0,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }
    AVStream *videoStream = ifmt_ctx->streams[video_index];
    pCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (pCodec == nullptr ||  videoStream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
        qWarning("can't find codecr :%s", avcodec_get_name(videoStream->codecpar->codec_id));
        emit decodeFailed(0,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }
    bool hwSurported = false;
    bool hwAviable = false;
    AVHWDeviceType HWType = AV_HWDEVICE_TYPE_NONE;

    if(videoStream->codecpar->codec_id == AV_CODEC_ID_H264 /*|| videoStream->codecpar->codec_id == AV_CODEC_ID_HEVC*/ && Task::HWDeviceName != "")
    {
        const char* HWDevice = Task::HWDeviceName.toStdString().c_str();
        HWType = av_hwdevice_find_type_by_name(HWDevice);
        if (HWType == AV_HWDEVICE_TYPE_NONE) {
            qWarning("device type %s is not supported", HWDevice);
            hwSurported = false;
        }
        else
            hwSurported = true;
        for (int i = 0; ; i++) {
            const AVCodecHWConfig *config = avcodec_get_hw_config(pCodec, i);
            if (config == nullptr) {
                qWarning("decoder %s does not support device type %s", pCodec->name, av_hwdevice_get_type_name(HWType));
                if(QThreadPool::globalInstance()->maxThreadCount() == 1)
                    QThreadPool::globalInstance()->setMaxThreadCount(3);
                hwAviable = false;
                break;
            }

            if ((config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX) && (config->device_type == HWType)) {
                mutex.lock();
                HWPixFmt = config->pix_fmt;
                mutex.unlock();
                hwAviable = true;
                break;
            }
        }
    }
label1:
    if(pCodecCtx != NULL)
    {
        avcodec_close(pCodecCtx);
        pCodecCtx = NULL;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(pCodecCtx == NULL)
    {
        qDebug() << "Cant find codecs";
        emit decodeFailed(0,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }
    if(avcodec_parameters_to_context(pCodecCtx, videoStream->codecpar) < 0)
    {
        qWarning("get codec parameters failed");
        emit decodeFailed(0,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }
    AVDictionary *param = NULL;
//    hwSurported = false;
    if(hwSurported && hwAviable)
    {
        if(av_hwdevice_ctx_create(&hwDecodeCtx,HWType,nullptr,nullptr,0) < 0)
        {
            qWarning("failed to create specified hw device");
            hwAviable = false;
            goto label1;
        }
        pCodecCtx->hw_device_ctx = av_buffer_ref(hwDecodeCtx);
    }
    else{
        if(pCodec->id == AV_CODEC_ID_H264)
        {
            av_dict_set(&param,"preset","ultrafast",0);
            av_dict_set(&param,"tune","fastdecode",0);
        }
    }
    if(avcodec_open2(pCodecCtx,pCodec,&param) != 0)
    {
        qDebug() << "codec open failed";
        emit decodeFailed(0,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }

    SwsContext* img_convert_ctx = NULL;
    AVFrame* pFrame = NULL ;
    AVFrame* pFrameYUV = NULL;
    AVFrame* hwFrame = NULL;
    AVFrame* tmpFrame = NULL;
    uint8_t* out_buffer = NULL;
    //初始化解码结构体
//    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    img_convert_ctx = NULL;

    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    hwFrame = av_frame_alloc();
    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24,  Task::pictureSize().width(), Task::pictureSize().height(),1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
                         AV_PIX_FMT_RGB24,Task::pictureSize().width(), Task::pictureSize().height(),1);
//    AVRational timeBase = videoStream->time_base;
    int64_t seekTime = videoStream->start_time + ((double)_seek_time / 1000) * videoStream->time_base.den / videoStream->time_base.num;
    int rst = av_seek_frame(ifmt_ctx,video_index,seekTime,AVSEEK_FLAG_BACKWARD);
    AVPacket dec_pack;
    int frames = 0;
    if(rst < 0)
    {
        emit decodeFailed(0,FRAME());
        goto end;
    }
    avcodec_flush_buffers(pCodecCtx);
    while(av_read_frame(ifmt_ctx,&dec_pack) >= 0)
    {
        if(isThreadStoped())
        {
            goto end;
        }
        if(dec_pack.stream_index == video_index)
        {
            int ret = avcodec_send_packet(pCodecCtx,&dec_pack);
            if(ret != 0)
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            bool got_frame = false;
            while(1)
            {
                if(isThreadStoped())
                {
                    goto end;
                }
                ret = avcodec_receive_frame(pCodecCtx,pFrame);
                if (ret == 0)
                {
                    got_frame = true;
                    break;
                }
                if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0)
                {
                    got_frame = false;
                    break;
                }
            }
            if(!got_frame)
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            if(pFrame->pts < seekTime)
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            frames++;
            if(frames > 10) //当前frame在_seek_time之后的10帧以上
            {
                emit decodeFailed(0,FRAME());
                goto end;
            }
            if(hwSurported && hwAviable)
            {

                if(pFrame->format == HWPixFmt){
                    if(av_hwframe_transfer_data(hwFrame,pFrame,0) < 0)
                    {
                        av_buffer_unref(&hwDecodeCtx);
                        av_packet_unref(&dec_pack);
                        av_frame_free(&pFrame);
                        av_frame_free(&pFrameYUV);
                        av_frame_free(&hwFrame);
                        av_free(out_buffer);
                        sws_freeContext(img_convert_ctx);
                        avcodec_close(pCodecCtx);
                        hwAviable = false;
                        goto label1;
                    }
                    tmpFrame = hwFrame;
                }
                else
                {
                    tmpFrame = pFrame;
                }
            }
            else
            {
                tmpFrame = pFrame;
            }
            tmpFrame->pts = pFrame->pts;

            if(img_convert_ctx == NULL)
                img_convert_ctx = sws_getContext(tmpFrame->width, tmpFrame->height, (AVPixelFormat)tmpFrame->format,
                                                 Task::pictureSize().width(), Task::pictureSize().height(), AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
            sws_scale(img_convert_ctx, (const unsigned char* const*)tmpFrame->data, tmpFrame->linesize, 0, tmpFrame->height,
                      pFrameYUV->data, pFrameYUV->linesize);
            QImage imagetmp((uchar*)out_buffer,Task::pictureSize().width(), Task::pictureSize().height(),pFrameYUV->linesize[0],QImage::Format_RGB888);
            if(imagetmp.isNull())
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            QTransform transform;
            transform.rotate(rotate);
            QImage img = imagetmp.transformed(transform);
            QByteArray data;
            QBuffer buff(&data);
            buff.open(QIODevice::WriteOnly);
            img.save(&buff,"png");
            FRAME frame;
            frame.time_pts = _seek_time;
            frame.frame_pts = tmpFrame->pts * av_q2d(videoStream->time_base) * 1000;
            frame.frameData = data;
            emit decodeOneFrame(frame);
            break;
        }
        av_packet_unref(&dec_pack);
    }   //while end
end:
    if(hwSurported && hwAviable)
        av_buffer_unref(&hwDecodeCtx);
    av_packet_unref(&dec_pack);
    av_frame_free(&pFrame);
    av_frame_free(&pFrameYUV);
    av_frame_free(&hwFrame);
    av_free(out_buffer);
    sws_freeContext(img_convert_ctx);
    avcodec_close(pCodecCtx);
    Task::push(ifmt_ctx);
//    avformat_close_input(&ifmt_ctx);
//    avformat_free_context(ifmt_ctx);
    emit threadFinished((Task*)this);
}


GetMediaInfoThread::GetMediaInfoThread(QString file_path, QObject *parent) : QObject(parent)
{
    _file_path = file_path;
    pCodecCtx = NULL;
    ifmt_ctx = NULL;
    pCodec = NULL;
    video_index = -1;
    setAutoDelete(true);
}

void GetMediaInfoThread::run()
{
    ifmt_ctx = Task::pop();
    if(ifmt_ctx == NULL)
    {
        ifmt_ctx = avformat_alloc_context();
        std::shared_ptr<char> filePath(new char[strlen(_file_path.toStdString().c_str()) + 1],
                [](char* p) {delete[] p; });

        strcpy(filePath.get(), _file_path.toStdString().c_str());
        if(avformat_open_input(&ifmt_ctx,filePath.get(),NULL,NULL) != 0) //打开文件
        {
            qDebug() << "open file failed";
            return;
        }
        if(avformat_find_stream_info(ifmt_ctx,NULL) < 0){
            Task::push(ifmt_ctx);
            return;
        }
    }
    //找到相关的流索引
    for(int i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        //默认第一个视频流
        if(ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_index = i;
            break;
        }
    }
    if(video_index < 0)
    {
        qDebug() << "Cant find stream info";
        Task::push(ifmt_ctx);
        return;
    }
    Task::setVideoIndex(video_index);
    pCodecCtx = ifmt_ctx->streams[video_index]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    MediaEditInfo info;
    AVDictionaryEntry *tag = NULL;
    tag = av_dict_get(ifmt_ctx->streams[video_index]->metadata, "rotate", tag, 0);
    if(pCodecCtx == NULL || pCodec == NULL)
    {
        qDebug() << "Cant find codecs";
        goto end;
    }

    if(tag != NULL) {
        int angle = atoi(tag->value);
        if(angle == 90 || angle == 270) {
            info.isTurn = true;
        }
        info.rotate = angle;
    }
    Task::turn = info.isTurn;
    Task::rotate = info.rotate;
    info.totalTime = (ifmt_ctx->duration + 5000) / AV_TIME_BASE * 1000;
    info.width = pCodecCtx->width;
    info.height = pCodecCtx->height;
    info.fps = (double)ifmt_ctx->streams[video_index]->avg_frame_rate.num / ifmt_ctx->streams[video_index]->avg_frame_rate.den;
    emit getMediaInfo(info);
end:
    avcodec_close(pCodecCtx);
    Task::push(ifmt_ctx);
}

void GetMediaInfoThread::stopThread()
{
    threadStop = true;
}

GetFramesThread::GetFramesThread(int64_t startTime, int64_t endTime, int frameCount, QString file_path) : Task(TaskDecodeAllFrame)
{
    _startTime = startTime;
    _endTime = endTime;
    _frameCount = frameCount;
    _file_path = file_path;
    _frameDuration = (_endTime - _startTime) / _frameCount;
    pCodecCtx = NULL;
    ifmt_ctx = NULL;
    pCodec = NULL;
    video_index = -1;
    setAutoDelete(true);
}

GetFramesThread::~GetFramesThread()
{

}

void GetFramesThread::threadRun()
{
    av_register_all();
    ifmt_ctx = avformat_alloc_context();
    AVDictionary *p = NULL;
    bool bTurn = false;
    int rotate = 0;
    std::shared_ptr<char> filePath(new char[strlen(_file_path.toStdString().c_str()) + 1],
            [](char* p) {delete[] p; });

    strcpy(filePath.get(), _file_path.toStdString().c_str());
    if(avformat_open_input(&ifmt_ctx,filePath.get(),NULL,&p) != 0) //打开文件
    {
        qDebug() << "open file failed";
        avformat_free_context(ifmt_ctx);
        emit threadFinished(this);
        return;
    }
    if(avformat_find_stream_info(ifmt_ctx,NULL) < 0){
        qDebug() << "Could't find stream info";
        avformat_close_input(&ifmt_ctx);
        avformat_free_context(ifmt_ctx);
        emit threadFinished(this);
        return;
    }
    //找到相关的流索引
    for(int i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        //默认第一个视频流
        if(ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_index = i;
            break;
        }
    }
    if(video_index < 0)
    {
        qDebug() << "Cant find stream info";
        emit threadFinished(this);
        return;
    }
    AVDictionaryEntry *tag = NULL;
    tag = av_dict_get(ifmt_ctx->streams[video_index]->metadata, "rotate", tag, 0);

    if(tag != NULL) {
        int angle = atoi(tag->value);
        if(angle == 90 || angle == 270) {
            bTurn = true;
        }
        rotate = angle;
    }

    pCodecCtx = ifmt_ctx->streams[video_index]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodecCtx == NULL || pCodec == NULL)
    {
        qDebug() << "Cant find codecs";
        avformat_close_input(&ifmt_ctx);
        avformat_free_context(ifmt_ctx);
        emit threadFinished(this);
        return;
    }
    AVDictionary *param = NULL;
    if(pCodec->id == AV_CODEC_ID_H264)
    {
        av_dict_set(&param,"preset","ultrafast",0);
        av_dict_set(&param,"tune","fastdecode",0);
    }

    if(avcodec_open2(pCodecCtx,pCodec,&param) != 0)
    {
        qDebug() << "codec open failed";
        avcodec_close(pCodecCtx);
        avformat_close_input(&ifmt_ctx);
        avformat_free_context(ifmt_ctx);
        emit threadFinished(this);
        return;
    }

    SwsContext* img_convert_ctx = NULL;
    AVFrame* pFrame = NULL ;
    AVFrame* pFrameYUV = NULL;
    uint8_t* out_buffer = NULL;
    //初始化解码结构体
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32,  pCodecCtx->width, pCodecCtx->height,1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
                         AV_PIX_FMT_RGB32,pCodecCtx->width, pCodecCtx->height,1);
    int64_t seek_time = ((double)_startTime / 1000) / av_q2d(ifmt_ctx->streams[video_index]->time_base);
    int rst = av_seek_frame(ifmt_ctx,video_index,seek_time,AVSEEK_FLAG_BACKWARD);
    AVPacket dec_pack;
    int curFrameNum = 0;
    if(rst < 0)
        goto end;
    while(av_read_frame(ifmt_ctx,&dec_pack) >= 0 && curFrameNum < _frameCount)
    {
        if(isThreadStoped())
            goto end;
        if(dec_pack.stream_index == video_index)
        {
            int got_picture = 0;

            int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &dec_pack);
            if(ret <= 0)
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            int64_t pts = pFrame->pts * av_q2d(ifmt_ctx->streams[video_index]->time_base) * 1000;
            int64_t duration = _frameDuration;
            if(got_picture && pts >= _startTime /*&& pts + duration >= _startTime*/){

                sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);
                QImage imagetmp((uchar*)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                if(imagetmp.isNull())
                {
                    av_packet_unref(&dec_pack);
                    continue;
                }
                QTransform transform;
                transform.rotate(rotate);
                imagetmp = imagetmp.transformed(transform);
                QImage img = imagetmp.scaled(Task::pictureSize());
                QByteArray data;
                QBuffer buff(&data);
                buff.open(QIODevice::WriteOnly);
                img.save(&buff,"png",100);
                FRAME frame;
                frame.time_pts = _startTime;
                frame.frame_pts = pFrame->pts * av_q2d(ifmt_ctx->streams[video_index]->time_base) * 1000;
                frame.frameDration = duration;
                frame.frameData = data;
//                qDebug() << QThread::currentThreadId() << curFrameNum << frame.time_pts << frame.frame_pts << frame.frameDration << threadStop;
                emit decodeOneFrame(curFrameNum,frame);
                curFrameNum++;

                _startTime += _frameDuration;
                seek_time = ((double)_startTime / 1000) / av_q2d(ifmt_ctx->streams[video_index]->time_base);
                av_seek_frame(ifmt_ctx,video_index,seek_time,AVSEEK_FLAG_BACKWARD);
            }
        }
        av_packet_unref(&dec_pack);
    }   //while end
end:
    av_packet_unref(&dec_pack);
    av_frame_free(&pFrame);
    av_frame_free(&pFrameYUV);
    av_free(out_buffer);
    sws_freeContext(img_convert_ctx);
    avcodec_close(pCodecCtx);
    avformat_close_input(&ifmt_ctx);
    avformat_free_context(ifmt_ctx);
    emit threadFinished((Task*)this);
}



RequestOneFrameThread::RequestOneFrameThread(int64_t seek_time, int index, QString filepath) : Task(Task::TaskRequestOneFrame)
{
    _seek_time = seek_time;
    _index = index;
    _file_path = filepath;
    pCodecCtx = NULL;
    ifmt_ctx = NULL;
    pCodec = NULL;
    video_index = Task::videoIndex();
    hwDecodeCtx = NULL;
    setAutoDelete(true);
}

RequestOneFrameThread::~RequestOneFrameThread()
{

}

void RequestOneFrameThread::threadRun()
{
    av_register_all();
    int rotate = Task::rotate;
    ifmt_ctx = Task::pop();
    if(ifmt_ctx == NULL)
    {
        ifmt_ctx = avformat_alloc_context();
        std::shared_ptr<char> filePath(new char[strlen(_file_path.toStdString().c_str()) + 1],
                [](char* p) {delete[] p; });

        strcpy(filePath.get(), _file_path.toStdString().c_str());
        if(avformat_open_input(&ifmt_ctx,filePath.get(),NULL,NULL) != 0) //打开文件
        {
            qDebug() << "open file failed";
            avformat_free_context(ifmt_ctx);
            ifmt_ctx = NULL;
            emit decodeFailed(_index,FRAME());
            emit threadFinished(this);
            return;
        }
        if(avformat_find_stream_info(ifmt_ctx,NULL) < 0){
            emit decodeFailed(_index,FRAME());
            emit threadFinished(this);
            Task::push(ifmt_ctx);
            return;
        }
    }
    if(video_index < 0)
    {
        qDebug() << "Cant find stream info";
        emit decodeFailed(_index,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }
    AVStream *videoStream = ifmt_ctx->streams[video_index];
    pCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (pCodec == nullptr ||  videoStream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
        qWarning("can't find codecr :%s", avcodec_get_name(videoStream->codecpar->codec_id));
        emit decodeFailed(_index,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }
    bool hwSurported = false;
    bool hwAviable = false;
    AVHWDeviceType HWType = AV_HWDEVICE_TYPE_NONE;

    if(videoStream->codecpar->codec_id == AV_CODEC_ID_H264 /*|| videoStream->codecpar->codec_id == AV_CODEC_ID_HEVC*/ && Task::HWDeviceName != "")
    {
        const char* HWDevice = Task::HWDeviceName.toStdString().c_str();
        HWType = av_hwdevice_find_type_by_name(HWDevice);
        if (HWType == AV_HWDEVICE_TYPE_NONE) {
            qWarning("device type %s is not supported", HWDevice);
            hwSurported = false;
        }
        else
            hwSurported = true;
        for (int i = 0; ; i++) {
            const AVCodecHWConfig *config = avcodec_get_hw_config(pCodec, i);
            if (config == nullptr) {
                qWarning("decoder %s does not support device type %s", pCodec->name, av_hwdevice_get_type_name(HWType));
                hwAviable = false;
                break;
            }

            if ((config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX) && (config->device_type == HWType)) {
                mutex.lock();
                HWPixFmt = config->pix_fmt;
                mutex.unlock();
                hwAviable = true;
                break;
            }
        }
    }
label1:
    if(pCodecCtx != NULL)
    {
        avcodec_close(pCodecCtx);
        pCodecCtx = NULL;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(pCodecCtx == NULL)
    {
        qDebug() << "Cant find codecs";
        emit decodeFailed(_index,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }
    if(avcodec_parameters_to_context(pCodecCtx, videoStream->codecpar) < 0)
    {
        qWarning("get codec parameters failed");
        emit decodeFailed(_index,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }
    AVDictionary *param = NULL;
//    hwSurported = false;
    if(hwSurported && hwAviable)
    {
        if(av_hwdevice_ctx_create(&hwDecodeCtx,HWType,nullptr,nullptr,0) < 0)
        {
            qWarning("failed to create specified hw device");
            hwAviable = false;
            goto label1;
        }
        pCodecCtx->hw_device_ctx = av_buffer_ref(hwDecodeCtx);
    }
    else{
        if(pCodec->id == AV_CODEC_ID_H264)
        {
            av_dict_set(&param,"preset","ultrafast",0);
            av_dict_set(&param,"tune","fastdecode",0);
        }
    }
    if(avcodec_open2(pCodecCtx,pCodec,&param) != 0)
    {
        qDebug() << "codec open failed";
        emit decodeFailed(_index,FRAME());
        emit threadFinished(this);
        Task::push(ifmt_ctx);
        return;
    }

    SwsContext* img_convert_ctx = NULL;
    AVFrame* pFrame = NULL ;
    AVFrame* pFrameYUV = NULL;
    AVFrame* hwFrame = NULL;
    AVFrame* tmpFrame = NULL;
    uint8_t* out_buffer = NULL;
    //初始化解码结构体
//    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    img_convert_ctx = NULL;

    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    hwFrame = av_frame_alloc();
    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24,  Task::pictureSize().width(), Task::pictureSize().height(),1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
                         AV_PIX_FMT_RGB24,Task::pictureSize().width(), Task::pictureSize().height(),1);
//    AVRational timeBase = videoStream->time_base;
    int64_t seekTime = videoStream->start_time + ((double)_seek_time / 1000) * videoStream->time_base.den / videoStream->time_base.num;
    int rst = av_seek_frame(ifmt_ctx,video_index,seekTime,AVSEEK_FLAG_BACKWARD);
    AVPacket dec_pack;
    int frames = 0;
    if(rst < 0)
    {
        emit decodeFailed(_index,FRAME());
        goto end;
    }
    avcodec_flush_buffers(pCodecCtx);
    while(av_read_frame(ifmt_ctx,&dec_pack) >= 0)
    {
        if(isThreadStoped())
        {
            goto end;
        }
        if(dec_pack.stream_index == video_index)
        {
            int ret = avcodec_send_packet(pCodecCtx,&dec_pack);
            if(ret != 0)
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            bool got_frame = false;
            while(1)
            {
                if(isThreadStoped())
                {
                    goto end;
                }
                ret = avcodec_receive_frame(pCodecCtx,pFrame);
                if (ret == 0)
                {
                    got_frame = true;
                    break;
                }
                if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0)
                {
                    got_frame = false;
                    break;
                }
            }
            if(!got_frame)
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            if(pFrame->pts < seekTime)
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            frames++;
            if(frames > 10) //当前frame在_seek_time之后的10帧以上
            {
                emit decodeFailed(_index,FRAME());
                goto end;
            }
            if(hwSurported && hwAviable)
            {

                if(pFrame->format == HWPixFmt){
                    if(av_hwframe_transfer_data(hwFrame,pFrame,0) < 0)
                    {
                        av_buffer_unref(&hwDecodeCtx);
                        av_packet_unref(&dec_pack);
                        av_frame_free(&pFrame);
                        av_frame_free(&pFrameYUV);
                        av_frame_free(&hwFrame);
                        av_free(out_buffer);
                        sws_freeContext(img_convert_ctx);
                        avcodec_close(pCodecCtx);
                        hwAviable = false;
                        goto label1;
                    }
                    tmpFrame = hwFrame;
                }
                else
                {
                    tmpFrame = pFrame;
                }
            }
            else
            {
                tmpFrame = pFrame;
            }
            tmpFrame->pts = pFrame->pts;

            if(img_convert_ctx == NULL)
                img_convert_ctx = sws_getContext(tmpFrame->width, tmpFrame->height, (AVPixelFormat)tmpFrame->format,
                                                 Task::pictureSize().width(), Task::pictureSize().height(), AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
            sws_scale(img_convert_ctx, (const unsigned char* const*)tmpFrame->data, tmpFrame->linesize, 0, tmpFrame->height,
                      pFrameYUV->data, pFrameYUV->linesize);
            QImage imagetmp((uchar*)out_buffer,Task::pictureSize().width(), Task::pictureSize().height(),pFrameYUV->linesize[0],QImage::Format_RGB888);
            if(imagetmp.isNull())
            {
                av_packet_unref(&dec_pack);
                continue;
            }
            QTransform transform;
            transform.rotate(rotate);
            QImage img = imagetmp.transformed(transform);
            QByteArray data;
            QBuffer buff(&data);
            buff.open(QIODevice::WriteOnly);
            img.save(&buff,"png");
            FRAME frame;
            frame.time_pts = _seek_time;
            frame.frame_pts = tmpFrame->pts * av_q2d(videoStream->time_base) * 1000;
            frame.frameData = data;
            emit decodeOneFrame(_index,frame);
            break;
        }
        av_packet_unref(&dec_pack);
    }   //while end
end:
    if(hwSurported && hwAviable)
        av_buffer_unref(&hwDecodeCtx);
    av_packet_unref(&dec_pack);
    av_frame_free(&pFrame);
    av_frame_free(&pFrameYUV);
    av_frame_free(&hwFrame);
    av_free(out_buffer);
    sws_freeContext(img_convert_ctx);
    avcodec_close(pCodecCtx);
    Task::push(ifmt_ctx);
//    avformat_close_input(&ifmt_ctx);
//    avformat_free_context(ifmt_ctx);
    emit threadFinished((Task*)this);
}

int RequestOneFrameThread::requestIndex()
{
    return _index;
}

void Task::setPictureSize(QSize size)
{
    _pictureSize = QSize(size.width() * 2,size.height() * 2);
}

QSize Task::pictureSize()
{
    return _pictureSize;
}

void Task::setVideoIndex(int index)
{
    mutex.lock();
    _video_index = index;
    mutex.unlock();
}

int Task::videoIndex()
{
    return _video_index;
}

void Task::cleanFormatList()
{
    foreach (AVFormatContext *fmtCtx, formatList) {
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
    }
    formatList.clear();
}

AVFormatContext *Task::pop()
{
    mutex.lock();
    AVFormatContext *fmtCtx = NULL;
    if(formatList.isEmpty())
    {
        mutex.unlock();
        return fmtCtx;
    }

    fmtCtx = formatList.takeFirst();
    mutex.unlock();
    return fmtCtx;

}

void Task::push(AVFormatContext *fmtCtx)
{
    mutex.lock();
    formatList.append(fmtCtx);
    mutex.unlock();
}


