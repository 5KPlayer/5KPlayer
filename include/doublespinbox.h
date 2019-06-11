#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QWidget>
#include <QDoubleSpinBox>

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleSpinBox(QWidget *parent = 0);

protected:
    void wheelEvent(QWheelEvent* e);
    void mousePressEvent(QMouseEvent *event);
};

#endif // DOUBLESPINBOX_H
