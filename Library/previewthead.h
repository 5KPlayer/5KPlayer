#ifndef PREVIEWTHEAD_H
#define PREVIEWTHEAD_H

#include <QObject>
#include <QThread>

class AVFormatContext;
class PreviewThead : public QThread
{
    Q_OBJECT
public:
    explicit PreviewThead(QObject *parent = 0);

    void setFilePathandTime(QString file, int64_t time);
    void stopffmpeg();
signals:
    void sendPicture(QString path,int64_t time,QByteArray data);
private:
    QByteArray getScreenByte();
protected:
     void run();
private:
    AVFormatContext *   _pFormatCtx = NULL;
    int                 _firstVideoStream;
    QString _filePath       = "";
    QString _openFile       = "";
    int64_t _time = 0;
};

#endif // PREVIEWTHEAD_H
