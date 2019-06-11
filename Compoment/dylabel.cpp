#include "dylabel.h"
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>

#include <Windows.h>
#include <winuser.h>
#include "globalarg.h"
DYLabel::DYLabel(QWidget *parent, bool state)
    : QLabel(parent)
{
    this->setMouseTracking(true);
    _mouseHead  = false;
    _mouseEnter = false;
    _btnEnable  = false;
    _scaleRatio = Global->_screenRatio;
    _styleState = state;

    _pressColor = "rgb(51, 51, 56)";
    _enterColor = "rgb(70, 70, 72)";
    _leaveColor = "rgb(94, 94, 98)";
    _btnStyle   = QString("border-radius: %1px; background-color: %2;color:rgb(255,255,255)").arg(4*_scaleRatio);
    if(_styleState) {
        this->setStyleSheet(_btnStyle.arg(_leaveColor));
    } else {
        this->setStyleSheet("background:transparent");
    }
}

void DYLabel::setMouseHead(bool state)
{
    _mouseHead = state;
}

void DYLabel::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _btnStyle   = QString("border-radius: %1px; background-color: %2;color:rgb(255,255,255)").arg(4*_scaleRatio);
    if(_mouseEnter) {
        this->setStyleSheet(_btnStyle.arg(_enterColor));
    } else {
        this->setStyleSheet(_btnStyle.arg(_leaveColor));
    }
}

void DYLabel::Enable(bool enable)
{
    _btnEnable = enable;
    if(enable) {
        this->setStyleSheet(_btnStyle.arg(_enterColor));
    } else {
        this->setStyleSheet(_btnStyle.arg(_leaveColor));
    }
}

void DYLabel::setBackClolr(QString norColor, QString entColor, QString pressColor)
{
    _enterColor = entColor;
    _leaveColor = norColor;
    _pressColor = pressColor;
    this->setStyleSheet(_btnStyle.arg(_leaveColor));
}

void DYLabel::mousePressEvent(QMouseEvent *ev)
{
    if(_styleState)
        this->setStyleSheet(_btnStyle.arg(_pressColor));
    ev->accept();
}

void DYLabel::mouseMoveEvent(QMouseEvent *e)
{
    if(!this->rect().contains(e->pos()) && _styleState) {
        this->setStyleSheet(_btnStyle.arg(_leaveColor));
    }
    e->accept();
    if(_mouseHead)
        ::SetCursor(LoadCursor(NULL, IDC_HAND));
}

void DYLabel::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    if(_styleState)
        this->setStyleSheet(_btnStyle.arg(_leaveColor));
    if(this->rect().contains(e->pos()) && !_btnEnable)
        emit clicked();
}

void DYLabel::enterEvent(QEvent *)
{
    _mouseEnter = true;
    if(this->isEnabled() && _styleState) {
        this->setStyleSheet(_btnStyle.arg(_enterColor));
    }
    if(_mouseHead)
        ::SetCursor(LoadCursor(NULL, IDC_HAND));
}

void DYLabel::leaveEvent(QEvent *)
{
    _mouseEnter = false;
    if(_styleState) {
        this->setStyleSheet(_btnStyle.arg(_leaveColor));
    }
    if(_mouseHead)
        ::SetCursor(LoadCursor(NULL, IDC_ARROW));
}
