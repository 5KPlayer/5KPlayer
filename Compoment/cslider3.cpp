#include "cslider3.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPolygonF>
#include <QtDebug>

CSlider3::CSlider3(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    b_showAllText   = false;
    b_mousePress    = false;
    b_enable        = false;
    setRange(0, 100);
    setPageStep(1.0);
}

void CSlider3::initSize(const int &w, const int &h)
{
    _originalWidth = w;
    _originalHeight = h;

    updateUI(1.0);
}

void CSlider3::initSize(const QSize &size)
{
    initSize(size.width(), size.height());
}

void CSlider3::setPageStep(const qreal &step)
{
    _step = step;
    if(step == 0.0)
        _step = _rangeEnded / 100.0;
}

void CSlider3::setRange(const double &rangeA, const double &rangeB)
{
    if(rangeA == rangeB)
        return;
    _rangeStart = qMin(rangeA, rangeB);
    _rangeEnded = qMax(rangeA, rangeB);
}

void CSlider3::setLineOrText(bool showtext)
{
    b_showAllText   = showtext;
}

void CSlider3::setValue(const double &value)
{
    _curValue = value;
    emit valueChanged(value);
    _curXPos = valueToXPos(value);
    this->update();
}

double CSlider3::getValue()
{
    return _curValue;
}

int CSlider3::valueToXPos(const double &value)
{
    double tmpValue = value;
    if(value < _rangeStart)
        tmpValue = _rangeStart;
    else if(value > _rangeEnded)
        tmpValue = _rangeEnded;

    const int tmpX = availableXPos(_minXPos + (_maxXPos-_minXPos) * (tmpValue-_rangeStart)/(_rangeEnded-_rangeStart));
    return tmpX;
}

double CSlider3::XPosToValue(const int &xPos)
{
    const int tmpPos = availableXPos(xPos);
    double tmpValue = _rangeStart + ((double)(tmpPos-_minXPos))/(_maxXPos-_minXPos)*(_rangeEnded-_rangeStart);
    return tmpValue;
}

void CSlider3::setSliderEnable(bool enable)
{
    this->setEnabled(!enable);
    b_enable = enable;
    this->update();
}

void CSlider3::updateUI(const qreal &scaleRatio)
{
    _ratio = scaleRatio;

    const int w = _originalWidth  * scaleRatio;
    const int h = _originalHeight * scaleRatio;

    _minXPos = 5*scaleRatio;
    _maxXPos = w - _minXPos*2;
    _curXPos = valueToXPos(_curValue);

    this->setFixedSize(w, h);
}

int CSlider3::availableXPos(const int &value)
{
    if(value < _minXPos) return _minXPos;
    if(value > _maxXPos) return _maxXPos;
    return value;
}

void CSlider3::mousePressEvent(QMouseEvent *e)
{
    b_mousePress = true;
    const int tmpXPos = availableXPos(e->pos().x());
    if(tmpXPos != _curXPos)
    {
        qreal value = XPosToValue(tmpXPos);
        const int tmp = int(value / _step);
        if(tmp * _step != value && (qAbs(value - tmp * _step) >= _step/2)) {
            if(value > 0) {
                value = (tmp + 1) * _step;
            } else {
                value = (tmp - 1) * _step;
            }
        } else {
            value = tmp * _step;
        }
        if(_curValue != value)
        {
            _curValue = value;
            _curXPos = valueToXPos(value);
            emit valueChanged(value);
        }
    }
    this->update();
    e->accept();
}

void CSlider3::mouseMoveEvent(QMouseEvent *e)
{
    const int tmpXPos = availableXPos(e->pos().x());
    if(tmpXPos != _curXPos)
    {
        qreal value = XPosToValue(tmpXPos);
        const int tmp = int(value / _step);
        if(tmp * _step != value && (qAbs(value - tmp * _step) >= _step/2)) {
            if(value > 0) {
                value = (tmp + 1) * _step;
            } else {
                value = (tmp - 1) * _step;
            }
        } else {
            value = tmp * _step;
        }
        if(_curValue != value)
        {
            _curValue = value;
            _curXPos = valueToXPos(value);
            emit valueChanged(value);
            this->update();
        }
    }
    e->accept();
}

void CSlider3::mouseReleaseEvent(QMouseEvent *e)
{
    b_mousePress = false;
    this->update();
    e->accept();
}

void CSlider3::paintEvent(QPaintEvent *)
{
    //设置画笔颜色
    QPen pen;
    pen.setColor(QColor(44,44,44));
    pen.setWidth(1 * _ratio);

    QPainter p(this);
    p.setPen(pen);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int w = this->width();
    const int h = this->height();
    const int y = h/2;
    // 主线
    p.drawLine(_minXPos,y,_maxXPos,y);

    if(b_showAllText) {
        pen.setColor(QColor(166,166,166));
        p.setPen(pen);
        // 开头的值
        p.drawText(QRect(0, y, 28 * _ratio, y), Qt::AlignVCenter|Qt::AlignLeft,
                   QString::number(_rangeStart));

        // 结束的值
        p.drawText(QRect(w-28*_ratio, y ,28*_ratio, y), Qt::AlignVCenter|Qt::AlignRight,
                   QString::number(_rangeEnded));

        // 画0的值的短线和字
        if(_rangeStart < 0) {
            int zeroX = valueToXPos(0);
            //p.drawLine(zeroX, y, zeroX, y-6 * _ratio);
            p.drawText(QRect(zeroX-4*_ratio, y ,8*_ratio, y), Qt::AlignVCenter|Qt::AlignRight,
                       QString::number(0));
        }
    }

    double minValue = 0;
    if(minValue < _rangeStart)
        minValue = _rangeStart;
    int zeroXpos = valueToXPos(minValue);
    QBrush brush;
    if(b_mousePress) {
        pen.setColor(QColor(3,141,228));
        brush = QBrush(QColor(3,141,228),Qt::SolidPattern);
    } else {
        QColor color = QColor(166,166,166);
        if(b_enable) {
            color = QColor(44,44,44);
        }
        pen.setColor(color);
        brush = QBrush(color,Qt::SolidPattern);
    }
    p.setPen(pen);
    // 画主线上的进度线
    p.drawLine(_curXPos,y,zeroXpos,y);

    if(b_mousePress) {
        // 按下外面的圈
        p.drawEllipse(_curXPos-5*_ratio,y-5*_ratio,10*_ratio,10*_ratio);
        // 按下时画当前值的字
        if(b_showAllText) {
            int zeroX = valueToXPos(_curValue);
            p.drawText(QRect(zeroX-16*_ratio, y+5*_ratio ,32*_ratio, y-5*_ratio), Qt::AlignCenter,
                       QString::number(_curValue));
        }
    }

    // 画滑块
    p.setBrush(brush);//设置画刷形式
    p.drawEllipse(_curXPos-3*_ratio,y-3*_ratio,6*_ratio,6*_ratio);
}
