#ifndef CDIALOG2_H
#define CDIALOG2_H

#include <QDialog>
#include "updateinterface.h"

class QLabel;
class SvgButton;
class QPropertyAnimation;

class CDialog2 : public QDialog, public UpdateInterface
{
    Q_OBJECT
public:
    explicit CDialog2(const QString &str="", QWidget *p = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~CDialog2(){}

    void setContentWidget(QWidget *); //设置中心界面

    void updateUI(const qreal &scaleRatio);
    void updateLanguage(){}

    void initGeometry(const int &x, const int &y, const int &w, const int &h);

public slots:
    virtual void accept();
    virtual void reject();
    void reject2();

private slots:
    void startCloseAnimal();
    void closeFinished();

protected:
    virtual bool eventFilter(QObject *, QEvent *);

private:
    QLabel  *_titleLabel;//标题
    SvgButton *_closeBtn;//关闭按钮

    QWidget *_contentW;  //
    QPropertyAnimation *_showAnimal; //显示动画
    QPropertyAnimation *_closeAnimal;//关闭动画

    int _space;
    int _state;//状态  accept or reject

    bool b_pressed;     //是否按下

    qreal _ratio;
};

#endif // CDIALOG2_H
