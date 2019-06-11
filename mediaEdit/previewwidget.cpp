#include "previewwidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <math.h>
#include <QApplication>
#include "globalarg.h"

PreviewWidget::PreviewWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    leftButton = new QSvgWidget(this);
    rightButton = new QSvgWidget(this);
    _screenRatio = Global->_screenRatio;
    leftButton->resize(12 * _screenRatio,20 * _screenRatio);
    rightButton->resize(12 * _screenRatio,20 * _screenRatio);
    leftButton->load(QString(":/res/svg/right_cursor_nor.svg"));
    rightButton->load(QString(":/res/svg/left_cursor_nor.svg"));
//    leftButton->setStyleSheet("border-image: url(:/right_cursor.svg);border-style:flat");
//    rightButton->setStyleSheet("border-image: url(:/left_cursor.svg);border-style:flat");
    leftButton->setAttribute(Qt::WA_TransparentForMouseEvents);
    rightButton->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setMouseTracking(true);
    leftLabel = new PreviewLabel(this);
    rightLabel = new PreviewLabel(this);
    leftLabel->show();
    leftLabel->raise();
    rightLabel->show();
    rightLabel->raise();
    timeLine = new RullerWidget(this);
    timeLine->raise();
    timeLine->setAttribute(Qt::WA_TransparentForMouseEvents);
    _hoverPix.load(":/res/png/scaleHover.png");
    _downPix.load(":/res/png/scaleDown.png");
    _hoverCursor = _hoverPix.scaled(QSize(_hoverPix.size().width() / 2 * _screenRatio,_hoverPix.size().height() / 2 * _screenRatio),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    _downCursor = _downPix.scaled(QSize(_downPix.size().width() / 2 * _screenRatio,_downPix.size().height() / 2 * _screenRatio),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    connect(leftLabel,&PreviewLabel::previewLabelDoubleClicked,this,&PreviewWidget::requestMoveToLeft);
    connect(rightLabel,&PreviewLabel::previewLabelDoubleClicked,this,&PreviewWidget::requestMoveToRight);
    connect(timeLine,&RullerWidget::leftMidPosChanged,[&]{
        QPoint point = timeLine->getLeftMidPos();
        setLeftPos(point);
    });
    connect(timeLine,&RullerWidget::rightMidPosChanged,[&]{
        QPoint point = timeLine->getRightMidPos();
        setRightPos(point);
    });
//    setStyleSheet("background-color:black;");
}

PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::updateUI(const qreal &scaleRatio)
{
    _hoverCursor = _hoverPix.scaled(QSize(_hoverPix.size().width() / 2 * scaleRatio ,_hoverPix.size().height() / 2 * scaleRatio ),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    _downCursor = _downPix.scaled(QSize(_downPix.size().width() / 2 * scaleRatio ,_downPix.size().height() / 2 * scaleRatio ),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    _itemSize = QSize(_itemSize.width() * scaleRatio / _screenRatio,_itemSize.height() * scaleRatio / _screenRatio);
    timeLine->updateUI(scaleRatio);
    timeLine->setWindowWidth(timeLine->windowWidth() * scaleRatio / _screenRatio);
    _screenRatio = scaleRatio;
//    emit requestScaleChange(timeLine->windowWidth());
}

void PreviewWidget::resizeEvent(QResizeEvent *event)
{
    leftButton->resize(12 * _screenRatio,20 * _screenRatio);
    rightButton->resize(12 * _screenRatio,20 * _screenRatio);
    leftButton->move(mapFromGlobal(timeLine->getLeftMidPos()));
    rightButton->move(mapFromGlobal(timeLine->getRightMidPos()));
    timeLine->setFixedHeight(this->height());
    setLeftPreviewPos();
    setRightPreviewPos();
}

void PreviewWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(isButtonDown)
    {
        if(event->pos().x() != currentPos.x() && event->pos().x() > 0 && event->pos().x() < this->width())
        {
            qreal newWidth = (qreal)event->pos().x() / currentPos.x() * timeLine->windowWidth();
            qreal lastRectTime = timeLine->getPtsFromPos(mapToGlobal(this->rect().topLeft())) + _correctTime;
            if(this->width() >= _maxFrameCount * _itemFixedWidth)
            {
                timeLine->setWindowWidth(this->width());
                qreal moveParam = lastRectTime / timeLine->timePerPixel();
                _correctTime = (moveParam - (int)moveParam) * timeLine->timePerPixel();
                timeLine->move(-moveParam,0);
                emit requestScaleChange(timeLine->windowWidth());
                currentPos = event->pos();
                isScaleChanged = true;
                emit leftMidPositionChanged(timeLine->getLeftMidPos());
                emit rightMidPositionChanged(timeLine->getRightMidPos());
                return;
            }
            if(newWidth < this->width())
            {
                timeLine->setWindowWidth(this->width());
            }
            else if(newWidth >= _maxFrameCount * _itemFixedWidth)
            {
                timeLine->setWindowWidth(_maxFrameCount * _itemFixedWidth);
            }
            else
            {
                timeLine->setWindowWidth(newWidth);
            }
            qreal moveParam = lastRectTime / timeLine->timePerPixel();
            _correctTime = (moveParam - (int)moveParam) * timeLine->timePerPixel();
            timeLine->move(-moveParam,0);
            emit requestScaleChange(timeLine->windowWidth());
            currentPos = event->pos();
            emit leftMidPositionChanged(timeLine->getLeftMidPos());
            emit rightMidPositionChanged(timeLine->getRightMidPos());
            isScaleChanged = true;
        }
        return;
    }
    if(leftButton->geometry().contains(event->pos()) || rightButton->geometry().contains(event->pos()))
    {
        qApp->setOverrideCursor(Qt::ArrowCursor);
    }
    else if(isMouseIn && !isButtonDown)
    {
        qApp->setOverrideCursor(QCursor(_hoverCursor));
    }
    if(leftButton->geometry().contains(event->pos()) && !leftpressed)
        leftButton->load(QString(":/res/svg/right_cursor_on.svg"));
    else if(leftpressed)
        leftButton->load(QString(":/res/svg/right_cursor_down.svg"));
    else
        leftButton->load(QString(":/res/svg/right_cursor_nor.svg"));
    if(rightButton->geometry().contains(event->pos()) && !rightpressed)
        rightButton->load(QString(":/res/svg/left_cursor_on.svg"));
    else if(rightpressed)
        rightButton->load(QString(":/res/svg/left_cursor_down.svg"));
    else
        rightButton->load(QString(":/res/svg/left_cursor_nor.svg"));
    if(event->pos().x() < 0 || event->pos().x() + rightButton->width() > this->width())
        return;
    if(leftpressed)
    {
        qApp->setOverrideCursor(Qt::ArrowCursor);
        if(event->pos().x() + leftButton->width() >= rightButton->x())
            return;
        leftButton->move(event->pos().x(),0);
        setLeftPreviewPos();
        timeLine->setLeftMidPos(mapToGlobal(QPoint(leftButton->pos().x(),0)));
        leftLabel->raise();
        emit leftButtonmoved(mapToGlobal(QPoint(leftButton->pos().x(),0)));
        emit leftMidPositionChanged(timeLine->getLeftMidPos());
    }
    else if(rightpressed)
    {
        qApp->setOverrideCursor(Qt::ArrowCursor);
        if(event->pos().x() - leftButton->width() <= leftButton->x())
            return;
        rightButton->move(event->pos().x(),0);
        setRightPreviewPos();
        rightLabel->raise();
        timeLine->setRightMidPos(mapToGlobal(QPoint(rightButton->pos().x() + rightButton->width(),0)));
        emit rightButtonMoved(mapToGlobal(QPoint(rightButton->pos().x() + rightButton->width(),0)));
        emit rightMidPositionChanged(timeLine->getRightMidPos());
    }
    else
        return QWidget::mouseMoveEvent(event);
}

void PreviewWidget::mousePressEvent(QMouseEvent *event)
{
    if(leftButton->geometry().contains(event->pos()))
    {
        qApp->setOverrideCursor(Qt::ArrowCursor);
        leftpressed = true;
        setLeftPreviewTime(getLeftMidPts());
        setLeftPreviewPos();
        leftLabel->raise();
        leftButton->load(QString(":/res/svg/right_cursor_down.svg"));
        timeLine->setLeftMidPos(mapToGlobal(QPoint(leftButton->pos().x(),0)));
        emit leftButtonmoved(mapToGlobal(QPoint(leftButton->pos().x(),0)));
        emit leftMidPositionChanged(timeLine->getLeftMidPos());
    }
    else if(rightButton->geometry().contains(event->pos()))
    {
        qApp->setOverrideCursor(Qt::ArrowCursor);
        rightpressed = true;
        setRightPreviewTime(getRightMidPts());
        setRightPreviewPos();
        rightLabel->raise();
        rightButton->load(QString(":/res/svg/left_cursor_down.svg"));
        timeLine->setRightMidPos(mapToGlobal(QPoint(rightButton->pos().x() + rightButton->width(),0)));
        emit rightButtonMoved(mapToGlobal(QPoint(rightButton->pos().x() + rightButton->width(),0)));
        emit rightMidPositionChanged(timeLine->getRightMidPos());
    }
    else{
        isButtonDown = true;
        qApp->setOverrideCursor(QCursor(_downCursor));
        currentPos = event->pos();
    }
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    emit buttonDragrelease();
    if(isMouseIn && !leftpressed && !rightpressed)
    {
        qApp->setOverrideCursor(QCursor(_hoverCursor));
    }
    else
        qApp->setOverrideCursor(Qt::ArrowCursor);
    if(isButtonDown)
    {
        isButtonDown = false;
        currentPos = event->pos();
        if(isScaleChanged)
            emit requestScaleChangeData();
        isScaleChanged = false;
    }
    leftButton->load(QString(":/res/svg/right_cursor_nor.svg"));
    rightButton->load(QString(":/res/svg/left_cursor_nor.svg"));
    leftpressed = false;
    rightpressed = false;
}

void PreviewWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(leftButton->geometry().contains(event->pos()))
    {
        qDebug() << "request left";
        emit requestMoveToLeft();
    }
    else if(rightButton->geometry().contains(event->pos()))
    {
        qDebug() << "request right";
        emit requestMoveToRight();
    }
}

void PreviewWidget::enterEvent(QEvent *event)
{
    isMouseIn = true;
    qApp->setOverrideCursor(QCursor(_hoverCursor));
}

void PreviewWidget::leaveEvent(QEvent *event)
{
    isMouseIn = false;
    leftButton->load(QString(":/res/svg/right_cursor_nor.svg"));
    rightButton->load(QString(":/res/svg/left_cursor_nor.svg"));
    qApp->setOverrideCursor(Qt::ArrowCursor);
}

void PreviewWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(this->rect(),Qt::black);
}

void PreviewWidget::setLeftPos(QPoint pos)
{
    QPoint p = mapFromGlobal(pos);
    leftButton->move(p.x(),0);
    if(leftButton->x() < 0)
        leftButton->move(0,0);
    if(leftButton->x() + leftButton->width() >= rightButton->x())
        leftButton->move(rightButton->x() - leftButton->width(),0);
    setLeftPreviewPos();

}

void PreviewWidget::setRightPos(QPoint pos)
{
    QPoint p = mapFromGlobal(pos);
    if(p.x() - rightButton->width() < leftButton->x() + leftButton->width())
        rightButton->move(leftButton->x() + leftButton->width(),0);
    rightButton->move(p.x() - rightButton->width(),0);
    if(rightButton->x() + rightButton->width() > this->width())
        rightButton->move(this->width() - rightButton->width(),0);
    if(rightButton->x() - leftButton->width() <= 0)
        rightButton->move(leftButton->x() + leftButton->width(),0);
    setRightPreviewPos();

    //    if(rightButton->x() < leftButton->x() + leftButton->width())

}

void PreviewWidget::setPreviewLabelPos()
{
    setLeftPreviewPos();
    setRightPreviewPos();
}

void PreviewWidget::setLeftPreviewPos()
{

    QPoint leftPos = mapToGlobal(leftButton->pos());
    if(leftPos.x() - leftLabel->width() / 2 <= mapToGlobal(this->pos()).x())
        leftLabel->move(mapToGlobal(this->pos()).x(),leftPos.y() - leftLabel->height());
    else if(leftPos.x() + leftLabel->width() / 2 >= mapToGlobal(this->geometry().topRight()).x())
        leftLabel->move(mapToGlobal(this->geometry().topRight()).x() - leftLabel->width(),leftPos.y() - leftLabel->height());
    else
        leftLabel->move(leftPos.x() - leftLabel-> width() / 2,leftPos.y() - leftLabel->height());
}

void PreviewWidget::setRightPreviewPos()
{

    QPoint rightPos = mapToGlobal(rightButton->pos());
    if(rightPos.x() + rightLabel->width() / 2 >= mapToGlobal(this->geometry().topRight()).x())
        rightLabel->move(mapToGlobal(this->geometry().topRight()).x() - rightLabel->width(),rightPos.y() - rightLabel->height());
    else if(rightPos.x() - rightLabel->width() / 2 <= mapToGlobal(this->geometry().topLeft()).x())
        rightLabel->move(mapToGlobal(this->geometry().topLeft()).x(),rightPos.y() - rightLabel->height());
    else
        rightLabel->move(rightPos.x() - rightLabel-> width() / 2,rightPos.y() - rightLabel->height());
}

void PreviewWidget::setPreviewLabelSize(QSize size)
{
    leftLabel->setLabelSize(QSize(size.width(),size.height() + 20 * _screenRatio));
    rightLabel->setLabelSize(QSize(size.width(),size.height() + 20 * _screenRatio));
}

void PreviewWidget::setRullerPos(int value)
{

    timeLine->move(-value,0);
}

int64_t PreviewWidget::getPtsFromPos(QPoint pos)
{
    return timeLine->getPtsFromPos(pos);
}

int64_t PreviewWidget::getLeftMidPts()
{
    return timeLine->getPtsFromPos(mapToGlobal(QPoint(leftButton->pos().x(),0)));
}

int64_t PreviewWidget::getRightMidPts()
{
    return timeLine->getPtsFromPos(mapToGlobal(QPoint(rightButton->pos().x() + rightButton->width(),0)));
}

QPoint PreviewWidget::getLeftMidPos(bool source)
{
    return timeLine->getLeftMidPos(source);
}

QPoint PreviewWidget::getRightMidPos(bool source)
{
    return timeLine->getRightMidPos(source);
}

void PreviewWidget::setLeftMidPts(int64_t pts)
{
    timeLine->setLeftMidPos(pts);
}

void PreviewWidget::setRightMidPts(int64_t pts)
{
    timeLine->setRightMidPos(pts);
}

void PreviewWidget::addRullerScale()
{
    timeLine->addScale();
}

void PreviewWidget::reduceRullerScale()
{
    timeLine->reduceScale();
}

QPoint PreviewWidget::getRullerPos()
{
    return timeLine->pos();
}

void PreviewWidget::setLimitParams(int maxFrameCount, int minFrameCount, QSize itemSize,int itemFixedWidth)
{
    _maxFrameCount = maxFrameCount;
    _minFrameCount = minFrameCount;
    _itemSize = itemSize;
    _itemFixedWidth = itemFixedWidth;
    if(_itemShowMode == 0)
        timeLine->setMaxWindowWidth(maxFrameCount * (int)(itemSize.width() * pow(0.618,4)));
    else
        timeLine->setMaxWindowWidth(maxFrameCount * _itemFixedWidth + itemSize.width() - _itemFixedWidth);
}

void PreviewWidget::setItemShowMode(int mode)
{
    if(mode == _itemShowMode)
        return;
    _itemShowMode = mode;
    if(_itemShowMode == 0)
        timeLine->setMaxWindowWidth(_maxFrameCount * (int)(_itemSize.width() * pow(0.618,4)));
    else
        timeLine->setMaxWindowWidth(_maxFrameCount * _itemFixedWidth + _itemSize.width() - _itemFixedWidth);

    emit requestScaleChange(timeLine->windowWidth());
    emit requestScaleChangeData();
}

int64_t PreviewWidget::getCurrentPts()
{
    return timeLine->getCurrentPts();
}

void PreviewWidget::setLeftPreviewImage(FRAME frame)
{
    QImage img;
    img.loadFromData(frame.frameData);
    leftLabel->setPixmap(img.copy());
}

void PreviewWidget::setRightPreviewImage(FRAME frame)
{
    QImage img;
    img.loadFromData(frame.frameData);
    rightLabel->setPixmap(img.copy());
}

void PreviewWidget::moveRullerToLeftPts(int64_t pts)
{

}

void PreviewWidget::moveRullerToRightPts(int64_t pts)
{

}

void PreviewWidget::setTotalTime(int64_t time)
{
    timeLine->setTotalTime(time);
}

void PreviewWidget::setRullerWidth(int width)
{
    timeLine->setWindowWidth(width);
}

int PreviewWidget::getRullerWidth()
{
    return timeLine->windowWidth();
}

void PreviewWidget::setLeftPreviewTime(int64_t pts)
{
    leftLabel->setPts(pts);
    leftLabel->raise();
}

void PreviewWidget::setRightPreviewTime(int64_t pts)
{
    rightLabel->setPts(pts);
    rightLabel->raise();
}

void PreviewWidget::drawCurrentPos(int64_t pts,QPoint currentPos)
{
    timeLine->drawCurrentLine(pts,currentPos);
}

void PreviewWidget::setLeftPreviewNodata()
{
    leftLabel->setPixLabelNodata();
}

void PreviewWidget::setRightPreviewNodata()
{
    rightLabel->setPixLabelNodata();
}

void PreviewWidget::setcurrentItemPosAndPts(int64_t pts, int64_t duration, QPoint left, QPoint right)
{
    timeLine->setcurrentItemPosAndPts(pts,duration,left,right);
}



