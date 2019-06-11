#include "liveico.h"
#include "globalarg.h"
LiveIco::LiveIco(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    Init();
}

void LiveIco::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _liveImage->setGeometry(0,0,this->width(),this->height());
    _liveType->setGeometry(0,0,20*_scaleRatio,20*_scaleRatio);
    _favirtesbtn->setGeometry(_liveImage->width()-20*_scaleRatio,_liveImage->height()-20*_scaleRatio,20*_scaleRatio,20*_scaleRatio);
}

void LiveIco::setLiveData(const QString &liveUrl, const QString &liveImage, const bool &favires)
{
    _liveUrl = liveUrl;
    _bFavires= favires;
    _liveImage->setPixmap(liveImage);
    QString path;
    if (_bFavires)
        path = ":/res/svg/love_on.svg";
    else
        path = ":/res/svg/love_nor.svg";
    _favirtesbtn->load(path);
    QString typepath(":/res/svg/music-light.svg");
    _liveType->load(typepath);
}

QString LiveIco::getLiveData() const
{
    return _liveUrl;
}

void LiveIco::favirtesClick()
{
    _bFavires = !_bFavires;
    QString path;
    if (_bFavires)
        path = ":/res/svg/love_on.svg";
    else
        path = ":/res/svg/love_nor.svg";
    _favirtesbtn->load(path);
    emit updateLiveWidget(_liveUrl,_bFavires);
}

void LiveIco::Init()
{
    _liveImage      = new TransEventLabel(this);
    _liveType       = new TransEventIco(_liveImage);
    _favirtesbtn    = new TransEventIcoClk(_liveImage);

    _liveImage->setScaledContents(true);

    connect(_favirtesbtn,SIGNAL(clicked()),SLOT(favirtesClick()));
    updateUI(1.0);
}

void LiveIco::resizeEvent(QResizeEvent *)
{
    updateUI(Global->_screenRatio);
}
