#include "picturebrowserwidget.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QTime>
#include <math.h>
#include "globalarg.h"
#define FI 0.618

PictureBrowserWidget::PictureBrowserWidget(QWidget *parent) : UpdateInterfaceWidget(parent),
    _leftMidPosition(QPoint(-1,-1)),
    _rightMidPosition(QPoint(-1,-1))
{
    _isExpanded = false;
    _expandedIndex = -1;
    _lastindex = -1;
    _scaleRatio = Global->_screenRatio;
//    setStyleSheet("border:1px solid red;");
}

PictureBrowserWidget::~PictureBrowserWidget()
{
    clearItemList();
}

void PictureBrowserWidget::clearItemList()
{
    qDeleteAll(_itemList);
}

void PictureBrowserWidget::setItemList(QList<PictureItemWidget *> list)
{
    clearItemList();
    _itemList = list;
    emit itemListSizeChanged(_itemList.size());
    if(!_itemList.isEmpty())
    {
//        _itemSize = _itemList[0]->size();
        setStartTime(_itemList[0]->pts());
        setTimeDuration(_itemList[_itemList.size() - 1]->pts() + _itemList[_itemList.size() - 1]->duration() - _itemList[0]->pts());
        setEndTime(_itemList[_itemList.size() - 1]->pts() + _itemList[_itemList.size() - 1]->duration());
        setFrameDuration(_itemList[0]->duration());
    }
    else
        return;
    foreach (PictureItemWidget* item, _itemList) {
//        connect(item,PictureItemWidget::itemClicked,[&]{
//            _itemKeepExpanded = !_itemKeepExpanded;
//        });
        connect(item,SIGNAL(currentItemPtsAndPos(int64_t,QPoint,int)),this,SLOT(setFocusItemWithPosition(int64_t,QPoint,int)));
        connect(item,&PictureItemWidget::currentMousePts,this,&PictureBrowserWidget::currentItemPts);
        connect(item,&PictureItemWidget::leftButtonDoubleClick,this,&PictureBrowserWidget::itemLeftButtonDoubleClick);
        connect(item,&PictureItemWidget::rightButtonDoubleClick,this,&PictureBrowserWidget::itemRightButtonDoubleClick);
        connect(item,&PictureItemWidget::requestItemData,this,&PictureBrowserWidget::itemRequestData);
//        connect(item,PictureItemWidget::currentItemPosAndPts,this,PictureBrowserWidget::currentItemPosAndPts);
//        connect(item,PictureItemWidget::changeItemImage,[&](int curIndex,int index){
//            if(index < 0 || index >= _itemList.size() || curIndex < 0 || curIndex >= _itemList.size())
//                return;
//            _itemList[curIndex]->setImage(_itemList[index]->getPixmap());
//        });
        item->setParent(this);
        item->setItemShowMode(_itemShowMode);
//        item->move(item->index() * (item->width() / 4),0);
//        item->setExpandedLevel(4,false,LEFTTORIGHT);
//        item->raise();
        item->setExpandedLevel(4,false,RIGHTTOLEFT);
        item->show();
    }
    foldAllItems();
//    for(int i = 0; i < _itemList.size(); i++)
//    {
//        connect(_itemList[i],SIGNAL(itemClicked(int)),this,SLOT(setFocusItem(int)));
//        _itemList[i]->setParent(this);
//        _itemList[i]->move(i * _itemList[i]->width() / 4,0);
//        _itemList[i]->raise();
//    }
}

QList<PictureItemWidget *> PictureBrowserWidget::getItemList()
{
    return _itemList;
}

void PictureBrowserWidget::appendItem(PictureItemWidget *item)
{
//    connect(item,PictureItemWidget::itemClicked,[&]{
//        _itemKeepExpanded = !_itemKeepExpanded;
//    });
    connect(item,SIGNAL(currentItemPtsAndPos(int64_t,QPoint,int)),this,SLOT(setFocusItemWithPosition(int64_t,QPoint,int)));
    connect(item,&PictureItemWidget::currentMousePts,this,&PictureBrowserWidget::currentItemPts);
    connect(item,&PictureItemWidget::leftButtonDoubleClick,this,&PictureBrowserWidget::itemLeftButtonDoubleClick);
    connect(item,&PictureItemWidget::rightButtonDoubleClick,this,&PictureBrowserWidget::itemRightButtonDoubleClick);
    connect(item,&PictureItemWidget::requestItemData,this,&PictureBrowserWidget::itemRequestData);
//    connect(item,PictureItemWidget::currentItemPosAndPts,this,PictureBrowserWidget::currentItemPosAndPts);
//    connect(item,PictureItemWidget::changeItemImage,[&](int curIndex,int index){
//        if(index < 0 || index >= _itemList.size() || curIndex < 0 || curIndex >= _itemList.size())
//            return;
//        _itemList[curIndex]->setImage(_itemList[index]->getPixmap());
//    });
    item->setParent(this);
    _itemList.append(item);
    item->show();
    item->lower();
}

int PictureBrowserWidget::getItemCount()
{
    return _itemList.count();
}

void PictureBrowserWidget::setItemImage(int index, FRAME image, bool init)
{
    if(index < 0 || index >= _itemList.size())
        return;
    QRegion regin = this->visibleRegion();
    QRect visiableRect = regin.boundingRect();
    _itemList[index]->setPixmap(image);
    foreach (PictureItemWidget *item, _itemList) {
        if(!item)
            continue;
        if(item->geometry().right() < visiableRect.left())
            continue;
        if(item->geometry().left() > visiableRect.right())
            continue;
        if(!item->isDecoded())
            item->setPixmap(image);
    }
}

void PictureBrowserWidget::decodeFailed(int index, FRAME image)
{
    if(index < 0 || index >= _itemList.size())
        return;
    _itemList[index]->setItemDecodeFailed();
}

void PictureBrowserWidget::setItemInitData(int index,FRAME frame)
{
    if(index < 0 || index >= _itemList.size())
        return;
    _itemList[index]->setInitData(frame);
}

void PictureBrowserWidget::resizeEvent(QResizeEvent *event)
{
    emit sizeChanged(this->width(),this->height());
}

void PictureBrowserWidget::mouseMoveEvent(QMouseEvent *event)
{
    //    qDebug() << "mouse move" << event->pos();
}

void PictureBrowserWidget::setStartTime(int64_t time)
{
    _startTime = time;
}

void PictureBrowserWidget::setEndTime(int64_t time)
{
    _endTime = time;
}

void PictureBrowserWidget::setTimeDuration(int64_t duration)
{
    _timeDuration = duration;
}

void PictureBrowserWidget::setFrameDuration(int64_t duration)
{
    _frameDuration = duration;
}

QPoint PictureBrowserWidget::getCurrentPosition()
{
    if(_isExpanded && _expandedIndex != -1)
        return mapToGlobal(QPoint(_itemList[_expandedIndex]->pos().x() + _itemList[_expandedIndex]->width() / 2,_itemList[_expandedIndex]->pos().y()));
    else
        return QPoint(-1,-1);
}

int64_t PictureBrowserWidget::startTime()
{
    return _startTime;
}

int64_t PictureBrowserWidget::endTime()
{
    return _endTime;
}

int64_t PictureBrowserWidget::timeDuration()
{
    return _timeDuration;
}

int64_t PictureBrowserWidget::frameDuration()
{
    return _frameDuration;
}

void PictureBrowserWidget::setLeftMidPosition(QPoint left)
{
    QPoint p = mapFromGlobal(left);
    if(p.x() >= _rightMidPosition.x() && _leftMidPositionInited && _rightMidPositionInited)
        return;
    _leftMidPosition.setX(p.x());
    _leftMidPositionInited = true;
    emit leftMidPositionChnaged(mapToGlobal(_leftMidPosition));
}

QPoint PictureBrowserWidget::getLeftMidPosition(bool sourceData)
{
    if(!_leftMidPositionInited)
        return sourceData ? QPoint(0,0) : mapToGlobal(QPoint(0,0));
    return sourceData ? _leftMidPosition : mapToGlobal(_leftMidPosition);
}

void PictureBrowserWidget::setRightMidPosition(QPoint right)
{
    QPoint p = mapFromGlobal(right);
    if(p.x() <= _leftMidPosition.x() && _rightMidPositionInited && _leftMidPositionInited)
        return;
    _rightMidPosition.setX(mapFromGlobal(right).x());
    _rightMidPositionInited = true;
    emit rightMidPositionChanged(mapToGlobal(_rightMidPosition));
}

QPoint PictureBrowserWidget::getRightMidPosition(bool sourceData)
{
    if(!_rightMidPositionInited)
        return sourceData ? QPoint(this->width(),0) : mapToGlobal(QPoint(this->width(),0));
    return sourceData ? _rightMidPosition: mapToGlobal(_rightMidPosition);
}

QPoint PictureBrowserWidget::getPictureBrowserPosition()
{
    return mapToGlobal(this->pos());
}

bool PictureBrowserWidget::isExpanded()
{
    return _isExpanded;
}


int64_t PictureBrowserWidget::posTopts(int xpos)
{
    if(xpos < 0 || xpos >= this->width())
        return -1;
    int index = posToIndex(xpos);
    return -1;
}

int PictureBrowserWidget::ptsTopos(int64_t pts)
{
    return 0;
}

int PictureBrowserWidget::ptsToIndex(int64_t pts)
{
    int index = -1;
    for(int i = 0; i < _itemList.size(); i++)
    {
        if(pts >= _itemList[i]->pts() && pts < _itemList[i]->pts() + _itemList[i]->duration())
        {
            index = i;
            break;
        }
    }
    return index;

}

int PictureBrowserWidget::posToIndex(int xpos)
{
    return -1;
}

QPair<int64_t, int64_t> PictureBrowserWidget::indexTopts(int index)
{
    if(_itemList.isEmpty() || index < 0 || index >= _itemList.size())
        return QPair<int64_t,int64_t>(-1,-1);
    return QPair<int64_t,int64_t>(_itemList[index]->pts(),_itemList[index]->pts() + _itemList[index]->duration());
}

PictureItemWidget *PictureBrowserWidget::itemAt(QPoint pos)
{
    QPoint p = mapFromGlobal(pos);
    p.setY(this->height() / 2);
    PictureItemWidget *item = qobject_cast<PictureItemWidget*>(childAt(p));
    return item;
}
PictureItemWidget *PictureBrowserWidget::itemAt(int index)
{
    if(index < 0 || index >= _itemList.size())
        return NULL;
    return _itemList[index];
}

void PictureBrowserWidget::addTail(int pcs)
{
    for(int i = 0; i < pcs; i++)
    {
        PictureItemWidget *wid = new PictureItemWidget(_itemList.size(),this);
        wid->setItemShowMode(_itemShowMode);
        if(_itemShowMode == 0)
            wid->resize(_itemSize);
        else
            wid->resize(_itemFixedWidth,_itemSize.height());
        wid->setExpandedLevel(4,false,RIGHTTOLEFT);
        wid->move(_itemList.last()->pos().x() + _itemFixedWidth,0);
//        FRAME frame = _itemList[_itemList.size() - 1]->getPixmap();
//        wid->setPixmap(frame);
        appendItem(wid);
    }
    emit itemListSizeChanged(_itemList.size());
}

void PictureBrowserWidget::removeTail(int pcs)
{
    bool needFold = false;
    for(int i = 0; i < pcs; i++)
    {
        PictureItemWidget *item = _itemList.last();
        if(item->index() == _lastindex)
        {
            _lastindex = -1;
            needFold = true;
        }
        _itemList.removeLast();
        item->deleteLater();
    }
    if(needFold)
        foldAllItems();
    emit itemListSizeChanged(_itemList.size());
}

void PictureBrowserWidget::updateLanguage()
{

}

void PictureBrowserWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
//    _itemSize.setWidth(_itemSize.width() * scaleRatio);
//    _itemSize.setHeight(_itemSize.height() * scaleRatio);
//    foreach (PictureItemWidget *item, _itemList) {
//        if(_itemShowMode == 0)
//            item->resize(_itemSize);
//        else
//            item->resize(_itemFixedWidth,_itemSize.height());
//        item->setItemShowMode(_itemShowMode);
//    }
    foreach (PictureItemWidget *item, _itemList) {
        item->updateUI(_scaleRatio);
    }
    if(!_itemList.isEmpty())
        foldAllItems(true);
}
void PictureBrowserWidget::setItemSize(QSize size){
    _itemSize = size;
}

void PictureBrowserWidget::setItemShowMode(int mode)
{
    if(_itemShowMode == mode)
        return;
    _itemShowMode = mode;
}

void PictureBrowserWidget::setItemFixedWidth(int width)
{
    _itemFixedWidth = width;
}

void PictureBrowserWidget::setFocusItem(int index)
{
    //封面流+缩放
    int unitWidth = _itemList[0]->width();
    QList<PictureItemWidget*> frontList, tailList;
    frontList = _itemList.mid(0,index);
    tailList = _itemList.mid(index + 1);
    int curX = 0;
    foreach (PictureItemWidget *item, frontList) {
        if(item->index() == index - 1)
        {
            item->move(curX,0);
            item->setExpandedLevel(1,false,LEFTTORIGHT);
            curX += unitWidth * pow(FI,1);
        }
        else if(item->index() == index - 2)
        {
            item->move(curX,0);
            item->setExpandedLevel(2,false,LEFTTORIGHT);
            curX += unitWidth * pow(FI,2);
        }
        else if(item->index() == index - 3)
        {
            item->move(curX,0);
            item->setExpandedLevel(3,false,LEFTTORIGHT);
            curX += unitWidth * pow(FI,3);
        }
        else
        {
            item->move(curX,0);
            item->setExpandedLevel(4,false,LEFTTORIGHT);
            curX += unitWidth * pow(FI,4);
        }
        item->raise();
    }
    _itemList[index]->move(curX,0);
    _itemList[index]->raise();
    _itemList[index]->setExpandedLevel(0,true);
    curX += unitWidth * pow(FI,1);
    foreach (PictureItemWidget *item, tailList) {
        if(item->index() == index + 1)
        {
            item->move(curX,0);
            item->setExpandedLevel(1,false,RIGHTTOLEFT);
            curX += unitWidth * pow(FI,2);
        }
        else if(item->index() == index + 2)
        {
            item->move(curX,0);
            item->setExpandedLevel(2,false,RIGHTTOLEFT);
            curX += unitWidth * pow(FI,3);
        }
        else if(item->index() == index + 3)
        {
            item->move(curX,0);
            item->setExpandedLevel(3,false,RIGHTTOLEFT);
            curX += unitWidth * pow(FI,4);
        }
        else
        {
            item->move(curX,0);
            item->setExpandedLevel(4,false,RIGHTTOLEFT);
            curX += unitWidth * pow(FI,4);
        }
        item->lower();
    }
    this->setFixedWidth(curX + _itemList.size() * FI);
    emit pictureBrowserWidthChanged(this->width());
}

void PictureBrowserWidget::setFocusItemWithPosition(int64_t pts, QPoint pos, int sendindex)
{
    QRect rect = visibleRegion().boundingRect();
    int index = ptsToIndex(pts);
    if(sendindex != -1)
        index = sendindex;
    if(_itemList.isEmpty() || index >= _itemList.size() || index < 0)
        return;
    emit currentItemPts(pts,pos);
    if(_lastindex == index)
        return;
    if(_lastindex < 0 || _lastindex >= _itemList.size())
        _lastindex = 0;
    PictureItemWidget *currentItem = _itemList[index];
    currentItem->resize(_itemSize);
    currentItem->setExpandedLevel(0,true);
    currentItem->raise();
    _itemList[_lastindex]->resize(_itemFixedWidth,_itemSize.height());
    _itemList[_lastindex]->setExpandedLevel(4,false,LEFTTORIGHT);
    _itemList[_lastindex]->lower();
    QList<PictureItemWidget*> frontList = _itemList.mid(0,index);
    QList<PictureItemWidget*> tailList = _itemList.mid(index + 1);
    for(int i = frontList.size() - 1; i >= 0; i--)
    {
        PictureItemWidget *item = frontList[i];
        item->setExpandedLevel(4,false,LEFTTORIGHT);
        if(item->geometry().right() <= rect.left())
            break;
    }
    foreach (PictureItemWidget *item, tailList) {
        item->setExpandedLevel(4,false,RIGHTTOLEFT);
        if(item->geometry().left() >= rect.right())
            break;
    }
    _lastindex = index;
}

void PictureBrowserWidget::foldAllItems(bool force)
{
    if(_itemList.isEmpty())
        return;
    if(force)
    {
        if(_lastindex == -1)
            return;
        int curX = 0;
        foreach (PictureItemWidget *item, _itemList) {
            item->resize(_itemFixedWidth,_itemSize.height());
            item->setItemShowMode(_itemShowMode);
            item->move(curX,0);
            curX += _itemFixedWidth;
            if(item->index() < _lastindex)
            {
                item->resize(_itemFixedWidth,_itemSize.height());
                item->setExpandedLevel(4,false,LEFTTORIGHT);
            }
            else if(item->index() > _lastindex)
            {
                item->resize(_itemFixedWidth,_itemSize.height());
                item->setExpandedLevel(4,false,RIGHTTOLEFT);
            }
            else
            {
                item->resize(_itemSize.width(),_itemSize.height());
                item->setExpandedLevel(0,true);
                item->raise();
            }
        }
    }
    if(_lastindex == -1)
    {
        int index = _itemList.size() % 2 == 0 ? _itemList.size() / 2 : (_itemList.size() / 2 + 1);
        int curX = 0;
        foreach (PictureItemWidget *item, _itemList) {
            item->resize(_itemFixedWidth,item->height());
            item->setItemShowMode(_itemShowMode);
            if(item->index() < index)
            {
                item->setExpandedLevel(4,false,LEFTTORIGHT);
            }
            else if(item->index() > index)
            {
                item->setExpandedLevel(4,false,RIGHTTOLEFT);
            }
            else
            {
                item->setExpandedLevel(0,true);
                item->resize(_itemSize.width(),_itemSize.height());
            }
            item->move(curX,0);
            curX += _itemFixedWidth;
        }
        PictureItemWidget *currentItem = _itemList[index];
        currentItem->raise();
        _lastindex = index;
        emit currentItemPts(_itemList[_lastindex]->pts(),mapToGlobal(_itemList[_lastindex]->pos()));
    }
}

void PictureBrowserWidget::setItemRepaintable(bool b)
{
    QRect rect = visibleRegion().boundingRect();
    PictureItemWidget *firstItem = (PictureItemWidget*)childAt(rect.topLeft());
    PictureItemWidget *lastItem = (PictureItemWidget*)childAt(rect.topRight());
    QList<PictureItemWidget*> itemList;
    if(firstItem && lastItem)
        itemList = _itemList.mid(firstItem->index(),lastItem->index() - firstItem->index() + 1);
    else
        itemList = _itemList;
    foreach (PictureItemWidget *item, itemList) {
        item->setRepaintable(b);
    }
}
