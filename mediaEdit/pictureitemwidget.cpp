#include "pictureitemwidget.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTime>
#include "globalarg.h"

#define PERCENT 10


PictureItemWidget::PictureItemWidget(int index, QWidget *parent) : UpdateInterfaceWidget(parent)
{
//    setAttribute(Qt::WA_TransparentForMouseEvents);
//    pixLabel = new QLabel(QString::number(index),this);
//    pixLabel->setAlignment(Qt::AlignLeft);
    m_index = index;
    m_expanded = false;
    m_expandLevel = -1;
    setMouseTracking(true);
    setContextMenuPolicy(Qt::NoContextMenu);
    _scaleRatio = Global->_screenRatio;
//    this->setStyleSheet("border: 1px solid blue;");
}

bool PictureItemWidget::isExpanded()
{
    return m_expanded;
}

void PictureItemWidget::setInitData(FRAME frame)
{
    if(m_frame.time_pts == frame.time_pts && m_frame.frameDration == frame.frameDration)
        return;
    m_frame = frame;
    _isDecoded = false;
    _itemDecodeFailed = false;
    if(_requestEmited)
        _requestEmited = false;
    update();
}

void PictureItemWidget::setPixmap(FRAME frame)
{
//    if(m_frame.time_pts == frame.time_pts && m_frame.frameDration == frame.frameDration)
//        return;
//    m_frame = frame;
    if(frame.time_pts == m_frame.time_pts)
        _isDecoded = true;
    m_frame.frameData = frame.frameData;
    QImage tmp;
    tmp.loadFromData(frame.frameData);
    m_image = tmp.copy();
    needRepaint = true;
    update();
}

QImage PictureItemWidget::getPixmap()
{
    QImage img;
    img.loadFromData(m_frame.frameData);
    return img;
}

void PictureItemWidget::setImage(QImage image)
{
    m_image = image;
    update();
}

bool PictureItemWidget::isDecoded()
{
    return !m_image.isNull();
}

int PictureItemWidget::index()
{
    return m_index;
}

int64_t PictureItemWidget::pts()
{
    return m_frame.time_pts;
}

int64_t PictureItemWidget::duration()
{
    return m_frame.frameDration;
}

void PictureItemWidget::updateLanguage()
{

}

void PictureItemWidget::updateUI(const qreal &scaleRatio)
{
    if(_scaleRatio == scaleRatio)
        return;
    _scaleRatio = scaleRatio;
}

void PictureItemWidget::setItemShowMode(int mode)
{
    if(_itemShowMode == mode)
        return;
    _itemShowMode = mode;
    update();
}

void PictureItemWidget::setItemDecodeFailed()
{
    _itemDecodeFailed = true;
    update();
}

void PictureItemWidget::resizeEvent(QResizeEvent *event)
{
}

void PictureItemWidget::mousePressEvent(QMouseEvent *event)
{
//    emit itemClicked(m_index);
}

void PictureItemWidget::paintEvent(QPaintEvent *event)
{
    if(!_requestEmited && _repaintAble)
    {
        emit requestItemData(pts(),index());
        _requestEmited = true;
    }
    if(_itemDecodeFailed)
    {
        QPainter painter(this);
        painter.setPen(Qt::red);
        painter.drawText(this->rect(),"No Data");
        if(m_direction == LEFTTORIGHT)
        {
            QRect whiteRect(this->width() - (4 - m_expandLevel + 1),0,4 - m_expandLevel + 1,this->height());
            painter.fillRect(whiteRect,Qt::white);
        }
        else if(m_direction == RIGHTTOLEFT)
        {
            QRect whiteRect(0,0,4 - m_expandLevel + 1,this->height());
            painter.fillRect(whiteRect,Qt::white);
        }
        return;
    }
    if(m_image.isNull())
    {
        return QWidget::paintEvent(event);
    }
//    //封面流 + 缩放
    if(_itemShowMode == 0)
    {
        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(100,100,100));
        QPainter painter(this);
        painter.setPen(pen);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        if(m_expanded)
        {
            painter.drawImage(this->rect(),m_image,m_image.rect());
            painter.drawRect(this->rect());
        }
        else
        {
            QPainterPath path;
            QPolygon poly;
            QBrush brush(m_image);
            if(m_direction == LEFTTORIGHT)
            {
                if(m_expandLevel == 1)
                {
                    poly.append(QPoint(0,0));
                    poly.append(QPoint(0,this->height()));
                    poly.append(QPoint(this->width(),this->height() - this->height() / PERCENT));
                    poly.append(QPoint(this->width(),this->height() / PERCENT));
                }
                else if(m_expandLevel == 2)
                {
                    poly.append(QPoint(0,0));
                    poly.append(QPoint(0,this->height()));
                    poly.append(QPoint(this->width(),this->height() - this->height() / PERCENT * 2));
                    poly.append(QPoint(this->width(),this->height() / PERCENT * 2));
                }
                else if(m_expandLevel == 3)
                {
                    poly.append(QPoint(0,0));
                    poly.append(QPoint(0,this->height()));
                    poly.append(QPoint(this->width(),this->height() - this->height() / PERCENT * 3));
                    poly.append(QPoint(this->width(),this->height() / PERCENT * 3));
                }
                else
                {
                    poly.append(QPoint(0,0));
                    poly.append(QPoint(0,this->height()));
                    poly.append(QPoint(this->width(),this->height() - this->height() / PERCENT * 4));
                    poly.append(QPoint(this->width(),this->height() / PERCENT * 4));
                }
                poly.append(QPoint(0,0));
            }
            else
            {
                if(m_expandLevel == 1)
                {
                    poly.append(QPoint(this->width(),0));
                    poly.append(QPoint(this->width(),this->height()));
                    poly.append(QPoint(0,this->height() - this->height() / PERCENT));
                    poly.append(QPoint(0,this->height() / PERCENT));
                }
                else if(m_expandLevel == 2)
                {
                    poly.append(QPoint(this->width(),0));
                    poly.append(QPoint(this->width(),this->height()));
                    poly.append(QPoint(0,this->height() - this->height() / PERCENT * 2));
                    poly.append(QPoint(0,this->height() / PERCENT * 2));
                }
                else if(m_expandLevel == 3)
                {
                    poly.append(QPoint(this->width(),0));
                    poly.append(QPoint(this->width(),this->height()));
                    poly.append(QPoint(0,this->height() - this->height() / PERCENT * 3));
                    poly.append(QPoint(0,this->height() / PERCENT * 3));
                }
                else
                {
                    poly.append(QPoint(this->width(),0));
                    poly.append(QPoint(this->width(),this->height()));
                    poly.append(QPoint(0,this->height() - this->height() / PERCENT * 4));
                    poly.append(QPoint(0,this->height() / PERCENT * 4));
                }
                poly.append(QPoint(this->width(),0));
            }
            path.addPolygon(poly);
            painter.fillPath(path,brush);
            painter.drawPath(path);
        }
    }
    else
    {
        QPainter painter(this);
        painter.setPen(Qt::red);

        if(m_expanded)
        {
            QRect rect = this->rect();
            QRect leftRect(0,0,qRound(6 * _scaleRatio),rect.height());
            QRect midRect(qRound(6 * _scaleRatio),0,rect.width() - qRound(12 * _scaleRatio),rect.height());
            QRect rightRect(this->width() - qRound(6 * _scaleRatio),0,qRound(6 * _scaleRatio),rect.height());
            QImage tmpimg;
            tmpimg = m_image.scaled(this->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            painter.drawImage(midRect,tmpimg,tmpimg.rect());
            painter.fillRect(leftRect,Qt::white);
            painter.fillRect(rightRect,Qt::white);
        }
        else
        {
            QRect rect = this->rect();
            QImage  tmpimg;
            tmpimg = m_image.scaled(QSize(this->width() * 3,this->height()),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            QRect srcRect = tmpimg.rect();
            srcRect.setLeft(srcRect.left() + (srcRect.width() - rect.width()) / 2);
            srcRect.setWidth(rect.width());
            painter.drawImage(rect,tmpimg,srcRect,Qt::NoFormatConversion);
            if(m_direction == LEFTTORIGHT)
            {
                QRect whiteRect(this->width() -  qRound(1 * _scaleRatio),0, qRound(1 * _scaleRatio),this->height());
                painter.fillRect(whiteRect,Qt::white);
            }
            else if(m_direction == RIGHTTOLEFT)
            {
                QRect whiteRect(0,0, qRound(1 * _scaleRatio),this->height());
                painter.fillRect(whiteRect,Qt::white);
            }
        }
    }
    needRepaint = false;
}

void PictureItemWidget::enterEvent(QEvent *event)
{
    _mouseIn = true;
    QEnterEvent *e = static_cast<QEnterEvent*>(event);
    int time = (double)e->pos().x() / this->width() * 3 * duration() + pts();/*pts();*/
    emit currentMousePts(time,mapToGlobal(e->pos()));
//    emit currentItemPosAndPts(pts(),duration(),mapToGlobal(this->geometry().topLeft()),mapToGlobal(this->geometry().topRight()));
    if(e->pos().x() > 0 && e->pos().x() < this->width() / 3)
        emit changeItemImage(m_index,m_index);
    else if(e->pos().x() >= this->width() / 3 && e->pos().x() < this->width() / 3 * 2)
        emit changeItemImage(m_index,m_index + 1);
    else if(e->pos().x() >= this->width() / 3 * 2 && e->pos().x() < this->width())
        emit changeItemImage(m_index,m_index + 2);
    emit currentItemPtsAndPos(time,mapToGlobal(e->pos()),m_index);

}

void PictureItemWidget::leaveEvent(QEvent *event)
{
    _mouseIn = false;
    m_image.loadFromData(m_frame.frameData);
    update();
    emit mouseLeave();
}

void PictureItemWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(_mouseIn)
    {
//        emit itemClicked(m_index);
        int time = (double)event->pos().x() / this->width() * 3 * duration() + pts();/*pts();*/
        emit currentMousePts(time,mapToGlobal(event->pos()));
        if(event->pos().x() > 0 && event->pos().x() < this->width() / 3)
            emit changeItemImage(m_index,m_index);
        else if(event->pos().x() >= this->width() / 3 && event->pos().x() < this->width() / 3 * 2)
            emit changeItemImage(m_index,m_index + 1);
        else if(event->pos().x() >= this->width() / 3 * 2 && event->pos().x() < this->width())
            emit changeItemImage(m_index,m_index + 2);
//        emit currentItemPosAndPts(pts(),duration(),mapToGlobal(this->geometry().topLeft()),mapToGlobal(this->geometry().topRight()));
        emit currentItemPtsAndPos(time,mapToGlobal(event->pos()),m_index);

    }
}

void PictureItemWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        int time = (double)event->pos().x() / this->width() * 3 * duration() + pts();
        emit leftButtonDoubleClick(time,mapToGlobal(event->pos()));
    }
    else if(event->button() == Qt::RightButton)
    {
        int time = (double)event->pos().x() / this->width() * 3 * duration() + pts();
        emit rightButtonDoubleClick(time,mapToGlobal(event->pos()));
    }
}

void PictureItemWidget::setExpandedLevel(int level, bool b, DIRECTION dir)
{
    if(m_expanded == b && m_expandLevel == level && m_direction == dir)
        return;
    m_expanded = b;
    m_expandLevel = level;
    m_direction = dir;
    needRepaint = true;
    update();
//    if(b)
//    {
////        pixLabel->resize(this->width(),this->height());
////        pixLabel->move(0,0);
//    }
//    else
//    {
////        pixLabel->resize(this->width(),this->height() - level);
////        pixLabel->move(0,level / 2);
    //    }
}

void PictureItemWidget::setRepaintable(bool b)
{
    if(_repaintAble == b)
        return;
    _repaintAble = b;
    if(_repaintAble)
        update();
}
