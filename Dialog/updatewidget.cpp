#include "updatewidget.h"
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QVersionNumber>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QApplication>
#include "TransEventTextBrowser.h"
#include <QScrollBar>
#include "combobox.h"
#include "dylabel.h"
#include "globalarg.h"
#include "configure.h"
#include "updatalogic.h"
#include "playermainwidget.h"

UpdateWidget::UpdateWidget(bool isCheck, QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    init(isCheck);
}

void UpdateWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);
    font.setBold(true);
    _versionUpTitle->setFont(font);
    font.setBold(false);
    _versionUpInfo->setFont(font);
    _updataLogLab->setFont(font);
    _versionMsg->setFont(font);
    _checkAnginBtn->setFont(font);
    _upgradeBtn->setFont(font);

    _checkAnginBtn->resize(180*scaleRatio,20*scaleRatio);
    _upgradeBtn->resize(180*scaleRatio,20*scaleRatio);
    _versionMsg->verticalScrollBar()->setStyleSheet(ComboBox::verticalStyle(scaleRatio,"rgba(0,0,0,0%)"));
}

void UpdateWidget::init(bool isCheck)
{
    _versionUpTitle = new QLabel(this);
    _versionUpInfo  = new QLabel(this);
    _updataLogLab   = new QLabel(this);
    _versionMsg     = new TransEventTextBrowser(this);
    _handline       = new QLabel(this);
    _bottonLine     = new QLabel(this);
    _checkAnginBtn  = new DYLabel(this,true);
    _upgradeBtn     = new DYLabel(this,true);
    _stratCheckTimer= new QTimer(this);

    QVersionNumber cur(Global->_version_maj, Global->_version_min);
    _curVersion = cur.toString();

    _checkAnginBtn->setText(Lge->getLangageValue("UpDate/checkagain"));
    _upgradeBtn->setText(Lge->getLangageValue("UpDate/update"));
    _versionUpTitle->setStyleSheet("color: rgb(222, 222, 222);");
    _versionUpInfo->setStyleSheet("color: rgb(222, 222, 222);");
    _updataLogLab->setStyleSheet("color: rgb(222,222,222);");

    QPalette p1;
    p1.setColor(QPalette::Text, QColor(115,117,128));
    p1.setColor(QPalette::WindowText, QColor(115,117,128));


    _versionMsg->setFrameShape(QFrame::NoFrame);
    _versionMsg->setPalette(p1);
    _versionMsg->setReadOnly(true);

    _versionMsg->setStyleSheet("color: rgb(222, 222, 222);");

    _handline->setStyleSheet("background-color: rgb(255, 255, 255);");
    _bottonLine->setStyleSheet("background-color: rgb(255, 255, 255);");
    _versionMsg->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    _updataLogLab->setText(Lge->getLangageValue("UpDate/versionLogLab"));
    _checkAnginBtn->setAlignment(Qt::AlignCenter);
    _upgradeBtn->setAlignment(Qt::AlignCenter);

    connect(_checkAnginBtn,SIGNAL(clicked()),SLOT(clickCheckAgain()));
    connect(_upgradeBtn,SIGNAL(clicked()),SLOT(downUpGrade()));

    if(isCheck) {
        setStartLog();
        connect(UpLogic,SIGNAL(upCheckFail()),SLOT(errorUp()));
        connect(UpLogic,SIGNAL(upNeedUp(QByteArray)),SLOT(finishNeedUp(QByteArray)));
        connect(UpLogic,SIGNAL(upNoUp()),SLOT(finishNoUp()));
        connect(_stratCheckTimer,SIGNAL(timeout()),SLOT(checkVersionStart()));
        _stratCheckTimer->setInterval(1000);
        _stratCheckTimer->setSingleShot(true);
        _stratCheckTimer->start();
    }

    _scaleRatio = Global->_screenRatio;
    updateUI(_scaleRatio);
}

void UpdateWidget::checkVersionStart()
{
    _stratCheckTimer->stop();
    setStartLog();
    UpLogic->startCheckVersion(false);
}


void UpdateWidget::checkVersionFinish(bool isUp)
{
    if(isUp) {
        QJsonObject json_object = QJsonDocument::fromJson(_upinfoByte).object();
        _lastVersion        = json_object.value("version").toString();
        QString checkUpWeb  = json_object.value("upgradeUrl").toString();
        if(!checkUpWeb.isEmpty())
            Global->setMainUpGradeWeb(checkUpWeb);

        QJsonObject jobj    = json_object.value(Global->getLanguage()).toObject();
        _versionUpLog       = jobj.value("msg").toString().trimmed();
        _UpgradeWeb         = jobj.value("url").toString();
    }

    QString versionTitleinfo,versionUpLog,versionUpInfo;
    if(isUp) {
        versionTitleinfo    = QString(Lge->getLangageValue("UpDate/versionUp"));
        versionUpInfo       = QString(Lge->getLangageValue("UpDate/versionInfo")).arg(_lastVersion).arg(_curVersion);
        versionUpLog        = _versionUpLog;
        _updataLogLab->setVisible(true);
    } else {
        versionTitleinfo = QString(Lge->getLangageValue("UpDate/versionNUp"));
        versionUpInfo       = "";
        _updataLogLab->setVisible(false);
        versionUpLog = Lge->getLangageValue("UpDate/versionMsg");
    }
    _versionMsg->setText(versionUpLog);
    _versionUpTitle->setText(versionTitleinfo);
    _versionUpInfo->setText(versionUpInfo);

    _upgradeBtn->setVisible(isUp);
    _checkAnginBtn->setVisible(false);
}

void UpdateWidget::setStartLog()
{
    _versionUpTitle->setText(Lge->getLangageValue("UpDate/checking"));
    _versionMsg->setText("");
    _checkAnginBtn->setVisible(false);
    _upgradeBtn->setVisible(false);
    _updataLogLab->setVisible(false);

    _lastVersion    = "";
    _versionUpLog   = "";
    _UpgradeWeb     = "";
}

void UpdateWidget::finishNoUp()
{
    checkVersionFinish(false);
}

void UpdateWidget::finishNeedUp(const QByteArray &byteData)
{
    _upinfoByte = byteData;
    checkVersionFinish(true);
}

void UpdateWidget::errorUp()
{
    _versionUpTitle->setText(Lge->getLangageValue("UpDate/checkFail"));
    _checkAnginBtn->setVisible(true);
}

void UpdateWidget::clickCheckAgain()
{
    setStartLog();
    _stratCheckTimer->start();
}

void UpdateWidget::downUpGrade()
{
    QDesktopServices::openUrl(QUrl(_UpgradeWeb, QUrl::TolerantMode));
}

void UpdateWidget::resizeEvent(QResizeEvent *)
{
    _versionUpTitle->setGeometry(16*_scaleRatio,8*_scaleRatio,this->width(),14*_scaleRatio);
    _versionUpInfo->setGeometry(16*_scaleRatio,30*_scaleRatio,this->width()-32*_scaleRatio,40*_scaleRatio);
    _updataLogLab->setGeometry(16*_scaleRatio,90*_scaleRatio,this->width()-32*_scaleRatio,20*_scaleRatio);
    _versionMsg->setGeometry(16*_scaleRatio,110*_scaleRatio,this->width()-32*_scaleRatio,this->height()-200*_scaleRatio);
    _checkAnginBtn->move(this->width()-200*_scaleRatio,this->height()-51*_scaleRatio);
    _upgradeBtn->move(this->width()-200*_scaleRatio,this->height()-51*_scaleRatio);
    _handline->setGeometry(0,30*_scaleRatio,this->width(),1*_scaleRatio);
    _bottonLine->setGeometry(0,this->height()-80*_scaleRatio,this->width(),1*_scaleRatio);
}
