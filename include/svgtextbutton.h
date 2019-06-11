#ifndef SVGTEXTBUTTON_H
#define SVGTEXTBUTTON_H

/**********************
 * 主要用在设置界面的按钮
 **********************/


#include "updateinterface.h"
class QLabel;
class QSvgWidget;

class SvgTextButton : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SvgTextButton(QWidget *parent = 0);
    void setInfo(const QString &svgFile, const QString &text);
    void initSize(const int &w, const int &h, const int &sw, const int &sh);

    void setPressed();//设置按下
    void setRaised(); //弹起

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

protected:
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private:
    void showPressedState(); //显示按下状态
    void showNormalState();  //显示正常状态

signals:
    void clicked();

private:
    QSvgWidget *_svgImage;
    QLabel *_textLabel;

    bool b_pressed;

    QString _pressImage;  //按下时的图片
    QString _normalImage; //正常时的图片

    int _svgWidth;
    int _svgHeight;
};

#endif // SVGTEXTBUTTON_H
