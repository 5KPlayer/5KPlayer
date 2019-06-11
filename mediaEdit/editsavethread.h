#ifndef EDITSAVETHREAD_H
#define EDITSAVETHREAD_H

#include <QThread>
#include <QProcess>
#include "editrightwidget.h"

class EditSaveThread : public QThread
{
    Q_OBJECT
public:
    explicit EditSaveThread(CutParams params, QObject *parent = 0);
    void endSaving();               //中断转码
    bool deleteFile();              //转码被中断，删除临时文件

signals:
    void saveProgressAndFPS(int current,int total,int fps);   //转码进度和每秒的fps
    void saveDone();                //转码结束
private:
    QString getCurBitRate();
    bool    startSaveFile(QStringList args);
protected:
    void run();
public slots:

private:
    CutParams _params;
    bool _savingStop;
};

#endif // EDITSAVETHREAD_H
