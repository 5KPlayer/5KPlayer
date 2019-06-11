#ifndef PICTUREBROWSERWIDGET_H
#define PICTUREBROWSERWIDGET_H

#include <QWidget>
#include "pictureitemwidget.h"
#include <QList>
#include "updateinterface.h"

class PictureBrowserWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit PictureBrowserWidget(QWidget *parent = 0);
    ~PictureBrowserWidget();
    //清空list中的数据
    void clearItemList();

    void setItemList(QList<PictureItemWidget*> list);
    QList<PictureItemWidget*> getItemList();
    void appendItem(PictureItemWidget *item);
    int getItemCount();

    void setItemImage(int index, FRAME image, bool init = false);
    void decodeFailed(int index,FRAME image);
    void setItemInitData(int index, FRAME frame);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void setStartTime(int64_t time);
    void setEndTime(int64_t time);
    void setTimeDuration(int64_t duration);
    void setFrameDuration(int64_t duration);

    QPoint getCurrentPosition();

    int64_t startTime();
    int64_t endTime();
    int64_t timeDuration();
    int64_t frameDuration();

    void setLeftMidPosition(QPoint left);
    QPoint getLeftMidPosition(bool sourceData = false);
    void setRightMidPosition(QPoint right);
    QPoint getRightMidPosition(bool sourceData = false);
    QPoint getPictureBrowserPosition();
    bool isExpanded();

    int64_t posTopts(int xpos);
    int ptsTopos(int64_t pts);
    int ptsToIndex(int64_t pts);
    int posToIndex(int xpos);
    QPair<int64_t,int64_t> indexTopts(int index);

    PictureItemWidget *itemAt(QPoint pos);
    PictureItemWidget *itemAt(int index);

    void addTail(int pcs);
    void removeTail(int pcs);
    virtual void updateLanguage();
    virtual void updateUI(const qreal &scaleRatio);
    void setItemSize(QSize size);
    void setItemShowMode(int mode);
    void setItemFixedWidth(int width);

signals:
    void sizeChanged(int width,int height);
    void currentItemPos(QPoint pos);
    void leftMidPositionChnaged(QPoint left);
    void rightMidPositionChanged(QPoint right);
    void pictureBrowserWidthChanged(int width);
    void currentItemPts(int pts,QPoint currentPos);
    void itemLeftButtonDoubleClick(int64_t pts,QPoint pos);
    void itemRightButtonDoubleClick(int64_t pts,QPoint pos);
    void itemRequestData(int64_t pts,int index);
    void itemListSizeChanged(int size);
    void currentItemPosAndPts(int64_t pts,int64_t duration,QPoint left,QPoint right);
public slots:
    void setFocusItem(int index);
    void setFocusItemWithPosition(int64_t pts, QPoint pos, int sendindex = -1);
    void foldAllItems(bool force = false);
    void setItemRepaintable(bool b);
private:
    QList<PictureItemWidget*> _itemList;
    bool _isExpanded;
    int _expandedIndex;
    QSize _itemSize;
    //开始时间
    int64_t _startTime = 0;
    //结束时间
    int64_t _endTime = 0;
    //总时间
    int64_t _timeDuration = 0;
    //一帧的时长
    int64_t _frameDuration = 0;

    //左边剪切开始位置
    QPoint _leftMidPosition;
    //右边剪切结束位置
    QPoint _rightMidPosition;

    bool _leftMidPositionInited = false;
    bool _rightMidPositionInited = false;

    bool _itemKeepExpanded = false;
    int _lastindex = -1;
    double _scaleRatio;
    int _itemShowMode = 0;
    int _itemFixedWidth;
    bool _isFolded = false;
    bool _itemRepaintable = true;
};

#endif // PICTUREBROWSERWIDGET_H
