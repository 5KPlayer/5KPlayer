#ifndef SPINBOX_H
#define SPINBOX_H

#include <QWidget>
#include <QSpinBox>

class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit SpinBox(QWidget *parent = 0);

protected:
    void wheelEvent(QWheelEvent* e);
    void mousePressEvent(QMouseEvent *event);
};

#endif // SPINBOX_H
