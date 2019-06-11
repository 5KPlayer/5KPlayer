#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include "dystructu.h"
#include "updateinterface.h"

enum DIRECTION{
    NONE,
    LEFTTORIGHT,
    RIGHTTOLEFT,
};

class PictureItemWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit PictureItemWidget(int index,QWidget *parent = 0);
    bool isExpanded();  //Item是否为展开状态
    void setInitData(FRAME frame);
    void setPixmap(FRAME frame);    //设置Item的数据
    QImage getPixmap();              //获取Item数据
    void setImage(QImage image);
    bool isDecoded();
    int index();                    //获取Item的索引
    int64_t pts();                  //获取Item的时间
    int64_t duration();             //获取Item的时长
    virtual void updateLanguage();
    virtual void updateUI(const qreal &scaleRatio);
    void setItemShowMode(int mode);
    void setItemDecodeFailed();
protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
signals:
    void itemClicked(int index);
    void currentMousePts(int pts,QPoint currentPos);  //鼠标在Item上的位置对应的时间
    void mouseLeave();              //鼠标移开Item
    void currentItemPtsAndPos(int64_t pts,QPoint pos,int index);  //鼠标当前对应的Item的时间和全局位置
    void leftButtonDoubleClick(int64_t pts,QPoint pos);
    void rightButtonDoubleClick(int64_t pts,QPoint pos);
    void requestItemData(int64_t pts,int index);
    void currentItemPosAndPts(int64_t pts,int64_t duration,QPoint left,QPoint right);
    void changeItemImage(int curIndex, int index);
public slots:
    void setExpandedLevel(int level,bool b,DIRECTION dir = NONE);   //设置Item的缩放等级
    void setRepaintable(bool b);
private:
    int m_index;                    //Item的索引
    bool m_expanded;                //是否展开
    QImage m_image;                 //图像
    int m_expandLevel;              //展开等级
    FRAME m_frame;                  //图像数据
    bool _mouseIn = false;          //
    DIRECTION m_direction = NONE;   //Item的方向
    bool needRepaint = true;
    bool _requestEmited = false;
    bool _repaintAble = false;
    double _scaleRatio;
    int _itemShowMode = 0;
    bool _isDecoded = false;
    bool _itemDecodeFailed = false;
};

#endif // ITEMWIDGET_H
