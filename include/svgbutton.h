#ifndef SVGBUTTON_H
#define SVGBUTTON_H

#include <QSvgWidget>
#include "updateinterface.h"

class QTimer;
class QPropertyAnimation;
/*!
 * \brief The SvgButton class
 *
 * Copyright: Digiarty
 * Author: 赵彦博
 * Date: 2016-9-2
 * Description: 封装的矢量图片按钮, 可以进行动画缩放,
 *
 */
class SvgButton : public QSvgWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit SvgButton(QWidget *parent = Q_NULLPTR);
    explicit SvgButton(const qreal &ratio, QWidget *parent = Q_NULLPTR);
    ~SvgButton();

    void openAnimal(bool b);//用于openMode

    void setImagePrefixName(const QString &prefixName);
    void setSvgText(QString text);
    void setSvgTextColor(QColor textON,QColor textNor);
    void setSvgTextCenter(bool isCenter);
    void setSvgTextFlag(int flag);
    void setSvgTextXpos(int x);
    void setSvgTextFontSize(QFont font,int fontsize);

    void showAnimal(); //用于截图时显示动画

    void setshowDis(); //显示灰色[禁用]

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    //初始化大小
    void initSize(const int &w, const int &h);
    void initSize(const QSize &size);

    //忽略离开事件
    void ignoreLeaveEvent() {_isIgnoreLeave = true;}

    QSize sizeHint() const Q_DECL_OVERRIDE;

    void changeState(const QString &file, const bool &b);//专门针对airplay按钮的变化

    void resetMove(const int &x, const int &y);

private:
    void init();
    void leave();

    void setEnterImage(const QString &img)   {_enterImage = img;}    //进入
    void setNormalImage(const QString &img)  {_normalImage = img;}   //常态

    void setCurShowedImage(const QString &img);//设置当前显示的图片

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

    virtual void showEvent(QShowEvent *);
    virtual void changeEvent(QEvent *);
    virtual void paintEvent(QPaintEvent *);

signals:
    void clicked();
    void clicked_r();

private slots:
    void timerOut();

private:
    bool _leave; //标记是否离开

    QString _enterImage;  //进入时的图片
    QString _normalImage; //正常时的图片

    bool _isAnimalOn; //是否开启动画
    qreal _ratio;     //缩放比例

    bool _isIgnoreLeave;
    bool _isPressed;  //记录是否按下 - 正常状态

    QPropertyAnimation *_animation;//属性动画类
    QString     _curShowImg;
    QString     _svgText;
    QColor      _svgTextColor_nor;
    QColor      _svgTextColor_on;
    bool        _textCenter;
    int         _textFlag;
    int         _textDrawX;
    QFont       _font;
    int         _fontSize;

    QTimer *p_timer;
    int i_rectLen;//动画框的长度
};

#endif // SVGBUTTON_H
