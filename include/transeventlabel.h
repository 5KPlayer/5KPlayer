#ifndef TRANSEVENTLABEL_H
#define TRANSEVENTLABEL_H

#include <QLabel>

class TransEventLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TransEventLabel(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
};

#endif // TRANSEVENTLABEL_H
