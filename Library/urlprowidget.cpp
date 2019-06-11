#include "urlprowidget.h"

UrlproWidget::UrlproWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void UrlproWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void UrlproWidget::Init()
{
    _scaleRatio = 1.0;
    _proGIF     = new QLabel(this);

    _proMovie   = new QMovie(":/res/loading.gif");
    _proGIF->setScaledContents(true);
    _proGIF->setMovie(_proMovie);

    _proMovie->start();
    _proGIF->setAlignment(Qt::AlignCenter);

    this->setFocusPolicy(Qt::NoFocus);
    this->setStyleSheet("background-color: transparent;");
    upUI();
}

void UrlproWidget::upUI()
{
    _proMovie->setScaledSize(QSize(52*_scaleRatio,52*_scaleRatio));
    _proGIF->setGeometry(5,5,52*_scaleRatio,52*_scaleRatio);
}
