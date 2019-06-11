#include "spinbox.h"
#include <QWheelEvent>
SpinBox::SpinBox(QWidget *parent)
    : QSpinBox(parent)
{

}

void SpinBox::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

void SpinBox::mousePressEvent(QMouseEvent *event)
{
    QSpinBox::mousePressEvent(event);
    event->accept();
}
