#include "doublespinbox.h"

#include <QWheelEvent>


DoubleSpinBox::DoubleSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
{

}

void DoubleSpinBox::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

void DoubleSpinBox::mousePressEvent(QMouseEvent *event)
{
    QDoubleSpinBox::mousePressEvent(event);
    event->accept();
}
