#ifndef ICOSMODEWIDGET_H
#define ICOSMODEWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "icosvideo.h"
#include "listsvideo.h"
#include "dystructu.h"
#include "transeventwidget.h"
#include "transeventscrollarea.h"
#include "updateinterface.h"

class QTimer;
class QScrollArea;

/*!
 * \brief The IcosModeWidget class
 *
 * Copyright: Digiarty
 * Author: 赵彦博
 * Date: 2016-7-19
 * Description: 以Icos方式显示的主界面
 *
 */
class IcosModeWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    IcosModeWidget(QWidget *parent = 0);
    ~IcosModeWidget();
    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    /*!
     * \brief addWidget: 增加子界面
     * \param w: 子控件指针
     */
    void addWidget(const MEDIAINFO &videoInfo);

    /*!
     * \brief removeWidget: 移除指定的子界面
     * \param w: 子控件指针
     */
    void removeWidget(IcosVideo *w);

    /*!
     * \brief removeWidget: 移除指定的子界面
     * \param index: 索引号
     */
    void removeWidget(int index);

    /*!
     * \brief clearWidget: 清空子控件
     */
    void clearWidget();

    /*!
     * \brief updateSubWidget: 更新界面
     */
    void updateSubWidget();
    /*!
     * \brief getChooseWidget : 得到选中的界面
     * \return
     */
    IcosVideo* getChooseWidget();
    /*!
     * \brief 当选中界面被删除时，设置选中的面板为空
     */
    void upIcoWidget();
    /*!
     * \brief 清除选中的媒体列表
     */
    void clearSelWidget();
    /*!
     * \brief 删除选中的媒体列表
     */
    void delSelWidget(bool ismove);

    QString getShowItemName() {return _showItemName;}
    void    setShowItemName(QString itemName) {_showItemName = itemName;}

    int     getShowCount() {return wlist.size();}
    void    widgetAddVideo(int count);

private slots:
    void singleClicked();
    void scroolBarChange(int pos);

protected:
    virtual void mousePressEvent(QMouseEvent *);   //鼠标按下
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *); // 鼠标双击事件
    virtual void dragEnterEvent(QDragEnterEvent *);//拖动进入事件
    virtual void dragMoveEvent(QDragMoveEvent *);  //拖动事件
    virtual void dropEvent(QDropEvent *);          //放下事件

    virtual void resizeEvent(QResizeEvent *);

    virtual void wheelEvent(QWheelEvent *);
    virtual void paintEvent(QPaintEvent *e);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void focusOutEvent(QFocusEvent *);

signals:
    void delbtnEnable(bool);

private:
    bool _showDetailInfo;  //是否显示详细信息界面
    QList<IcosVideo *> wlist;//子控件
    QString _showItemName;  //当前界面显示的item

    IcosVideo *_chooseWidget;//选中的界面
    ListsVideo *_detailWidget;//详细信息界面

    TransEventScrollArea *_area; //滚动区域
    TransEventWidget *_scrollW;  //滚动主界面

    QTimer *_singleTimer;       //区分 单击事件 还是 双击事件
    QPoint _singleClickedPoint;
    bool _isReleased;

    bool   _isMoveFlag;         // 选中的文件有拖动动作
    bool   _ctrl_isdown;        // Ctrl是否按下
    bool   _shift_isdown;       // Shift是否按下
    bool   _bSelectall;         // 是否选中全部
    double _scaleRatio;
    int    _min_select;         // 当前选中最小的
    int    _max_select;         // 当前选中最大的
    int    _showCount;          // 当前显示的条目
    bool   _isFolder;
};

#endif // ICOSMODEWIDGET_H
