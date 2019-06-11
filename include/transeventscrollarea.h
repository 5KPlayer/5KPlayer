#ifndef TRANSEVENTSCROLLAREA_H
#define TRANSEVENTSCROLLAREA_H

#include <QScrollArea>

class TransEventScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit TransEventScrollArea(QWidget *parent = 0);
    virtual ~TransEventScrollArea() {}

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    virtual void moveEvent(QMoveEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
};

#endif // TRANSEVENTSCROLLAREA_H
