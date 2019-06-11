#ifndef DLNAANALYTHEAD_H
#define DLNAANALYTHEAD_H

#include <QObject>
#include <QThread>
#include <QUrl>
#include "dystructu.h"
class DLnaAnalyThead : public QThread
{
    Q_OBJECT
public:
    explicit DLnaAnalyThead(QObject *parent = 0);

    void setAddpathList(QStringList pathlist, int showMax);
signals:
    void sendDLnalist(int count,QList<dlnaLib> dlnalist);
    void analyzefinish();
private:
    void analyzeMedia();
    dlnaLib analyzepath(const QString &path);
public slots:
    void cancelLoad();
private:
    QStringList _pathList;
    int         _maxCount;      // 显示页面最多能显示的媒体数
    const int sendCount = 3;    // 每次发送显示的数量
    bool        _iscancel;      // 取消标记
protected:
     void run();
};

#endif // DLNAANALYTHEAD_H
