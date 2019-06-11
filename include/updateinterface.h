#ifndef UPDATEINTERFACE_H
#define UPDATEINTERFACE_H

#include <QWidget>
#include "language.h"
class UpdateInterface
{
public:
    //改变语言
    virtual void updateLanguage() = 0;

    //更新界面（大小 间距 行高 字体）
    virtual void updateUI(const qreal &scaleRatio) = 0;

protected:
    UpdateInterface(){}
    virtual ~UpdateInterface(){}

protected:
    int _originalWidth;
    int _originalHeight;
};

class UpdateInterfaceWidget: public QWidget, public UpdateInterface
{
    Q_OBJECT
protected:
    UpdateInterfaceWidget(QWidget *p = Q_NULLPTR) : QWidget(p){}
    virtual ~UpdateInterfaceWidget(){}
};


#endif // UPDATEINTERFACE_H
