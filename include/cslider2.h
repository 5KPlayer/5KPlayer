#ifndef CSLIDER2_H
#define CSLIDER2_H

#include "updateinterface.h"

class CSlider2 : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit CSlider2(QWidget *parent = 0);

    //初始化大小
    void initSize(const int &, const int &);
    void initSize(const QSize &);

    //设置每步间距
    void setPageStep(const qreal &);

    //设置取值范围
    void setRange(const int &, const int &);

    //设置当前值
    void setValue(const double &);

    // 值 转为 X坐标
    int valueToXPos(const double &);

    // X坐标 转为 值
    double XPosToValue(const int &);

    // 设置显示所有短线
    void showAllLine();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private:
    //返回可用的X坐标范围
    int availableXPos(const int &);

protected:
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void valueChanged(double);

private:
    int _curXPos;   //当前X坐标
    qreal _curValue;

    //范围值
    int _rangeStart;
    int _rangeEnded;

    //
    int _minXPos;
    int _maxXPos;

    qreal _ratio;
    qreal _step;

    // 是否显示所有短线
    bool b_showAllLine;
};

#endif // CSLIDER2_H
