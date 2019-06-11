#include "dypixlabel.h"
#include "language.h"
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include "globalarg.h"
#include "systemfunc.h"
#include <QLabel>
#include <QDebug>

DYPixLabel::DYPixLabel(QString res, QString text, QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    _pixRes             = res;
    _text               = text;
    _mousePress         = false;
    _isMouseHead        = false;
    _fontUnderrline     = false;
}

void DYPixLabel::initSize(int w, int h)
{
    _originalWidth = w;
    _originalHeight = h;

    updateUI(1.0);
}

bool DYPixLabel::setFontUnderline(bool overLine)
{
    _fontUnderrline = overLine;
}

void DYPixLabel::setMouseHead(bool head)
{
    _isMouseHead = head;
}

void DYPixLabel::updateLanguage()
{
    this->update();
}

void DYPixLabel::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    this->resize(_originalWidth*_scaleRatio,_originalHeight*_scaleRatio);
    this->update();
}

void DYPixLabel::mousePressEvent(QMouseEvent *ev)
{
    _mousePress = true;
    this->update();
}

void DYPixLabel::mouseReleaseEvent(QMouseEvent *e)
{
    if(this->rect().contains(e->pos())) {
        emit clicked();
    }
    _mousePress = false;
    this->update();
}

void DYPixLabel::enterEvent(QEvent *)
{
    if(_isMouseHead) {
        my_setCursor(CursorType::Hand);
    }
}

void DYPixLabel::leaveEvent(QEvent *)
{
    my_setCursor(CursorType::Arrow);
}

void DYPixLabel::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QFont font;
    font.setBold(true);
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    if(_fontUnderrline) {
        font.setUnderline(true);
    }
    p.setFont(font);

    int h = this->height();
    QPixmap map;
    map.load(_pixRes);

    p.drawPixmap(QRect(1*_scaleRatio,4*_scaleRatio,12*_scaleRatio,12*_scaleRatio),
                 map.scaled(12*_scaleRatio,12*_scaleRatio, Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    p.setPen(QColor(121,121,125));
    p.drawText(QRect(h,2*_scaleRatio,this->width() - h,h-4*_scaleRatio),
               _text);
}
