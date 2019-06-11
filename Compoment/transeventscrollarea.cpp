#include "transeventscrollarea.h"
#include <QEvent>
#include <QMouseEvent>
#include <QMoveEvent>

TransEventScrollArea::TransEventScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    this->setMouseTracking(true);
}

void TransEventScrollArea::mousePressEvent(QMouseEvent *e)
{
    QScrollArea::mousePressEvent(e);
    e->ignore();
}

void TransEventScrollArea::mouseMoveEvent(QMouseEvent *e)
{
    QScrollArea::mouseMoveEvent(e);
    e->ignore();
}

void TransEventScrollArea::mouseReleaseEvent(QMouseEvent *e)
{
    QScrollArea::mouseReleaseEvent(e);
    e->ignore();
}

void TransEventScrollArea::moveEvent(QMoveEvent *e)
{
    QScrollArea::moveEvent(e);
    e->ignore();
}

void TransEventScrollArea::enterEvent(QEvent *e)
{
    QScrollArea::enterEvent(e);
    e->ignore();
}

void TransEventScrollArea::leaveEvent(QEvent *e)
{
    QScrollArea::leaveEvent(e);
    e->ignore();
}
