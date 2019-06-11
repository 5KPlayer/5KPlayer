#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include "updateinterface.h"

class QLabel;
class DYLabel;
class TransEventTextBrowser;

class UpdateWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit UpdateWidget(bool isCheck = true,QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private:
    void init(bool isCheck);
    void checkVersionFinish(bool isUp);
    void setStartLog();

public slots:
    void finishNeedUp(const QByteArray &byteData);      // 检查更新完成，需要更新的信号槽
private slots:
    void checkVersionStart();                           // 开始更新的界面
    void finishNoUp();                                  // 检查更新完成，不需要更新的信号槽
    void errorUp();                                     // 检查更新失败的信号槽
    void clickCheckAgain();                             // 点击再次检查的按钮
    void downUpGrade();                                 // 点击下载更新

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    QLabel*                 _versionUpTitle;
    QLabel*                 _versionUpInfo;
    QLabel*                 _updataLogLab;
    TransEventTextBrowser*  _versionMsg;
    QLabel*                 _handline;
    QLabel*                 _bottonLine;
    DYLabel*                _checkAnginBtn;
    DYLabel*                _upgradeBtn;

    QByteArray              _upinfoByte;
    QString                 _curVersion;
    QString                 _lastVersion;
    QString                 _versionUpLog;
    QString                 _UpgradeWeb;

    QTimer*                 _stratCheckTimer;

    double                  _scaleRatio;
};

#endif // UPDATEWIDGET_H
