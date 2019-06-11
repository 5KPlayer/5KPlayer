#include "virtuallinewidget.h"
#include <QPainter>

VirtualLineWidget::VirtualLineWidget(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
#if 0
    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    this->setPalette(pal);
#else
    this->setAttribute(Qt::WA_TranslucentBackground);
#endif

    _lineWidth = 4;
    _lineColor = Qt::black;
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    this->setModal(false);
}

void VirtualLineWidget::setLineWidth(const int &w)
{
    if(_lineWidth != w)
        _lineWidth = w;
}

void VirtualLineWidget::setLineColor(const QColor &color)
{
    _lineColor = color;
}

void VirtualLineWidget::paintEvent(QPaintEvent *)
{
    /*
    QPen pen(_lineColor);
    pen.setCapStyle(Qt::SquareCap);
    pen.setWidth(_lineWidth);
    pen.setStyle(Qt::DotLine); //画虚线

    QPainter p(this);
    p.setPen(pen);
    p.drawRect(this->rect());
    */
    QPainter p(this);
    p.fillRect(this->rect(),QColor(255,255,255,80));
}
