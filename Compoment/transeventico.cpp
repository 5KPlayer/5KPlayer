#include "transeventico.h"
#include <QMouseEvent>

TransEventIco::TransEventIco(QWidget *parent)
    : QSvgWidget(parent)
{
    this->setMouseTracking(true);
}

void TransEventIco::mousePressEvent(QMouseEvent *e)
{
    QSvgWidget::mousePressEvent(e);
    e->ignore();
}

void TransEventIco::mouseMoveEvent(QMouseEvent *e)
{
    QSvgWidget::mouseMoveEvent(e);
    e->ignore();
}

void TransEventIco::mouseReleaseEvent(QMouseEvent *e)
{
    QSvgWidget::mouseReleaseEvent(e);
    e->ignore();
}
