#include "dlnadevinfowidget.h"
#include "globalarg.h"
#include "dlnamanager.h"
#include <QLabel>
dlnaDevInfoWidget::dlnaDevInfoWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    this->setMouseTracking(true);
    _ico        = new TransEventIco(this);
    _devName    = new QLabel(this);
    _devSupInfo = new QLabel(this);
    _line1      = new QLabel(this);

    _devName->setStyleSheet("color: rgb(115, 117, 128)");
    _devName->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _devSupInfo->setStyleSheet("color: rgb(115, 117, 128)");
    _devSupInfo->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _line1->setStyleSheet("background-color: rgb(31, 31, 35);");
    _scaleRatio = 1.0;
}

void dlnaDevInfoWidget::updateLanguage()
{
    upInfo();
}

void dlnaDevInfoWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void dlnaDevInfoWidget::setDevInfo(deviceInfo dev)
{
    _devInfo.deviceAudioFormats = dev.deviceAudioFormats;
    _devInfo.deviceVideoFormats = dev.deviceVideoFormats;
    _devInfo.deviceType         = dev.deviceType;
    strcpy(_devInfo.friendlyName,dev.friendlyName);
    strcpy(_devInfo.baseHttp,dev.baseHttp);
    strcpy(_devInfo.udn,dev.udn);
    if(dev.deviceType == 1) {
        _ico->load(QString(":/res/svg/video_equipment.svg"));
    } else {
        _ico->load(QString(":/res/svg/audio_equipment.svg"));
    }
    upInfo();
}
#include <QDebug>
void dlnaDevInfoWidget::upUI()
{
    _ico->setGeometry(5*_scaleRatio,2*_scaleRatio,40*_scaleRatio,40*_scaleRatio);
    _devName->setGeometry(56*_scaleRatio,2*_scaleRatio,this->width()-60*_scaleRatio,20*_scaleRatio);
    _devSupInfo->setGeometry(56*_scaleRatio,24*_scaleRatio,_devName->width(),_devName->height());
    _line1->setGeometry(5*_scaleRatio,46*_scaleRatio,this->width()-5*_scaleRatio,1*_scaleRatio);
    upInfo();
}

void dlnaDevInfoWidget::upInfo()
{
    QFont font;
    font.setPixelSize(13*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _devName->setFont(font);
    _devSupInfo->setFont(font);
    QString devName = _devInfo.friendlyName;
    _devName->setText(DLNA->getVirTextInfo(_devName,devName));
    QString supInfo = Lge->getLangageValue("DLNA/supfmt");
    if(_devInfo.deviceType == 1) {
        QString video;
        foreach (QString val, _devInfo.deviceVideoFormats) {
            video.append(val).append(",");
        }
        supInfo.append(video);
    }
    QString audio;
    int apos = 0;
    foreach (QString val, _devInfo.deviceAudioFormats) {
        apos +=1;
        audio.append(val);
        if(apos < _devInfo.deviceAudioFormats.count())
            audio.append(",");
    }
    supInfo.append(audio);
    if(supInfo == Lang("DLNA/supfmt")) {
        _devSupInfo->setText(supInfo.append(Lang("DLNA/unkown")));
    } else {
        _devSupInfo->setText(DLNA->getVirTextInfo(_devSupInfo,supInfo));
    }
}

void dlnaDevInfoWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
