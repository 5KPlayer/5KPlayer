#include "viewportwidget.h"
#include <QPainter>
#include <QDebug>
#include "globalarg.h"

ViewportWidget::ViewportWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    _scaleRatio = Global->_screenRatio;
//    _currentSlider = new QSlider(Qt::Horizontal,this);
}

ViewportWidget::~ViewportWidget()
{

}

//void ViewportWidget::resizeEvent(QResizeEvent *event)
//{
////    _currentSlider->resize(this->width(),10);
////    _currentSlider->move(0,(this->height() - 10) / 2);
//}

void ViewportWidget::paintEvent(QPaintEvent *event)
{
//    QPainter painter(this);
//    painter.setPen(Qt::red);
//    painter.drawLine(_currentPos.x(), 0,_currentPos.x(),this->height());
    drawLeftMask(_leftMaskWidth);
    drawRightMask(_rightMaskWidth);
}

void ViewportWidget::setLeftMaskWidth(int width)
{
    if(_leftMaskWidth == width)
        return;
    _leftMaskWidth = width;
    update();

}

void ViewportWidget::setRightMaskWidth(int width)
{

    if(_rightMaskWidth == width)
        return;
    _rightMaskWidth = width;
    update();
}

void ViewportWidget::setTimeDuration(int64_t duration)
{
    _timeDuration = duration;
}

void ViewportWidget::updateUI(const qreal &scaleRatio)
{
    if(_scaleRatio == scaleRatio)
        return;
    _scaleRatio = scaleRatio;
    update();
}

void ViewportWidget::updateLanguage()
{

}

void ViewportWidget::drawCurrentPos(QPoint pos)
{
        _currentPos = mapFromGlobal(pos);
        update();
}

void ViewportWidget::drawLeftMask(QPoint pos)
{
    QPoint p = mapFromGlobal(pos);
    setLeftMaskWidth(p.x());
}

void ViewportWidget::drawRightMask(QPoint pos)
{
    QPoint p = mapFromGlobal(pos);
    setRightMaskWidth(this->width() - p.x());
}

void ViewportWidget::drawLeftMask(int width)
{
    if(width == 0)
        return;
    QPainter painter(this);
    QBrush brush(QColor(153,153,153,125));
    QRect rect(0,0,width,this->height());
    painter.fillRect(rect,brush);

    QRect lineRect(rect.right(),0,3 * _scaleRatio,this->height());
    painter.setPen(Qt::black);

    painter.fillRect(lineRect,QBrush(QColor(Qt::white)));
    painter.drawRect(lineRect);
    /*
    painter.setPen(Qt::gray);
    painter.drawLine(rect.right(),0,rect.right(),this->height());*/
}

void ViewportWidget::drawRightMask(int width)
{
    if(width == 0)
        return;
    QPainter painter(this);
    QBrush brush(QColor(153,153,153,125));
    QRect rect(this->width() - width,0,width,this->height());
    painter.fillRect(rect,brush);

    QRect lineRect(rect.left() - 3 * _scaleRatio,0,3 * _scaleRatio,this->height());
    painter.setPen(Qt::black);
    painter.fillRect(lineRect,QBrush(QColor(Qt::white)));
    painter.drawRect(lineRect);
}
