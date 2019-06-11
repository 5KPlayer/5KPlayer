#include "urlanalyzeexit.h"
#include "cdialog.h"
#include "globalarg.h"
UrlAnalyzeExit::UrlAnalyzeExit(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    Init();
    this->resize(355,155);
}

void UrlAnalyzeExit::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void UrlAnalyzeExit::continueClick()
{
    CDialog* dialog = (CDialog*)this->parentWidget();
    dialog->done(1);
}

void UrlAnalyzeExit::cancelClick()
{
    CDialog* dialog = (CDialog*)this->parentWidget();
    dialog->done(2);
}

void UrlAnalyzeExit::Init()
{
    _scaleRatio= 1.0;
   _info = new QLabel(this);
   _deal = new QLabel(this);
   _cancel      = new DYLabel(this,true);
   _continue    = new DYLabel(this,true);
   _info->setWordWrap(true);
   _deal->setWordWrap(true);

   _info->setStyleSheet("color:rgb(182,183,184)");
   _deal->setStyleSheet("color:rgb(115,117,128)");

   _info->setAlignment(Qt::AlignTop);
   _deal->setAlignment(Qt::AlignTop);
   _info->setText(Lge->getLangageValue("UrlExit/info"));
   _deal->setText(Lge->getLangageValue("UrlExit/deal"));

   _cancel->setText(Lge->getLangageValue("Main/cancel"));
   _continue->setText(Lge->getLangageValue("UrlExit/continue"));

   _cancel->setAlignment(Qt::AlignCenter);
   _continue->setAlignment(Qt::AlignCenter);

   connect(_cancel,SIGNAL(clicked()),SLOT(cancelClick()));
   connect(_continue,SIGNAL(clicked()),SLOT(continueClick()));

   upUI();
}

void UrlAnalyzeExit::upUI()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _deal->setFont(font);
    font.setBold(true);
    _info->setFont(font);
    this->resize(355*_scaleRatio,155*_scaleRatio);
    _info->setGeometry(15*_scaleRatio,18*_scaleRatio,320*_scaleRatio,50*_scaleRatio);
    _deal->setGeometry(15*_scaleRatio,38*_scaleRatio,320*_scaleRatio,50*_scaleRatio);
    _cancel->setGeometry(260*_scaleRatio,108*_scaleRatio,80*_scaleRatio,20*_scaleRatio);
    _continue->setGeometry(150*_scaleRatio,108*_scaleRatio,95*_scaleRatio,20*_scaleRatio);
}
