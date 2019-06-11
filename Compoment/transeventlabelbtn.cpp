#include "transeventlabelbtn.h"
#include <QMouseEvent>

#include <Windows.h>
#include <winuser.h>
TransEventLabelBtn::TransEventLabelBtn(QWidget *parent, const QString &style) : QLabel(parent)
{
    this->setMouseTracking(true);
    this->setStyleSheet(style);
    _mousePress = false;
    _mouseHand  = false;
    _enterStyle = style;
    _leaveStyle = style;
}

void TransEventLabelBtn::setnorStyle(const QString &style)
{
    _leaveStyle = style;
}

void TransEventLabelBtn::setEnterStyle(const QString &style)
{
    _enterStyle = style;
}

void TransEventLabelBtn::setmouseHand(const bool &hand)
{
    _mouseHand = hand;
}

void TransEventLabelBtn::mousePressEvent(QMouseEvent *e)
{
    e->accept();
    _mousePress = true;
}

void TransEventLabelBtn::mouseMoveEvent(QMouseEvent *e)
{
    e->accept();
    if(this->rect().contains(e->pos())) {
        if(_mouseHand) {
            ::SetCursor(LoadCursor(NULL, IDC_HAND));
        }
        _mousePress = true;
    } else {
        _mousePress = false;
    }
}

void TransEventLabelBtn::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    if(_mousePress)
        emit clicked(true);
}

void TransEventLabelBtn::enterEvent(QEvent *)
{
    if(_mousePress && this->isEnabled())
        this->setStyleSheet(_enterStyle);
    if(_mouseHand) {
        ::SetCursor(LoadCursor(NULL, IDC_HAND));
    }
}

void TransEventLabelBtn::leaveEvent(QEvent *)
{
    if(this->isEnabled())
        this->setStyleSheet(_leaveStyle);
    ::SetCursor(LoadCursor(NULL, IDC_ARROW));
}
