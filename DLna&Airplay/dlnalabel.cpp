#include "dlnalabel.h"
#include <QPainter>
#include <QMouseEvent>
#include "globalarg.h"
#include "systemfunc.h"

DLnaLabel::DLnaLabel(QWidget *parent, bool state)
    : QLabel(parent)
{
    _scaleRatio = 1.0;
    _clkstate   = state;
    this->setText("DLNA");
    this->setAlignment(Qt::AlignCenter);
    this->setStyleSheet(norState);
}

void DLnaLabel::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void DLnaLabel::setLabelAct(bool actstate)
{
    _clkstate = actstate;
    if(_clkstate) {
        this->setStyleSheet(actState);
    } else {
        this->setStyleSheet(norState);
    }
}

bool DLnaLabel::getLabelAct()
{
    return _clkstate;
}

void DLnaLabel::upUI()
{
    QFont font;
    font.setFamily("Tahoma");
    font.setPixelSize(8*_scaleRatio);
    font.setLetterSpacing(QFont::AbsoluteSpacing,0.4);
    this->setFont(font);

}

void DLnaLabel::mousePressEvent(QMouseEvent *ev)
{
    ev->accept();
}

void DLnaLabel::mouseMoveEvent(QMouseEvent *e)
{
    e->accept();
    my_setCursor(CursorType::Hand);
}

void DLnaLabel::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    if(this->rect().contains(e->pos())) {
        _clkstate = !_clkstate;
        if(_clkstate) {
            this->setStyleSheet(actState);
        } else {
            this->setStyleSheet(norState);
        }
        emit clicked();
    }
}

void DLnaLabel::enterEvent(QEvent *)
{
    my_setCursor(CursorType::Hand);
}

void DLnaLabel::leaveEvent(QEvent *)
{
    my_setCursor(CursorType::Arrow);
}

void DLnaLabel::resizeEvent(QResizeEvent *)
{
    upUI();
}
