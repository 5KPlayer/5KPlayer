#ifndef CSLIDER_H
#define CSLIDER_H

#include <QSlider>
#include "updateinterface.h"

class QTimer;

class CSlider : public QSlider, public UpdateInterface
{
    Q_OBJECT
public:
    explicit CSlider(Qt::Orientation orientation, QWidget *p = Q_NULLPTR);
    void initSize(const int &w, const int &h);

    void showRectHandle();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void updateState(bool b);

    void openMouseTrack();

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void wheelEvent(QWheelEvent *);

Q_SIGNALS:
    void pressedValue(int);
    void curValue(int);     //显示缩略图
    void leave();         //隐藏缩略图

private:
    bool b_isCircle;
    int  i_value;

    QTimer *p_timer;
};

#endif // CSLIDER_H
