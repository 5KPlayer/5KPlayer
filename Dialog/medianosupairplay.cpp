#include "medianosupairplay.h"
#include <QFileInfo>
#include <QLabel>
#include <QUrl>
#include "globalarg.h"
#include <QDesktopServices>
#include "dylabel.h"
MediaNoSupAirPlay::MediaNoSupAirPlay(QWidget *parent, QString filePath) : UpdateInterfaceWidget(parent)
{
    Init(filePath);
}

void MediaNoSupAirPlay::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    UpUI();
}

void MediaNoSupAirPlay::Init(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    _scaleRatio = 1.0;
    _info       = new QLabel(this);
    _deal       = new QLabel(this);
    _later      = new DYLabel(this,true);
    _converter  = new DYLabel(this,true);

    _info->setWordWrap(true);
    _deal->setWordWrap(true);

    _info->setStyleSheet("color:rgb(182,183,184)");
    _deal->setStyleSheet("color:rgb(115,117,128)");

    _info->setAlignment(Qt::AlignTop);
    _deal->setAlignment(Qt::AlignTop);
    _info->setText(Lge->getLangageValue("AirNoSup/info"));
    _deal->setText(QString(Lge->getLangageValue("AirNoSup/mesg")).arg(fileInfo.suffix()));

    _later->setText(Lge->getLangageValue("AirNoSup/later"));
    _converter->setText(Lge->getLangageValue("AirNoSup/conter"));
    _later->setAlignment(Qt::AlignCenter);
    _converter->setAlignment(Qt::AlignCenter);
    connect(_later,SIGNAL(clicked()),this->parentWidget(),SLOT(close()));
    connect(_converter,SIGNAL(clicked()),SLOT(converterClick()));

    UpUI();
}

void MediaNoSupAirPlay::UpUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    _deal->setFont(font);
    _later->setFont(font);
    _converter->setFont(font);
    font.setBold(true);
    _info->setFont(font);
    this->resize(400*_scaleRatio,175*_scaleRatio);
    _info->setGeometry(15*_scaleRatio,18*_scaleRatio,375*_scaleRatio,25*_scaleRatio);
    _deal->setGeometry(15*_scaleRatio,43*_scaleRatio,375*_scaleRatio,50*_scaleRatio);
    _later->setGeometry(150*_scaleRatio,108*_scaleRatio,80*_scaleRatio,20*_scaleRatio);
    _converter->setGeometry(250*_scaleRatio,108*_scaleRatio,135*_scaleRatio,20*_scaleRatio);
}

void MediaNoSupAirPlay::converterClick()
{
    QString url = "https://www.5kplayer.com/software/convert-dvd-video.htm?airplay-supported-convert";
    if(Global->getLanguage() == "jp") {
        url = "https://www.5kplayer.com/software/music-video-transfer-jp.htm?airplay-get-converter-jp";
    }
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}
