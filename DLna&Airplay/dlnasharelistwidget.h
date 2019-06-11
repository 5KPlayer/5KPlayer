#ifndef DLNASHARELISTWIDGET_H
#define DLNASHARELISTWIDGET_H

#include <QWidget>
#include "dystructu.h"
#include "transeventwidget.h"
#include "transeventscrollarea.h"
#include "updateinterface.h"
#include "dlnafilewidget.h"
class DLnaShareGroupWidget;
class DLnaFileDetailWidget;
class DLnaShareListWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit DLnaShareListWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void clearDLnaWidget();
    void addDLnaWidget(const dlnaLib &dlnaInfo);
    bool addDLnaWidget(int addRows);
    void delDLnaWidget(const dlnaLib &dlnaInfo);

    int  getShowWidgetRows();
    int  getShowWidgetCols();
    void upDLnaSubWidget();
private:
    void init();
    void upDLnaShareMaxRow();
signals:
    void delbtnEnable(bool);
private slots:
    void singleClicked();
    void scroolBarChange(int pos);
protected:
    virtual void mousePressEvent(QMouseEvent *e);   //鼠标按下
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *); // 鼠标双击事件
    virtual void dragEnterEvent(QDragEnterEvent *e);//拖动进入事件
    virtual void dragMoveEvent(QDragMoveEvent *e);  //拖动事件
    virtual void dropEvent(QDropEvent *e);          //放下事件

    virtual void wheelEvent(QWheelEvent *e);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *);
private:
    QTimer *_singleTimer;       //区分 单击事件 还是 双击事件
    QPoint _singleClickedPoint;
    bool _isReleased;

    bool   _isMoveFlag;         // 选中的文件有拖动动作
    bool   _ctrl_isdown;        // Ctrl是否按下
    bool   _shift_isdown;       // Shift是否按下
    bool   _bSelectall;         // 是否选中全部
    int    _showRows;           // 当前显示的row
    int    _showCols;           // 当前一排显示多少个

    QList<DLnaFileWidget *> _dlnaFolders;   //文件夹子控件
    QList<DLnaFileWidget *> _dlnaVideos;    //video子控件
    QList<DLnaFileWidget *> _dlnaMusics;    //music子控件

    DLnaFileWidget *        _dlnaWidget;    //选中的界面

    DLnaFileDetailWidget*   _dlnaDetailWidget;  // 详细界面

    DLnaShareGroupWidget*   _folderGroup;
    DLnaShareGroupWidget*   _videoGroup;
    DLnaShareGroupWidget*   _musicGroup;

    TransEventScrollArea *_area; //滚动区域
    TransEventWidget *_scrollW;  //滚动主界面



    double  _scaleRatio;
};

#endif // DLNASHARELISTWIDGET_H
