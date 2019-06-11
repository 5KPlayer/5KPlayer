#include "urlanalyzefailed.h"
#include "globalarg.h"
UrlAnalyzeFailed::UrlAnalyzeFailed(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    _scaleRatio = 1.0;
    this->resize(335*_scaleRatio,150*_scaleRatio);
    _okBtn = new DYLabel(this,true);
    _okBtn->setText(Lge->getLangageValue("Main/ok"));
    _okBtn->setAlignment(Qt::AlignCenter);

    _detailed  = new QLabel(this);
    _detailed->setText(Lge->getLangageValue("UrlAnalyFail/detailed"));
    _detailed->setStyleSheet("Color:rgb(182,183,184);");

    _info = new QLabel(this);
    _info->setText(Lge->getLangageValue("UrlAnalyFail/info"));
    _info->setStyleSheet("Color:rgb(115,117,128);");
    _info->setWordWrap(true);
    connect(_okBtn,SIGNAL(clicked()), this->parentWidget(), SLOT(close()));

    UpUI();
}

void UrlAnalyzeFailed::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    UpUI();
}

void UrlAnalyzeFailed::UpUI()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    this->resize(335*_scaleRatio,150*_scaleRatio);
    _okBtn->setGeometry(235*_scaleRatio,95*_scaleRatio,100*_scaleRatio,20*_scaleRatio);
    _detailed->setGeometry(20*_scaleRatio,20*_scaleRatio,335*_scaleRatio,20*_scaleRatio);
    _info->setGeometry(20*_scaleRatio,_detailed->height()+_detailed->y(),335*_scaleRatio,40*_scaleRatio);
    _okBtn->setFont(font);
    _info->setFont(font);
    font.setBold(true);
    _detailed->setFont(font);
}
