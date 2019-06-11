#ifndef EDITDIALOG_H
#define EDITDIALOG_H
#include <QDialog>
#include "updateinterface.h"
#include "editcutwidget.h"

#define REGION_COUNT 9

class QPropertyAnimation;
class EditRightWidget;
class EditDialog : public QDialog, public UpdateInterface
{
    Q_OBJECT
public:
    EditDialog(QWidget *parent = 0);
    ~EditDialog();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void setShowAnimation();        // 右边和下边的画面动画显示

private slots:
    void upGeometry();
private:
    //鼠标位置九宫格
    enum WidgetRegion
    {
        Top = 0,
        TopRight,
        Right,
        RightBottom,
        Bottom,
        LeftBottom,
        Left,
        LeftTop,
        Inner,
        Unknown
    };

    void initGeometry(const int &x, const int &y, const int &w, const int &h);
    //根据鼠标判断当前所在区域
    WidgetRegion hitTest(const QPoint &pos);
    //设置边缘宽度
    void setBorderWidth();
    //根据区域设置鼠标样式
    void setCursorSharpe(const EditDialog::WidgetRegion &regin);
    // 鼠标从边框快速移到窗体内子控件上，可能会造成鼠标样式未改变，这里使用计时器监控
    void startCursorTimer();
    void stopCursorTimer();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    EditCutWidget*    _bottomWidget;
    EditRightWidget*  _rightWidget;

    double  _scaleRatio;
    bool    _mousePress;
    bool    _inited;

    // 记录鼠标按下时所点击的区域
    WidgetRegion _regionPressed;

    // 九宫格，对应9个区域
    QRect _regions[REGION_COUNT];

    //定时器ID
    int i_cursorTimerId;

protected:

private slots:
};

#endif // EDITDIALOG_H
