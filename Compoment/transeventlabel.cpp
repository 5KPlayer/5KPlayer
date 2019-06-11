#include "transeventlabel.h"
#include <QMouseEvent>
TransEventLabel::TransEventLabel(QWidget *parent)
    : QLabel(parent)
{
    this->setMouseTracking(true);
}

void TransEventLabel::mousePressEvent(QMouseEvent *e)
{
    QLabel::mousePressEvent(e);
    e->ignore();
}

void TransEventLabel::mouseMoveEvent(QMouseEvent *e)
{
    QLabel::mouseMoveEvent(e);
    e->ignore();
}

void TransEventLabel::mouseReleaseEvent(QMouseEvent *e)
{
    QLabel::mouseReleaseEvent(e);
    e->ignore();
}
