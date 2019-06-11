#ifndef TRANSEVENLISTWIDGET_H
#define TRANSEVENLISTWIDGET_H

#include <QListWidget>
class TransEvenListWidget : public QListWidget
{
    Q_OBJECT
public:
    TransEvenListWidget(QWidget *parent = 0);
    virtual ~TransEvenListWidget() {}
    bool isctrldown() const {return _ctrl_is_down;}
    bool isShiftdown() const {return _shift_is_down;}
    void setminselect(const int &selectcount);
    int  getminselect() const {return _minselect;}
    void chearminselect(){_minselect = -1;}
    void upDateStyle(qreal ratio);

signals:
    void widgetselall();

private:
    bool _ctrl_is_down;
    bool _shift_is_down;
    int _minselect;
    bool _mouse_ignore;

public slots:
    void setmouse_ignore(const bool &ignore) {_mouse_ignore = ignore;}

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void enterEvent(QEvent *);

    virtual void wheelEvent(QWheelEvent *);
};

#endif // TRANSEVENLISTWIDGET_H
