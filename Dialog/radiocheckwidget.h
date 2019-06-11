#ifndef RADIOCHECKWIDGET_H
#define RADIOCHECKWIDGET_H

#include "updateinterface.h"

class QParallelAnimationGroup;
class QPropertyAnimation;
class QLabel;

class RadioCheckWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit RadioCheckWidget(QWidget *parent = 0);
    ~RadioCheckWidget();
    void recount();//重新计算位置

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    virtual void paintEvent(QPaintEvent *);

Q_SIGNALS:
    void curClickedInfo(const QString &info);

private:
    void release(const int &curX);
    void moveAll(const int &page, const int &t);

    int _lrSpace;//左右间隔
    int _tbSpace;//上下间隔
    int _labelWidth;

    bool _leave;
    int _curPage;
    int _totalPage;
    int _pressedX, _tmpX;
    QList<QLabel *> _labelList;

    QStringList _radioHttpList;
    QList<QPropertyAnimation *> _animalList;
    QParallelAnimationGroup *_animalGroup;
};

#endif // RADIOCHECKWIDGET_H
