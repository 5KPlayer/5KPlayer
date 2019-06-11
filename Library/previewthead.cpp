#include "previewthead.h"
#include <windows.h>
#include <memory>
#include <QImage>
#include <QBuffer>
#include <QtDebug>

#include <QDateTime>

extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/frame.h"
#include "libavutil/avstring.h"
#include "libavutil/imgutils.h"
}

PreviewThead::PreviewThead(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<int64_t>("int64_t");
}

void PreviewThead::setFilePathandTime(QString file, int64_t time)
{
    _filePath       = file;
    _time           = time;
}

void PreviewThead::stopffmpeg()
{
    if(this->isRunning())
        this->terminate();

    if(_pFormatCtx != NULL) {
        avformat_close_input(&_pFormatCtx);
        _pFormatCtx = NULL;
    }
    _filePath       = "";
    _openFile       = "";
    _time = 0;
}

QByteArray PreviewThead::getScreenByte()
{
    QByteArray screenByte;
    av_register_all();
    if(_openFile == "" || _openFile != _filePath) {
        if(_pFormatCtx != NULL) {
            avformat_close_input(&_pFormatCtx);
            _pFormatCtx = NULL;
        }
        _openFile = _filePath;
        int length = strlen(_filePath.toStdString().c_str());
        std::shared_ptr<char> filePath(new char[length + 1], [](char* p){delete[] p;} );
        strcpy(filePath.get(), _filePath.toStdString().c_str());

        // Open video file
        if(avformat_open_input(&_pFormatCtx, filePath.get(), NULL, NULL)!=0)
            return screenByte; // Couldn't open file

        // Retrieve stream information
        if(avformat_find_stream_info(_pFormatCtx, NULL)<0)
            return screenByte; // Couldn't find stream information

        // Dump information about file onto standard error
        //av_dump_format(_pFormatCtx, -1, filePath.get(), 0);
        _firstVideoStream=-1;
        bool firstVideoFlag = true;
        for(unsigned int i=0; i<_pFormatCtx->nb_streams; ++i) {
            if(_pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
                if(firstVideoFlag) {
                    firstVideoFlag = false;
                    _firstVideoStream = i;
                    break;
                }
            }
        }
    }
    AVCodecContext  *   pCodecCtx = NULL;
    AVCodec         *   pCodec = NULL;
    AVFrame         *   pFrame = NULL;
    AVFrame         *   pFrameRGB = NULL;
    AVPacket        *   packet;

    int                 rotate = 0;
    uint8_t           * buffer = NULL;
    struct SwsContext * sws_ctx = NULL;

    if(_firstVideoStream != -1) {
        AVDictionaryEntry *tag = NULL;
        tag = av_dict_get(_pFormatCtx->streams[_firstVideoStream]->metadata, "rotate", tag, 0);
        if(tag != NULL) {
            rotate = atoi(tag->value);
        }

    pCodec = avcodec_find_decoder(_pFormatCtx->streams[_firstVideoStream]->codecpar->codec_id);
    if (pCodec == NULL) {
        return screenByte;//解码器寻找失败
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        return screenByte;
    }

    avcodec_parameters_to_context(pCodecCtx, _pFormatCtx->streams[_firstVideoStream]->codecpar);
    // Open codec
    if (avcodec_open2(pCodecCtx, NULL, NULL) < 0) {
        return screenByte; // 解码器打开失败
    }

    pFrame = av_frame_alloc();
    packet = av_packet_alloc();

    pFrameRGB = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);
    buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer,
            AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);


    AVRational timeBase = _pFormatCtx->streams[_firstVideoStream]->time_base;
    int64_t seekTime = _pFormatCtx->streams[_firstVideoStream]->start_time + av_rescale(_time, timeBase.den, timeBase.num);
        av_seek_frame(_pFormatCtx,_firstVideoStream,seekTime,AVSEEK_FLAG_BACKWARD);
        while(av_read_frame(_pFormatCtx, packet)>=0) {
            if (packet->stream_index == _firstVideoStream) {
                avcodec_send_packet(pCodecCtx, packet);// Decode video frame
                if(avcodec_receive_frame(pCodecCtx, pFrame) == 0)
                {
                    if(pFrame->key_frame == 1)//关键帧
                    {
                    // initialize SWS context for software scaling
                    sws_ctx = sws_getContext(pCodecCtx->width,
                             pCodecCtx->height,
                             pCodecCtx->pix_fmt,
                             pCodecCtx->width,
                             pCodecCtx->height,
                             AV_PIX_FMT_RGB32,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL
                             );
                    sws_scale (sws_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

                    QImage image((uchar*)pFrameRGB->data[0],pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);


                    int imageH = 200;
                    if(pCodecCtx->width != 0) {
                        imageH = 300*pCodecCtx->height/pCodecCtx->width;
                    }
                    QByteArray ba = NULL;
                    QBuffer buf(&ba);
                    buf.open(QIODevice::ReadWrite);
                    QImage saveImage;
                    saveImage = image.scaled(300,imageH,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    bool isRotate = false;
                    if(rotate != 0) {
                        QTransform transform;
                        transform.rotate(rotate);
                        saveImage.transformed(transform).save(&buf,"png");
                        isRotate = true;
                    }
                    if(!isRotate) {
                        saveImage.save(&buf,"png");
                    }
                    screenByte = ba;
                    break;
                    }
                }
            }

        // Free the packet that was allocated by av_read_frame
        av_packet_unref(packet);
        }

        av_free(buffer);
        av_frame_free(&pFrameRGB);

        // Free the YUV frame
        av_frame_free(&pFrame);

        // Close the codecs
        avcodec_close(pCodecCtx);
    }
    return screenByte;
}

void PreviewThead::run()
{
    if(_time != 0 && !_filePath.isNull()) {
        QByteArray data = getScreenByte();
        if(!data.isNull()) {
            emit sendPicture(_filePath,_time,data);
        }
    }
}
