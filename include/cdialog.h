#ifndef CDIALOG_H
#define CDIALOG_H

#include <QDialog>
#include "updateinterface.h"

class QLabel;
class QScreen;
class SvgButton;
class QPropertyAnimation;

class CDialog : public QDialog, public UpdateInterface
{
    Q_OBJECT
public:
    explicit CDialog(bool showLogo = true, QWidget *p = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~CDialog(){}

    void setNeedSpace(const bool &b) {b_space = b;}
    void setNoClose(bool isHide);     //不显示关闭按钮
    void setTitle(const QString &str);//设置显示标题
    void setContentWidget(QWidget *); //设置中心界面
    void DontShowHead();              //不显示head
    void disableMove();

    void initSize(const int &w, const int &h);

    void updateUI(const qreal &scaleRatio);
    void updateLanguage(){}

    void recount(const QPoint &point);

private slots:
    void screenChanged(QObject *obj);
    void startCloseAnimal();

private:
    void changeWidgets();

protected:
    virtual bool eventFilter(QObject *, QEvent *);

private:
    QLabel  *_logoLabel;//LOGO

    QWidget *_headW;     //标题栏
    QLabel  *_titleLabel;//标题
    SvgButton *_closeBtn;//关闭按钮

    QWidget *_contentW;  //
    QPropertyAnimation *_showAnimal;//显示动画
    QPropertyAnimation *_closeAnimal;//关闭动画

    bool _pressed;     //是否按下
    QPoint _pressPoint;//按下的坐标

    int _space;
    bool m_bMove;

    qreal _ratio;
    QScreen *_curScreen;
    QList<QScreen *> _screenList;

    bool b_space;
    bool b_showHead;
};

#endif // CDIALOG_H
