#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <QLabel>
#include "updateinterface.h"

class QPropertyAnimation;

/*!
 * \brief The ImageButton class
 *
 * Copyright: Digiarty
 * Author: 赵彦博
 * Date: 2016-7-13
 * Description: 封装的图片按钮, 可以进行动画缩放,
 * 支持4种显示状态: 进入 按下 正常 禁用.
 *
 */
class ImageButton : public QLabel, public UpdateInterface
{
    Q_OBJECT
public:
    explicit ImageButton(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    /*!
     * \brief 构造函数
     *
     * \param ratio  : 缩放比例
     * \param parent : 父类的指针
     * \param f : flags
     */
    explicit ImageButton(qreal ratio, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ImageButton(){}

    /*!
     * \brief setImagePrefixName: 设置图片前缀名和后缀名,然后对应四种状态图片,
     *                           (eg: add,  add_nor.png  add_dis.png  add_down.png  add_on.png)
     *
     * \param prefixName: 图片前缀名称
     * \param subfix: 图片的后缀名,默认是 .png
     * \return void
     */
    void setImagePrefixName(QString prefixName, QString subfix=".png");

    void setshowDis();
    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    //初始化大小
    void initSize(const int &w, const int &h);

private:
    void leave();

    void setPressImage(const QString &img)   {_pressImage = img;}    //按下
    void setEnterImage(const QString &img)   {_enterImage = img;}    //进入
    void setNormalImage(const QString &img)  {_normalImage = img;}   //常态
    void setDisableImage(const QString &img) {_disableImage = img;}  //禁用

    void setCurShowedImage(const QString &img);//设置当前显示的图片

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

    virtual void changeEvent(QEvent *);

signals:
    void clicked();

private:
    bool _leave; //标记是否离开

    QString _pressImage;  //按下时的图片
    QString _enterImage;  //进入时的图片
    QString _normalImage; //正常时的图片
    QString _disableImage;//禁用时的图片

    bool _isAnimalOn; //是否开启动画
    bool _isSet;
    qreal _ratio;     //缩放比例

    QPropertyAnimation *_animation;//属性动画类

};

#endif
