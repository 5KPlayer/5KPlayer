#ifndef PRESVGBUTTON_H
#define PRESVGBUTTON_H

#include <QSvgWidget>
#include "updateinterface.h"

class PreSvgButton : public QSvgWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit PreSvgButton(QWidget *parent = 0);
    ~PreSvgButton();

    void setImagePrefixName(const QString &prefixName);
    void initSize(int w,int h);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private:
    void init();

    void setPressImage(const QString &img)   {_pressImage = img;}    //按下
    void setEnterImage(const QString &img)   {_enterImage = img;}    //进入
    void setNormalImage(const QString &img)  {_normalImage = img;}   //常态
    void setDisableImage(const QString &img) {_disableImage = img;}  //禁用

    void setCurShowedImage(const QString &img);//设置当前显示的图片

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
    void btnMove(int xPos);
    void btnRelease();
private:
    QPoint  _mousePoint;
    bool    _leave; //标记是否离开

    QString _pressImage;  //按下时的图片
    QString _enterImage;  //进入时的图片
    QString _normalImage; //正常时的图片
    QString _disableImage;//禁用时的图片
    QString _curShowImg;

    qreal _ratio;     //缩放比例

    bool _isIgnoreLeave;
    bool _isPressed;  //记录是否按下 - 正常状态
    bool _isState;
};

#endif // PRESVGBUTTON_H
