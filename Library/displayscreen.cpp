#include "displayscreen.h"
#include <windows.h>
#include <memory>
#include <QImage>
#include <QBuffer>
#include <QtDebug>

extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/frame.h"
#include "libavutil/avstring.h"
#include "libavutil/imgutils.h"
}

displayScreen::displayScreen(QObject *parent) : QObject(parent)
{
    // Register all formats and codecs
    av_register_all();
}

int displayScreen::supQsvDecoder(const QString &file, const QString &codeName)
{
    int sourec = -1;
    av_register_all();

    std::shared_ptr<char> filePath(new char[strlen(file.toStdString().c_str()) + 1],
            [](char* p) {delete[] p; });

    strcpy(filePath.get(), file.toStdString().c_str());

    AVFormatContext *ptr = avformat_alloc_context();
    int rt = avformat_open_input(&ptr, filePath.get(), NULL, NULL);//打开文件
    std::shared_ptr<AVFormatContext> pFormatCtx(ptr, [](AVFormatContext *ctx) {
        avformat_close_input(&ctx);
        avformat_free_context(ctx);
    });

    if (rt != 0) {
        return -1;//文件打开失败
    }

    if (avformat_find_stream_info(pFormatCtx.get(), NULL) < 0) {
        return -2; //流信息读取失败
    }

    //找第一条视频流
    int videoStream = -1;
    for (unsigned int i = 0; i < pFormatCtx.get()->nb_streams; i++) {
        if (pFormatCtx.get()->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) {
        return -3;//视频流寻找失败
    }

    //AVCodec *pCodecx = avcodec_find_decoder(pFormatCtx.get()->streams[videoStream]->codec->codec_id);
    //std::cout << pCodecx->name;

    AVCodec *pCodec = avcodec_find_decoder_by_name(codeName.toStdString().c_str());
    if (pCodec == NULL) {
        return -4;//解码器寻找失败
    }

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        return -5;
    }

    //av_opt_set(pCodecCtx->priv_data, "-vf", "hflip", 0);

    avcodec_parameters_to_context(pCodecCtx, pFormatCtx.get()->streams[videoStream]->codecpar);
    // Open codec
    if (avcodec_open2(pCodecCtx, NULL, NULL) < 0) {
        return -7; // 解码器打开失败
    }

    AVFrame *pFrame = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();
    AVFrame *pFrameRGB = av_frame_alloc();

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);
    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer,
            AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);

    // Read frames and save first five frames to disk
    while(av_read_frame(pFormatCtx.get(), packet)>=0) {
        // Is this a packet from the video stream?
        if(packet->stream_index==videoStream) {
            avcodec_send_packet(pCodecCtx, packet);// Decode video frame
            if(avcodec_receive_frame(pCodecCtx, pFrame) == 0)
            {
                if(pFrame->key_frame==1) {//这里取到关键帧数据
                    sourec = 1;
                    break;
                }
            }
        }
    // Free the packet that was allocated by av_read_frame
    av_packet_unref(packet);
    }
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    return sourec;
}
