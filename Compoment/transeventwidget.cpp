#include "transeventwidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QMoveEvent>

TransEventWidget::TransEventWidget(QWidget *parent) : QWidget(parent)
{
    this->setMouseTracking(true);
    _drawLineB = false;
}

void TransEventWidget::setDrawLineToBottom(bool flag)
{
    _drawLineB = flag;
}

void TransEventWidget::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    e->ignore();
}

void TransEventWidget::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
    e->ignore();
}

void TransEventWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
    e->ignore();
}

void TransEventWidget::moveEvent(QMoveEvent *e)
{
    QWidget::moveEvent(e);
    e->ignore();
}

void TransEventWidget::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);
    e->ignore();
}

void TransEventWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
    e->ignore();
}

void TransEventWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);


    if(_drawLineB) {
        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(106,107,108));
        p.setPen(pen);
        p.drawLine(QPoint(0,this->height()-1),QPoint(this->width(),this->height()-1));
    }

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}
