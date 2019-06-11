#include "displayaspectratio.h"
#include <memory>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QImage>
#include <math.h>
#include <QFileInfo>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#include <QBuffer>
#include <QTextCodec>
extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/frame.h"
#include "libavutil/avstring.h"
#include "libavutil/imgutils.h"
}

DisplayAspectRatio::DisplayAspectRatio()
{
    av_register_all();
}

DisplayAspectRatio::~DisplayAspectRatio()
{
}

int DisplayAspectRatio::getmediainfo(const QString &fileName, MEDIAINFO *media)
{
    AVCodecContext    *pCodecCtx = NULL;
    AVCodec           *pCodec = NULL;
    AVFrame           *pFrame = NULL;
    AVFrame           *pFrameRGB = NULL;
    AVPacket          *packet = NULL;
    int               firstVideoStream;
    struct SwsContext *sws_ctx = NULL;
    bool              bTurn = false; // 是否需要交互宽高比例标记
    bool              bCover = true;
    bool              bOnlySubStream = true;

    // Register all formats and codecs
    av_register_all();

    // Open video file
    std::shared_ptr<char> filePath(new char[strlen(fileName.toStdString().c_str()) + 1],
                [](char* p) {delete[] p; });

    strcpy(filePath.get(), fileName.toStdString().c_str());

    AVFormatContext *ptr = avformat_alloc_context();
    int rt = avformat_open_input(&ptr, filePath.get(), NULL, NULL);//打开文件
    std::shared_ptr<AVFormatContext> pFormatCtx(ptr, [](AVFormatContext *ctx) {
        avformat_close_input(&ctx);
        avformat_free_context(ctx);
    });

    if (rt != 0) {
        return -1;//文件打开失败
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx.get(), NULL)<0)
        return -1; // Couldn't find stream information

    // Dump information about file onto standard error
    //av_dump_format(pFormatCtx, -1, filePath.get(), 0);

    // Find the first video stream
    firstVideoStream=-1;
    bool firstVideoFlag = true;
    for(unsigned int i=0; i<pFormatCtx->nb_streams; ++i) {
        STREAMINFO streaminfo;
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            streaminfo.streamType = 0;
            bOnlySubStream = false;
            streaminfo.streamId = QString::number(i);
            if(firstVideoFlag) {
                firstVideoFlag = false;
                firstVideoStream = i;
            }

            AVDictionaryEntry *tag = NULL;
            tag = av_dict_get(pFormatCtx->streams[i]->metadata, "rotate", tag, 0);

            if(tag != NULL) {
                media->rotate = atoi(tag->value);
                int angle = atoi(tag->value);
                if(angle == 90 || angle == 270) {
                    bTurn = true;
                }
            }

            if(!(pFormatCtx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) > 0) {
                bCover = false;
            }

            int64_t duration = pFormatCtx->duration + 5000;

            media->duration = duration / AV_TIME_BASE;
            const int w = pFormatCtx->streams[i]->codecpar->width; //宽
            const int h = pFormatCtx->streams[i]->codecpar->height;//高
            const int sar_num = pFormatCtx->streams[i]->codecpar->sample_aspect_ratio.num;
            const int sar_den = pFormatCtx->streams[i]->codecpar->sample_aspect_ratio.den;
            if(bTurn)
                streaminfo.resolution = QSize(h,w);
            else
                streaminfo.resolution = QSize(w,h);

            pCodec=avcodec_find_decoder(pFormatCtx->streams[i]->codecpar->codec_id);
            if(pCodec==NULL) {
                return -1; // Codec not found
            }
            streaminfo.codec = QString(pCodec->name);

            QString aspectRatio;
            if(sar_num == 0 || sar_den == 0 || sar_num == sar_den ) {
                int tmp = gcd(w, h);
                if (bTurn)
                    aspectRatio = QString("%1:%2").arg(h/tmp).arg(w/tmp);
                else
                    aspectRatio = QString("%1:%2").arg(w/tmp).arg(h/tmp);
            } else {
                int dar_num = w * sar_num;
                int dar_den = h * sar_den;
                int tmp = gcd(dar_num, dar_den);
                if(bTurn)
                    aspectRatio = QString("%1:%2").arg(dar_den/tmp).arg(dar_num/tmp);
                else
                    aspectRatio = QString("%1:%2").arg(dar_num/tmp).arg(dar_den/tmp);
            }

            streaminfo.aspectRatio = aspectRatio;
        } else if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            streaminfo.streamType = 1;
            bOnlySubStream = false;
            streaminfo.streamId = QString::number(i);

            AVDictionaryEntry *tag = NULL;
            tag = av_dict_get(pFormatCtx->metadata, "artist", tag, AV_DICT_IGNORE_SUFFIX);
            if(tag != NULL) {
                media->artist = char2Qstring(tag->value);
            }

            int64_t duration = pFormatCtx->duration + 5000;
            if(duration>0)
                media->duration     = duration / AV_TIME_BASE;
            pCodec = avcodec_find_decoder(pFormatCtx->streams[i]->codecpar->codec_id);
            if(pCodec == NULL) {
                return -1; // Codec not found
            }
            streaminfo.codec    = pCodec->name;
            streaminfo.Channel  = pFormatCtx->streams[i]->codecpar->channels;
            streaminfo.rate     = pFormatCtx->streams[i]->codecpar->bit_rate;
            streaminfo.bitRate  = pFormatCtx->streams[i]->codecpar->bit_rate / 1000;
        } else if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
            streaminfo.streamType = 2;
            AVDictionaryEntry *tag = NULL;
            tag = av_dict_get(pFormatCtx->streams[i]->metadata, "LANGUAGE", tag, AV_DICT_IGNORE_SUFFIX);
            if(tag != NULL)
                streaminfo.language = tag->value;
            streaminfo.streamId = QString::number(i);
        }
        if(streaminfo.streamType!= -1)
            media->streamList.append(streaminfo);
    }

    if(bOnlySubStream) {
        return -1;
    }

    media->mediaType = bCover?2:1;

    if(firstVideoStream != -1) {
        pCodec = avcodec_find_decoder(pFormatCtx->streams[firstVideoStream]->codecpar->codec_id);
        if(pCodec==NULL) {
            return -1; // Codec not found
        }
        // Copy context
        pCodecCtx = avcodec_alloc_context3(pCodec);
        if (pCodecCtx == NULL) {
            return -1;
        }
        avcodec_parameters_to_context(pCodecCtx, pFormatCtx.get()->streams[firstVideoStream]->codecpar);
        // Open codec
        if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
            return -1; // Could not open codec

        pFrame = av_frame_alloc();
        packet = av_packet_alloc();
        pFrameRGB = av_frame_alloc();

        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);
        uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

        av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer,
                AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);

        // Read frames and save first five frames to disk

        av_seek_frame(pFormatCtx.get(),-1,20*AV_TIME_BASE  ,AVSEEK_FLAG_BACKWARD);

        while(av_read_frame(pFormatCtx.get(), packet)>=0) {
            // Is this a packet from the video stream?
            if(packet->stream_index==firstVideoStream) {
                avcodec_send_packet(pCodecCtx, packet);// Decode video frame
                if(avcodec_receive_frame(pCodecCtx, pFrame) == 0)
                {
                    if(pFrame->key_frame==1) {//这里取到关键帧数据
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

                    QByteArray ba;
                    QBuffer buf(&ba);
                    buf.open(QIODevice::WriteOnly);

                    if(media->rotate != 0 && !image.isNull()) {
                        QTransform transform;
                        transform.rotate(media->rotate);
                        QImage saveImage = image.transformed(transform);
                        saveImage.scaled(300,imageH,Qt::IgnoreAspectRatio, Qt::SmoothTransformation).save(&buf,"png");
                    } else {
                        image.scaled(300,imageH,Qt::IgnoreAspectRatio, Qt::SmoothTransformation).save(&buf,"png");
                    }
                    media->screenshot = ba;
                    break;
                    }
                }
            }

        // Free the packet that was allocated by av_read_frame
        av_packet_unref(packet);
        }

        // Free the YUV frame
        av_frame_free(&pFrame);

        // Close the codecs
        avcodec_close(pCodecCtx);
    }
    return 1;
}

int DisplayAspectRatio::gcd(const int &m, const int &n) const
{
    int temp=0;
    int a = m > n ? m : n;//a取较大数
    int b = m + n - a;    //b取较小数

    while(b) {
        temp = b;
        b = a % b;
        a = temp;
    }
    if(a == 0)
        return 1;
    return a;
}

QString DisplayAspectRatio::char2Qstring(char *value)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = codec->fromUnicode(value);
    int byteLen= ba.size();
    int strLen= 0;  // 需要进行转码的字符长度
    int strByLen = 0; // 需要进行转码的byte长度
    int count = 0;  // 字符的个数
    QString retStr;
    bool isUTF = false;
    for(int i=ba.size()-1;i>=0;--i) {
        if((uint)ba[i] < 128) {
            count++;
        }
    }
    if(count < byteLen) {
        strLen = QString(value).size()-count;
        strByLen = byteLen - count;
    }
    if(strByLen>0 && strLen>0){
        if(strByLen/strLen == 3) { // 能被3整除，是utf-8编码
            isUTF = true;
        }
    }
    if((byteLen == strLen) || isUTF) {//字符长度为1时，是英文字符，是UTF-8 编码 不需要进行转码
        retStr = value;
    } else {
        QString str = codec->toUnicode(ba);
        QByteArray bc= str.toLatin1();
        retStr = QString::fromLocal8Bit(bc.data());
    }
    return retStr;
}
/*
PAR - pixel aspect ratio大多数情况为1:1,就是一个正方形像素，否则为长方形像素

DAR - display aspect ratio就是视频播放时，我们看到的图像宽高的比例，缩放视频也要按这个比例来，否则会使图像看起来被压扁或者拉长了似的。

SAR - storage aspect ratio就是对图像采集时，横向采集与纵向采集构成的点阵，横向点数与纵向点数的比值。

比如VGA图像640/480 = 4:3，D-1 PAL图像720/576 = 5:4

这三者的关系PAR x SAR = DAR或者PAR = DAR/SAR.
*/
