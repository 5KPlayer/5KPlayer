#ifndef VIEWPORTWIDGET_H
#define VIEWPORTWIDGET_H

#include <QWidget>
#include <QSlider>
#include "updateinterface.h"

class ViewportWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit ViewportWidget(QWidget *parent = 0);
    ~ViewportWidget();

//    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void setLeftMaskWidth(int width);
    void setRightMaskWidth(int width);
    void setTimeDuration(int64_t duration);
    void updateUI(const qreal &scaleRatio);
    void updateLanguage();

signals:

public slots:
    void drawCurrentPos(QPoint pos);
    void drawLeftMask(QPoint pos);
    void drawRightMask(QPoint pos);

private:
    //左边遮罩宽度
    int _leftMaskWidth = 0;
    //右边遮罩宽度
    int _rightMaskWidth = 0;
    //图片的总时长    ms
    int64_t _timeDuration = 0;

    QPoint _currentPos = QPoint(-1,-1);

    //绘制左边遮罩
    void drawLeftMask(int width);
    //绘制右边遮罩
    void drawRightMask(int width);
    double _scaleRatio;
//    QSlider *_currentSlider;
};

#endif // VIEWPORTWIDGET_H
