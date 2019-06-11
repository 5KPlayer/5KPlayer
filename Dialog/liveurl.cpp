#include "liveurl.h"
#include "svgbutton.h"
#include "globalarg.h"
LiveUrl::LiveUrl(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    this->setMouseTracking(false);
    Init();
}

void LiveUrl::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _urlDelBtn->updateUI(scaleRatio);
    upUI();
}

void LiveUrl::setUrlPath(const QString &urlPath)
{
    _urlPath = urlPath;
    _urlPathLab->setText(_urlPath);
}

QString LiveUrl::getUrlPath()
{
    return _urlPath;
}

void LiveUrl::clickDelBtn()
{
    emit delLiveUrl(_urlPath);
}

void LiveUrl::clickPlayBtn()
{
    emit playLiveUrl(_urlPath);
}

void LiveUrl::Init()
{
    _urlDelBtn  = new SvgButton(this);
    _urlPlayBtn = new SvgButton(this);
    _urlPathLab = new QLabel(this);


    _urlPathLab->setStyleSheet("color:rgb(255,255,255)");
    _urlDelBtn->initSize(12,12);
    _urlDelBtn->setImagePrefixName("strike_out");
    _urlPlayBtn->setImagePrefixName("bofang");

    _urlDelBtn->setToolTip(Lge->getLangageValue("Main/delete"));
    _urlPlayBtn->setToolTip(Lge->getLangageValue("Main/play"));
    _urlDelBtn->hide();

    connect(_urlDelBtn,SIGNAL(clicked()),SLOT(clickDelBtn()));
    connect(_urlPlayBtn,SIGNAL(clicked()),SLOT(clickPlayBtn()));
    _scaleRatio = 1.0;
    upUI();
}

void LiveUrl::upUI()
{
    QFont font;
    font.setPixelSize(14*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _urlPathLab->setFont(font);
    double _space = 3*_scaleRatio;
    _urlDelBtn->setGeometry(_space,_space,12*_scaleRatio,12*_scaleRatio);
    _urlPathLab->setGeometry(18*_scaleRatio,0,this->width()-54*_scaleRatio,18*_scaleRatio);
    _urlPlayBtn->setGeometry(this->width()-36*_scaleRatio,0,18*_scaleRatio,18*_scaleRatio);
}

void LiveUrl::resizeEvent(QResizeEvent *)
{
    upUI();
}

void LiveUrl::enterEvent(QEvent *)
{
    _urlDelBtn->show();
}

void LiveUrl::leaveEvent(QEvent *)
{
    _urlDelBtn->hide();
}
