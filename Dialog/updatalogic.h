#ifndef UPDATALOGIC_H
#define UPDATALOGIC_H

#include <QObject>
#include <QProcess>
#include "singleton.h"


class QNetworkReply;

#define UpLogic UpdataLogic::getInstance()

class UpdataLogic : public QObject,public Singleton<UpdataLogic>
{
    Q_OBJECT
    friend class Singleton<UpdataLogic>;
    friend class QSharedPointer<UpdataLogic>;

public:
    void startCheckVersion(bool autoCheck);

private:
    Q_DISABLE_COPY(UpdataLogic)
    explicit UpdataLogic(QObject *parent = 0);

    void init();

    int checkUpVersion();           // 返回1，需要更新，返回0，不需要更新，返回2，json信息错误，更新失败
    void startUserOldWebCheck();    // 使用随包的url获取更新信息

signals:
    void upNeedUp(const QByteArray &);      // 需要更新
    void upAutoUP(const QByteArray &);      // 需要更新，这里是自动更新的消息
    void upNoUp();                          // 不需要更新
    void upCheckFail();                     // 检查失败
private slots:
    void httpReadyRead();
    void httpFinished();

private:
    QNetworkReply        *  _reply;
    QByteArray              _versionData;
    bool                    _autoCheck;         // 自动更新的标记，
    bool                    _hasChecked;        // 已经手动检查过的标记，让自动检查不检查
    bool                    _hasUseOld;         // 使用老的web检查过的标记,如果用老的web检查失败，则发送检查失败信息
};

#endif // UPDATALOGIC_H
