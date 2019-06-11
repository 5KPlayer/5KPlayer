#include "rullerwidget.h"
#include <QDebug>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QTime>
#include "globalarg.h"

RullerWidget::RullerWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    _scaleFactor = 1.0;
    _timePerPixel = -1.0;
    _totalTime = 0;
    _width = -1.0;
    _currentPts = -1;
    _leftMidPos = QPoint(0,0);
    _rightMidPos = QPoint(0,0);
    _screenRatio = Global->_screenRatio;
    _lineWidth = 12 * _screenRatio;
    _roundTime = 0;
    _correctTime = 0;
    _maxWindowWidth = 10000;
    _currentPos = QPoint(0,0);
    _currentItemLeft = QPoint(-1,-1);
    _currentItemRight = QPoint(-1,-1);
    connect(this,&RullerWidget::totalTimeChanged,[&]{
        if(_width == -1)
            return;
        setTimePerPixel(_totalTime / _width);
    });
    connect(this,&RullerWidget::scaleFactorChanged,[&]{
        qreal time = TIMEPERPIXEL * _scaleFactor;
        setTimePerPixel(time);
    });
    connect(this,&RullerWidget::timePerPixelChanged,[&]{
        update();
    });
    connect(this,&RullerWidget::windowWidthChanged,[&]{
        this->setFixedWidth(_width);
        if(_totalTime == -1)
            return;
        setTimePerPixel(_totalTime / _width);
    });
    _scale = 120;
}

qreal RullerWidget::scaleFactor() const
{
    return _scaleFactor;
}

qreal RullerWidget::timePerPixel() const
{
    return _timePerPixel;
}

int64_t RullerWidget::totalTime() const
{
    return _totalTime;
}

qreal RullerWidget::windowWidth() const
{
    return _width;
}

void RullerWidget::setScaleFactor(qreal factor)
{
    if(factor < 0.1 || factor > 2.0)
        return;
    if(factor == _scaleFactor)
        return;
    _scaleFactor = factor;
    emit scaleFactorChanged();
}

void RullerWidget::setTimePerPixel(qreal time)
{
    if(time == _timePerPixel)
        return;
    _timePerPixel = time;
    emit timePerPixelChanged();
}

void RullerWidget::setTotalTime(int64_t time)
{
    if(time == _totalTime || time <= 0)
        return;
    _totalTime = time;
    emit totalTimeChanged();
}

void RullerWidget::setWindowWidth(qreal width)
{
    if(width == _width)
        return;
    if(_width != -1 && _width != 0)
    {
        if(_leftMidPosInited)
        {
            _leftMidPos = QPoint((double)_leftMidPts / _totalTime * width,0);
            emit leftMidPosChanged();
        }
        if(_rightMidPosInited)
        {
            _rightMidPos = QPoint((double)_rightMidPts / _totalTime * width,0);
            emit rightMidPosChanged();
        }
    }
    _width = width;
    emit windowWidthChanged();
}

int64_t RullerWidget::getPtsFromPos(QPoint pos)
{
    QPoint p = mapFromGlobal(pos);
    return (double)p.x() / _width * _totalTime;
}

void RullerWidget::drawCurrentLine(int64_t pts,QPoint currentPos)
{
    if(pts < 0 || pts > _totalTime || pts == _currentPts)
        return;
    _currentPts = pts;
    _currentPos = mapFromGlobal(currentPos);
    _currentPts = _currentPos.x() * _timePerPixel;
    update();
}

int64_t RullerWidget::getCurrentPts()
{
    return _currentPts;
}

QPoint RullerWidget::getLeftMidPos(bool source)
{
    if(!_leftMidPosInited)
        return source ? QPoint(0,0) : mapToGlobal(QPoint(0,0));
    return source ? _leftMidPos : mapToGlobal(_leftMidPos);
}

QPoint RullerWidget::getRightMidPos(bool source)
{
    if(!_rightMidPosInited)
        return source ? QPoint(this->width(),0) : mapToGlobal(QPoint(this->width(),0));
    return source ? _rightMidPos : mapToGlobal(_rightMidPos);
}

QPoint RullerWidget::getRightLimitPos()
{
    return mapToGlobal(this->geometry().topRight());
}

void RullerWidget::setLeftMidPos(QPoint pos, bool globalpos)
{
    QPoint p = globalpos ? mapFromGlobal(pos) : pos;
    if(p.x() > _rightMidPos.x() && _rightMidPosInited)
        return;
    if(p.x() > this->width())
        return;
    _leftMidPosInited = true;
    _leftMidPos.setX(p.x());
    _leftMidPts = _leftMidPos.x() / _width * _totalTime;
    emit leftMidPosChanged();
}

void RullerWidget::setRightMidPos(QPoint pos, bool globalpos)
{
    QPoint p = globalpos ? mapFromGlobal(pos) : pos;
    if(p.x() < _leftMidPos.x() && _leftMidPosInited)
        return;
    if(p.x() > this->width())
        return;
    _rightMidPosInited = true;
    _rightMidPos.setX(p.x());
    _rightMidPts = _rightMidPos.x() / _width * _totalTime;
    emit rightMidPosChanged();
}

void RullerWidget::setLeftMidPos(int64_t pts)
{
    if(pts < 0 || pts > _totalTime)
        return;
    _leftMidPts = pts;
    int x = (double)_leftMidPts / _totalTime * _width;
    setLeftMidPos(QPoint(x,0),false);
}

void RullerWidget::setRightMidPos(int64_t pts)
{
    if(pts < 0 || pts > _totalTime)
        return;
    _rightMidPts = pts;
    int x = (double)_rightMidPts / _totalTime * _width;
    setRightMidPos(QPoint(x,0),false);
}

void RullerWidget::addScale()
{
    _scale ++;
    update();
}

void RullerWidget::reduceScale()
{
    _scale --;
    update();
}

void RullerWidget::setScale(int value, QPoint p)
{
    QRegion regin = this->visibleRegion();
    QRect rect = regin.boundingRect();
    QPoint lastPos = mapFromGlobal(p);
    if(lastPos.x() <= rect.left())
        return;

    qreal lastRectTime = (qreal)rect.left() * _timePerPixel + _correctTime; //可见左边的时间戳
    qreal newWidth = (qreal)((lastPos.x()) - rect.left() + value) * _width / (lastPos.x() - rect.left()) + 77;

    if(newWidth < rect.width())
    {
        _correctTime = 0;
        _lineWidth = 12;
        return;
    }
    if(newWidth > _maxWindowWidth)
    {
        _correctTime = 0;
        return;
    }
    _lineWidth = newWidth / _width * _lineWidth;
    if(_lineWidth > 24 * _screenRatio)
    {
        _lineWidth -= 12 * _screenRatio;
        _roundTime++;
    }
    if(_lineWidth < 12 * _screenRatio && _roundTime > 0)
    {
        _lineWidth += 12 * _screenRatio;
        _roundTime--;
    }
    if(_lineWidth < 12 * _screenRatio && _roundTime == 0)
        _lineWidth = 12 * _screenRatio;

    this->setWindowWidth(newWidth);
    qreal moveParam = lastRectTime / _timePerPixel;
    _correctTime = (moveParam - (int)moveParam) * _timePerPixel;    //窗口移动后的修正时间
    move(-moveParam /*+ this->pos().x()*/,0);
}

void RullerWidget::setMaxWindowWidth(int width)
{
    _maxWindowWidth = width;
}

int RullerWidget::getMaxWindowWidth()
{
    return _maxWindowWidth;
}

void RullerWidget::updateUI(const qreal &scaleRatio)
{
    _lineWidth = _lineWidth * scaleRatio / _screenRatio;
    _screenRatio = scaleRatio;
    update();
}

void RullerWidget::setcurrentItemPosAndPts(int64_t pts, int64_t duration, QPoint left, QPoint right)
{
    _currentItemLeft = mapFromGlobal(left);
    _currentItemRight = mapFromGlobal(right);
    _currentItemDuration = duration;
    _currentItemPts = pts;
}


void RullerWidget::paintEvent(QPaintEvent *event)
{
    //优化方案,只绘制当前显示出来的部分
    QRegion regin = this->visibleRegion();
    QRect rect = regin.boundingRect();
    QPainter painter(this);
    QPen pen;
    pen.setWidth(1 * _screenRatio);
    pen.setColor(Qt::gray);
    painter.setPen(pen);
    QFont font;
    font.setPixelSize(12 * _screenRatio);
    painter.setFont(font);
    painter.drawLine(QPoint(rect.left(),this->height() - 2),QPoint(rect.right(),this->height() - 2));   //底部基线
    int startTime = rect.left() * _timePerPixel;
    int endTime = rect.right() * _timePerPixel;
    int timeDuration = endTime - startTime;
    int lineDuration = 0;   //画短线的时间间隔
    if(timeDuration > 0 && timeDuration < 10000)            //0-10  秒
        lineDuration = 100;
    else if(timeDuration >= 10000 && timeDuration < 25000)  //10-25 秒
        lineDuration = 250;
    else if(timeDuration >= 25000 && timeDuration < 50000)  //25-50 秒
        lineDuration = 500;
    else if(timeDuration >= 50000 && timeDuration < 100000) //50-100 秒
        lineDuration = 1000;
    else if(timeDuration >= 100000 && timeDuration < 500000) //100-500 秒
        lineDuration = 5000;
    else if(timeDuration >= 500000 && timeDuration < 1000000) //500-1000 秒
        lineDuration = 10000;
    else if (timeDuration >= 1000000 && timeDuration < 6000000) //1000-6000 秒
        lineDuration = 60000;
    else if(timeDuration >= 6000000 && timeDuration < 12000000) //6000-12000 秒
        lineDuration = 120000;
    else                                                        // >12000 秒
        lineDuration = 300000;
    int longLineDuration = 10;      //画长线和刻度值的间隔
    if(lineDuration == 250)
    {
        if(timeDuration < 12500)
            longLineDuration = 4;
        else
            longLineDuration = 8;
    }
    else if(lineDuration == 500)
        longLineDuration = 8;
    else if(lineDuration == 5000)
    {
        longLineDuration = 6;
        if(timeDuration >= 250000)
            longLineDuration = 12;
    }
    else if(lineDuration == 10000)
        longLineDuration = 6;
    else if(lineDuration == 60000 && timeDuration < 3000000)
        longLineDuration = 5;

    QFontMetrics fm(font);
    int stringWidth = fm.width(QString("00:00:00"));

    if((longLineDuration * lineDuration) / _timePerPixel < stringWidth)
        longLineDuration *= 2;

    for(int i = startTime - startTime % lineDuration; i < endTime; i+=lineDuration)
    {
            int pos = i / _timePerPixel;
            if(i % (longLineDuration * lineDuration) == 0)  //长线+刻度值
            {
                int time = i / 1000;    //总时间
                int h = time / 3600;
                int min = time / 60 % 60;
                int sec = time % 60;
                QTime timeValue;
                timeValue.setHMS(h,min,sec);
                QString formatString = "hh:mm:ss";
                if(time < 60)
                    formatString = "s";
                else if(time >= 60 && time < 3600)
                    formatString = "m:ss";
                else
                    formatString = "h:mm:ss";
                QString timeString = timeValue.toString(formatString);
                painter.drawText(QPoint(pos,this->height() / 2),timeString);
                painter.drawLine(QPoint(pos,this->height() / 2),QPoint(pos,this->height() - 2));
                continue;
            }
            painter.drawLine(QPoint(pos,this->height() / 4 * 3),QPoint(pos,this->height() - 2));    //短线
    }
    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawLine(QPoint(_currentPts / _timePerPixel,0),QPoint(_currentPts / _timePerPixel,this->height() - 2));
}
