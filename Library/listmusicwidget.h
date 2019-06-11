#ifndef LISTMUSICWIDGET_H
#define LISTMUSICWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "dystructu.h"
#include "transeventwidget.h"
#include "transeventscrollarea.h"
#include "updateinterface.h"

class QTimer;
class QScrollArea;
class ListMusic;

class ListMusicWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ListMusicWidget(QWidget *parent = 0);
    ~ListMusicWidget();

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void    initData(QList<MEDIAINFO> mediaList);           // 初始化数据
    void    upWidgetData(QList<MEDIAINFO> mediaList);       // 更新数据
    void    initWidget(bool init,bool initScrool);          // initScrool=false，滚动条的位置不变， initScrool=true，滚动条位置到0

    void    clearWidget();                                  // 清除界面
    void    updateSubWidget();                              // 更新界面
    void    clearSelWidget();                               // 清除选中
    void    delSelWidget(bool ismove);                      // 删除选中

    QString getShowItemName(){return _showItemName;}
    void    setShowItemName(QString ItemName){_showItemName = ItemName;}
    int     getshowWidgetCount() {return wlist.size();}

signals:
    void delbtnEnable(bool);

private slots:
    void singleClicked();
    void scroolBarChange(int pos);

protected:
    virtual void mousePressEvent(QMouseEvent *);        //鼠标按下
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);  // 鼠标双击事件
    virtual void dragEnterEvent(QDragEnterEvent *);     //拖动进入事件
    virtual void dragMoveEvent(QDragMoveEvent *);       //拖动事件
    virtual void dropEvent(QDropEvent *);               //放下事件

    virtual void resizeEvent(QResizeEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void paintEvent(QPaintEvent *e);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void focusOutEvent(QFocusEvent *);

private:
    QList<ListMusic *> wlist;//子控件
    QString _showItemName;  //当前界面显示的item

    ListMusic *_chooseWidget;//鼠标点中的界面

    TransEventScrollArea *_area; //滚动区域
    TransEventWidget *_scrollW;  //滚动主界面

    TransEventWidget*   _pListWidget;   // listWidget主界面
    QScrollBar*         _pScrollBar;    // 滚动条

    QTimer *_singleTimer;       //区分 单击事件 还是 双击事件
    QPoint _singleClickedPoint;
    bool _isReleased;

    bool   _isMoveFlag;         // 选中的文件有拖动动作
    bool   _ctrl_isdown;        // Ctrl是否按下
    bool   _shift_isdown;       // Shift是否按下
    bool   _bSelectall;         // 是否选中全部
    int    _min_select;         // 当前选中最小的
    int    _max_select;         // 当前选中最大的
    bool   _isFolder;
    double _scaleRatio;

    QList<MEDIAINFO> _widgetData;       //widget中的媒体数据
    bool             _init;
    int              _pScrollMaxValue;
};

#endif // LISTMUSICWIDGET_H
