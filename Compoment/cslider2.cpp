#include "cslider2.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPolygonF>
#include <QtDebug>

#define SLIDER_WIDTH 284
#define SLIDER_HEIGHT 8

CSlider2::CSlider2(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    b_showAllLine = false;
    setRange(0, 100);
    setPageStep(1.0);
}

void CSlider2::initSize(const int &w, const int &h)
{
    _originalWidth = w;
    _originalHeight = h;

    updateUI(1.0);
}

void CSlider2::initSize(const QSize &size)
{
    initSize(size.width(), size.height());
}

void CSlider2::setPageStep(const qreal &step)
{
    _step = step;
    if(step == 0.0)
        _step = _rangeEnded / 100.0;
}

void CSlider2::setRange(const int &rangeA, const int &rangeB)
{
    if(rangeA == rangeB)
        return;
    _rangeStart = qMin(rangeA, rangeB);
    _rangeEnded = qMax(rangeA, rangeB);
}

void CSlider2::setValue(const double &value)
{
    _curValue = value;
    emit valueChanged(value);
    _curXPos = valueToXPos(value);
    this->update();
}

int CSlider2::valueToXPos(const double &value)
{
    double tmpValue = value;
    if(value < _rangeStart)
        tmpValue = _rangeStart;
    else if(value > _rangeEnded)
        tmpValue = _rangeEnded;

    const int tmpX = availableXPos(_minXPos + (_maxXPos-_minXPos) * tmpValue/(_rangeEnded-_rangeStart));

    return tmpX;
}

double CSlider2::XPosToValue(const int &xPos)
{
    const int tmpPos = availableXPos(xPos);
    double tmpValue = _rangeStart + ((double)(tmpPos-_minXPos))/(_maxXPos-_minXPos)*(_rangeEnded-_rangeStart);
    return tmpValue;
}

void CSlider2::showAllLine()
{
    b_showAllLine = true;
}

void CSlider2::updateUI(const qreal &scaleRatio)
{
    _ratio = scaleRatio;

    const int w = _originalWidth  * scaleRatio;
    const int h = _originalHeight * scaleRatio;

    _minXPos = (w - SLIDER_WIDTH * scaleRatio) / 2 + SLIDER_HEIGHT * scaleRatio;
    _maxXPos = w - _minXPos;
    _curXPos = valueToXPos(_curValue);

    this->setFixedSize(w, h);
}

int CSlider2::availableXPos(const int &value)
{
    if(value < _minXPos) return _minXPos;
    if(value > _maxXPos) return _maxXPos;
    return value;
}

void CSlider2::mousePressEvent(QMouseEvent *e)
{
    const int tmpXPos = availableXPos(e->pos().x());
    if(tmpXPos != _curXPos)
    {
        qreal value = XPosToValue(tmpXPos);
        const int tmp = int(value / _step);
        if(tmp * _step != value && value - tmp * _step >= _step/2) {
            value = (tmp + 1) * _step;
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

void CSlider2::mouseMoveEvent(QMouseEvent *e)
{
    const int tmpXPos = availableXPos(e->pos().x());
    if(tmpXPos != _curXPos)
    {
        qreal value = XPosToValue(tmpXPos);
        const int tmp = int(value / _step);
        if(tmp * _step != value && value - tmp * _step >= _step/2) {
            value = (tmp + 1) * _step;
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

void CSlider2::paintEvent(QPaintEvent *)
{
    const QRect r(_minXPos - SLIDER_HEIGHT * _ratio,
                  (this->height() - SLIDER_HEIGHT * _ratio) / 2,
                  SLIDER_WIDTH * _ratio,
                  SLIDER_HEIGHT * _ratio);

    //设置画笔颜色
    QPen pen;
    pen.setColor(QColor(132,132,134));
    pen.setWidth(1 * _ratio);

    QPainter p(this);
    p.setPen(pen);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(this->rect(), QColor(32,32,33));

    //填充圆角方框
    QPainterPath roundPath;
    roundPath.addRoundedRect(r, SLIDER_HEIGHT / 2 * _ratio, SLIDER_HEIGHT / 2 * _ratio);
    p.fillPath(roundPath, QColor(48,48,48));

    p.drawRoundedRect(r, SLIDER_HEIGHT / 2 * _ratio, SLIDER_HEIGHT / 2 * _ratio);

    //画3短线
    const int y = r.y() + r.height() + 8 * _ratio;
    p.drawLine(_minXPos, y, _minXPos, y + 6 * _ratio);
    p.drawLine(_maxXPos, y, _maxXPos, y + 6 * _ratio);
    p.drawLine((_minXPos + _maxXPos) / 2, y,
               (_minXPos + _maxXPos) / 2, y+6 * _ratio);

    if(b_showAllLine) {
        const int count = (_rangeEnded - _rangeStart)/_step + 1;
        for(int i=1; i<count/2;++i) {
            const int dx = valueToXPos((_rangeStart + _rangeEnded)/2.0 + _step * i);
            p.drawLine(dx, y, dx, y + 6 * _ratio);
        }
        for(int i=-1; i>-count/2;--i) {
            const int dx = valueToXPos((_rangeStart + _rangeEnded)/2.0 + _step * i);
            p.drawLine(dx, y, dx, y + 6 * _ratio);
        }
    }

    //滑块的5顶点
    const QPoint p1(_curXPos - 4 * _ratio, r.y() - 4 * _ratio);
    const QPoint p2(_curXPos + 4 * _ratio, r.y() - 4 * _ratio);
    const QPoint p3(_curXPos + 8 * _ratio, r.y() + r.height() / 2);
    const QPoint p4(_curXPos + 0,          r.y() + r.height() + 4 * _ratio);
    const QPoint p5(_curXPos - 8 * _ratio, r.y() + r.height() / 2);

    QPolygonF polygon;
    polygon << p1 << p2 << p3 << p4 << p5;

    //填充滑块
    QPainterPath polygonPath;
    polygonPath.addPolygon(polygon);
    p.fillPath(polygonPath, QColor(48,48,48));

    //画滑块（多边形）
    p.drawPolygon(polygon);
}
