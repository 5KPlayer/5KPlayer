#include "transeventicoclk.h"
#include <QMouseEvent>
#ifdef Q_OS_WIN
#include <Windows.h>
#include <winuser.h>
#endif
TransEventIcoClk::TransEventIcoClk(QWidget *parent, QString style)
    : QSvgWidget(parent)
{
    this->setMouseTracking(true);
    this->setStyleSheet(style);
    _mousePress = false;
    _mouseHand  = false;
    _enterStyle = style;
    _leaveStyle = style;
}

void TransEventIcoClk::setnorStyle(const QString &style)
{
    _leaveStyle = style;
}

void TransEventIcoClk::setEnterStyle(const QString &style)
{
    _enterStyle = style;
}

void TransEventIcoClk::setmouseHand(const bool &hand)
{
    _mouseHand = hand;
}

void TransEventIcoClk::mousePressEvent(QMouseEvent *e)
{
    e->accept();
    _mousePress = true;
}

void TransEventIcoClk::mouseMoveEvent(QMouseEvent *e)
{
    e->accept();
    if(this->rect().contains(e->pos())) {
        if(_mouseHand) {
            ::SetCursor(LoadCursor(NULL, IDC_HAND));
            //setCursor(Qt::PointingHandCursor);
        }
        _mousePress = true;
    } else {
        _mousePress = false;
    }
}

void TransEventIcoClk::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    if(_mousePress)
        emit clicked();
}

void TransEventIcoClk::enterEvent(QEvent *)
{
    if(_mousePress && this->isEnabled())
        this->setStyleSheet(_enterStyle);
    if(_mouseHand) {
        //setCursor(Qt::PointingHandCursor);
        ::SetCursor(LoadCursor(NULL, IDC_HAND));
    }
}

void TransEventIcoClk::leaveEvent(QEvent *)
{
    if(this->isEnabled())
        this->setStyleSheet(_leaveStyle);
    ::SetCursor(LoadCursor(NULL, IDC_ARROW));
    //setCursor(Qt::ArrowCursor);
}

