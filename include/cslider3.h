#ifndef CSLINDER3_H
#define CSLINDER3_H

#include "updateinterface.h"

// 建议控件最小高度为32
class CSlider3 : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit CSlider3(QWidget *parent = 0);

    //初始化大小
    void initSize(const int &, const int &);
    void initSize(const QSize &);

    //设置每步间距
    void setPageStep(const qreal &);

    //设置取值范围
    void setRange(const double &, const double &);

    //设置显示首位和0值的文字
    void setLineOrText(bool showtext);
    //设置当前值
    void setValue(const double &);
    double getValue();

    // 值 转为 X坐标
    int valueToXPos(const double &);

    // X坐标 转为 值
    double XPosToValue(const int &);


    void setSliderEnable(bool enable);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private:
    //返回可用的X坐标范围
    int availableXPos(const int &);

protected:
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void valueChanged(double);

private:
    int _curXPos;   //当前X坐标
    qreal _curValue;

    //范围值
    double _rangeStart;
    double _rangeEnded;

    //
    int _minXPos;
    int _maxXPos;

    qreal _ratio;
    qreal _step;

    // 是否显示所有短线和文字
    bool b_showAllText;
    bool b_mousePress;
    bool b_enable;

};

#endif // CSLINDER3_H
