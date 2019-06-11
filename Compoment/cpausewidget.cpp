#include "cpausewidget.h"
#include <QPainter>
#include <QTimer>
#include "svgbutton.h"
#include "dymediaplayer.h"

CPauseWidget::CPauseWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    _angle = 0;

    _timer = new QTimer(this);
    _timer->setInterval(50);

    _playBtn = new SvgButton(this);
    _playBtn->initSize(79, 88);
    _playBtn->setImagePrefixName("pause_state");

    _pixmap.load(":\\res\\png\\pausing.png");
    _pixmap2.load(":\\res\\png\\pausingback.png");

    connect(_timer, SIGNAL(timeout()), SLOT(timerOut()));
    connect(_playBtn, SIGNAL(clicked()), MediaPlayer, SLOT(togglePause()));

    updateUI(1.0);
}

void CPauseWidget::updateUI(const qreal &scaleRatio)
{
    _playBtn->updateUI(scaleRatio);
    _pixmap = _pixmap.scaled(220*scaleRatio, 220*scaleRatio);
    _pixmap2 = _pixmap2.scaled(220*scaleRatio, 220*scaleRatio);
}

void CPauseWidget::timerOut()
{
    _angle += 8;
    if(_angle >= 360) _angle -= 360;
    this->update();
}

void CPauseWidget::showEvent(QShowEvent *)
{
    _angle = 0;
    _timer->start();
}

void CPauseWidget::hideEvent(QHideEvent *)
{
    _timer->stop();
}

void CPauseWidget::resizeEvent(QResizeEvent *)
{
    _playBtn->move((this->width() - _playBtn->width())/2,
                   (this->height() - _playBtn->height())/2);
}

void CPauseWidget::paintEvent(QPaintEvent *)
{
    const int x = _pixmap.width() / 2;
    const int y = _pixmap.height() / 2;

    const int centX = this->width()/2 - x;
    const int centY = this->height()/2 - y;

    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.fillRect(this->rect(), QColor(12, 12, 12, 200));
    p.drawPixmap(centX, centY, _pixmap2);
    p.save();
    p.translate(centX + x, centY + y);
    p.rotate(_angle);
    p.drawPixmap(-x, -y, _pixmap);
    p.restore();
}
