#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <QRunnable>
#include <QObject>
#include <QThread>
#include "dystructu.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
#include <libavutil/hwcontext.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libavutil/timecode.h>
#include <libavutil/bprint.h>
}
#include <QThreadPool>
#include <QDebug>

#define MAX_FRAME_WIDTH 200
#define MIN_FRAME_WIDTH 50

class Task : public QObject, public QRunnable
{
    Q_OBJECT
public:
    enum TaskType{
        TaskDecodeOneFrame,
        TaskDecodeAllFrame,
        TaskRequestOneFrame,
    };

    Task(TaskType type) : _type(type)
    {}
    virtual ~Task()
    {}
    virtual TaskType type()
    {
        return _type;
    }
    void stopThread()
    {
        threadStop = true;
    }
    bool isThreadStoped(){
        return threadStop;
    }

    void run(){
        if(isThreadStoped())
        {
            emit threadFinished(this);
            return;
        }
        threadRun();
    }
    virtual void threadRun() = 0;
    static void setPictureSize(QSize size);
    static QSize pictureSize();
    static void setVideoIndex(int index);
    static int videoIndex();
    static void cleanFormatList();
    static AVFormatContext* pop();
    static void push(AVFormatContext* fmtCtx);
    static QList<AVFormatContext*> formatList;
    static int rotate;
    static bool turn;
    static QString HWDeviceName;
signals:
    void threadFinished(Task* task);
    void error(int err_id,QString err_string);
    void decodeFailed(int index,FRAME frame);
private:
    TaskType _type;
    bool threadStop = false;
    static QSize _pictureSize;
    static int _video_index;
};

class GetOneFrameThread : public Task
{
    Q_OBJECT
public:
    explicit GetOneFrameThread(int64_t seek_time,QString file_path);
    ~GetOneFrameThread();
    void threadRun();
signals:
    void decodeOneFrame(FRAME frame);

private:
    AVCodecContext *pCodecCtx;
    AVFormatContext *ifmt_ctx;
    AVCodec *pCodec;
    int video_index;
    QString _file_path;
    int64_t _seek_time = 0;
    AVBufferRef *hwDecodeCtx;
};

class RequestOneFrameThread : public Task
{
    Q_OBJECT
public:
    explicit RequestOneFrameThread(int64_t seek_time,int index, QString filepath);
    ~RequestOneFrameThread();
    void threadRun();
    int requestIndex();
signals:
    void decodeOneFrame(int index,FRAME frame);

private:
    AVCodecContext *pCodecCtx;
    AVFormatContext *ifmt_ctx;
    AVCodec *pCodec;
    int video_index;
    QString _file_path;
    int64_t _seek_time = 0;
    int _index = -1;
    AVBufferRef *hwDecodeCtx;
};

class GetMediaInfoThread :public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit GetMediaInfoThread(QString file_path,QObject *parent = 0);
    void run();
    void stopThread();
signals:
    void getMediaInfo(MediaEditInfo info);

private:
    AVCodecContext *pCodecCtx;
    AVFormatContext *ifmt_ctx;
    AVCodec *pCodec;
    int video_index;
    QString _file_path;
    bool threadStop = false;
};

class GetFramesThread :public Task
{
    Q_OBJECT
public:
    explicit GetFramesThread(int64_t startTime,int64_t endTime,int frameCount,QString file_path);
    ~GetFramesThread();
    void threadRun();
signals:
    void decodeOneFrame(int index,FRAME frame);
private:
    AVCodecContext *pCodecCtx;
    AVFormatContext *ifmt_ctx;
    AVCodec *pCodec;
    int video_index;
    QString _file_path;

    int64_t _startTime;
    int64_t _endTime;
    int64_t _frameCount;
    int64_t _frameDuration;
};

#endif // DECODETHREAD_H
