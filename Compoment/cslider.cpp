#include "cslider.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QtDebug>
#include <QTimer>

CSlider::CSlider(Qt::Orientation orientation, QWidget *p) : QSlider(orientation, p)
{
    b_isCircle = true;
    p_timer = new QTimer(this);
    p_timer->setInterval(100);
    p_timer->setSingleShot(true);
    connect(p_timer, &QTimer::timeout, [=](){
        emit curValue(i_value);
    });
}

void CSlider::initSize(const int &w, const int &h)
{
    _originalWidth = w;
    _originalHeight = h;
    if(w != 0)
        this->setFixedSize(w, h);
    else
        this->setFixedHeight(h);
}

void CSlider::showRectHandle()
{
    b_isCircle = false;
}

void CSlider::updateUI(const qreal &scaleRatio)
{
    if(_originalWidth != 0)
        this->setFixedSize(_originalWidth * scaleRatio, _originalHeight * scaleRatio);
    else
        this->setFixedHeight(_originalHeight * scaleRatio);
}

void CSlider::updateState(bool b)//b is disabled
{
    QString addPageColor("rgb(39,39,40)");
    QString subPageColor("rgb(225,243,255)");
    QString handleColor("white");
    if(!b)
    {
        addPageColor = "rgb(39,39,40)";
        subPageColor = "rgb(39,39,40)";
        handleColor  = "gray";
    }
    QString styleSheetStr;
    if(b_isCircle) {
        styleSheetStr = "QSlider{background:transparent;}"
                        "QSlider::groove:horizontal{background:transparent;height:%1px;}" //槽
                "QSlider::add-page:horizontal{background-color: %4;height:%1px;border-radius:%3px;}" //未划过槽
                "QSlider::sub-page:horizontal{background-color: %5;height:%1px;border-radius:%3px;}" //已划过的槽
                "QSlider::handle:horizontal{background-color: %6;width:%2px; height:%2px;border-radius:%1px;margin:-%3 0px;}";

    } else {
        styleSheetStr = "QSlider{background:transparent;}"
                        "QSlider::groove:horizontal{background:transparent;height:%1px;}" //槽
                "QSlider::add-page:horizontal{background-color: %4;height:%1px;border-radius:%3px;}" //未划过槽
                "QSlider::sub-page:horizontal{background-color: %5;height:%1px;border-radius:%3px;}" //已划过的槽
                "QSlider::handle:horizontal{background-color: %6;width:%2px; height:%2px;margin:-%2 0px;}";

    }
    const int h = this->height();
    this->setStyleSheet(styleSheetStr
                        .arg(h/8)
                        .arg(h/4)
                        .arg(qRound((qreal)h/16))
                        .arg(addPageColor)
                        .arg(subPageColor)
                        .arg(handleColor));
}

void CSlider::openMouseTrack()
{
    this->setMouseTracking(true);
}

void CSlider::mousePressEvent(QMouseEvent *e)
{
    if(!this->isSliderDown())
    {
        double pos = e->pos().x() / (double)width();
        const int value = pos * (maximum() - minimum()) + minimum();
        emit pressedValue(value);
    }
    e->accept();
    QSlider::mousePressEvent(e);
}

void CSlider::mouseMoveEvent(QMouseEvent *e)
{
    if(this->hasMouseTracking())
    {
        p_timer->stop();
        double pos = e->pos().x() / (double)width();
        const int value = pos * (maximum() - minimum()) + minimum();
        i_value = value / 1000;
        p_timer->start();
    }
    e->accept();
    QSlider::mouseMoveEvent(e);
}

void CSlider::leaveEvent(QEvent *e)
{
    if(this->hasMouseTracking())
    {
        p_timer->stop();
        emit leave();
    }
    e->accept();
    QSlider::leaveEvent(e);
}

void CSlider::resizeEvent(QResizeEvent *e)
{
    updateState(this->isEnabled());

    e->accept();
}

void CSlider::wheelEvent(QWheelEvent *e)
{
    e->accept();
}
