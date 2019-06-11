#ifndef ADDMEDIATHEAD_H
#define ADDMEDIATHEAD_H

#include <QObject>
#include <QThread>
#include <QUrl>
#include "dystructu.h"
class AddMediaThead : public QThread
{
    Q_OBJECT
public:
    explicit AddMediaThead(QObject *parent = 0);


    void setAddpathList(QStringList pathlist,QList<MEDIAINFO>list, QString itemname,int maxCount);
signals:
    void sendMedialist(int count,QList<MEDIAINFO> medialist);
    void analyzefinish();
private:
    void analyzeMedia();
    MEDIAINFO analyzepath(const QString &path);
public slots:
    void cancelLoad();
private:
    QList<MEDIAINFO> _itemMedia;
    QList<MEDIAINFO> _ytbMediaList;
    QString     _itemName;
    QStringList _pathList;
    int         _maxCount;          // 显示页面最多能显示的媒体数量
    QList<MEDIAINFO> _mediaList;    // 视频媒体的列表

    const int sendCount = 3;    // 每次发送显示的数量
    bool        _iscancel;      // 取消标记
protected:
     void run();
};

#endif // ADDMEDIATHEAD_H
