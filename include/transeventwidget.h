#ifndef TRANSEVENTWIDGET_H
#define TRANSEVENTWIDGET_H

#include <QWidget>

class TransEventWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TransEventWidget(QWidget *parent = 0);
    virtual ~TransEventWidget() {}

    void setDrawLineToBottom(bool flag);
protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    virtual void moveEvent(QMoveEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

    virtual void paintEvent(QPaintEvent *);
private:
    bool _drawLineB;
};

#endif // TRANSEVENTWIDGET_H
