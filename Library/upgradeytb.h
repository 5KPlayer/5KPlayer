#ifndef UPGRADEYTB_H
#define UPGRADEYTB_H

#include <QObject>
#include "singleton.h"

class QFile;
class QNetworkReply;

#define UpYTB UpGradeYtb::getInstance()

class UpGradeYtb : public QObject,public Singleton<UpGradeYtb>
{
    Q_OBJECT
    friend class Singleton<UpGradeYtb>;
    friend class QSharedPointer<UpGradeYtb>;
public:
    explicit UpGradeYtb(QObject *parent = 0);

    void checkUpGrade(bool ischeck);
    void ytbExtract();

    void ytdDownCancel();
private:
    Q_DISABLE_COPY(UpGradeYtb)
    void startUpGrade();            // 使用config文件中的下载地址进行下载，多个网络地址时，随机使用一个。
    void startUpGradeUserOldDown(); // 使用老的地址进行下载文件
    void checkUpGradeUserOldWeb();  // 使用老的web地址进行检查更新
signals:
    void ytbUpProgress(int progress);
    void ytbUpFinish();
    void ytbUpFail();
private slots:
    void httpReadyRead();       // 写入文件
    void httpFinished();        // 下载完成

    void httpHandReadyRead();
    void httpHandFinished();
    void ytbDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
private:
    QNetworkReply        *  _handReply  = NULL;
    QNetworkReply        *  _downReply  = NULL;
    QFile*                  _file;
    QByteArray              _ytbData;
    QString                 _fileWeb;
    QStringList             _ytbDownWebList;
    QString                 _filePath;
    QString                 _lastDate;
    bool                    _isCheck;           // 弹出dialog的时候， 必定会下载，自动检查的时候需要检查最后更新时间
    bool                    _useOldDown;        // 使用老的下载链接下载的标记，如果使用老的下载标记还是下载错误，则弹出网络错误的dialog
    bool                    _useOldWeb;         // 使用老的web解析
};

#endif // UPGRADEYTB_H
