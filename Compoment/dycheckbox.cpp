#include "dycheckbox.h"
#include "globalarg.h"

#include <QPainter>
#include <QtDebug>
DYCheckBox::DYCheckBox(QWidget *parent, QString text)
    : UpdateInterfaceWidget(parent)
{
    b_mouseIn   = false;
    _checkDown  = false;
    _enable     = true;
    b_rightText = false;
    _fontBlod   = false;
    _checkText  = text;
    _textColor  = "";
    _scaleRatio = Global->_screenRatio;
}

void DYCheckBox::updateLanguage()
{
    this->update();
}

void DYCheckBox::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    this->update();
}

void DYCheckBox::setChecked(const bool &check)
{
    _checkDown = check;
    this->update();
}

void DYCheckBox::setCheckEnable(const bool &enable)
{
    this->setEnabled(enable);
    _enable = enable;
    this->update();
}

void DYCheckBox::setRightText(bool right)
{
    b_rightText = right;
}

void DYCheckBox::setTextFontBlod(bool bold)
{
    _fontBlod = bold;
}

void DYCheckBox::setTextColor(QString color)
{
    _textColor = color;
    this->repaint();
}

void DYCheckBox::mousePressEvent(QMouseEvent *)
{
    _checkDown = !_checkDown;
    emit checkChange(_checkDown);
    this->update();
}

void DYCheckBox::enterEvent(QEvent *)
{
    b_mouseIn = true;
    this->update();
}

void DYCheckBox::leaveEvent(QEvent *)
{
    b_mouseIn = false;
    this->update();
}

void DYCheckBox::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QFont font;
    font.setBold(true);
    font.setFamily("Tahoma");
    font.setPixelSize(12*_scaleRatio);
    p.setFont(font);


    const int radius = 2 * _scaleRatio;
    if(_enable) {
        p.setBrush(QBrush(Qt::white));
        if(b_mouseIn) { //背景色变深
            p.setPen(QColor(4,34,114));
            p.drawRoundedRect(0,(this->height()-16*_scaleRatio)/2,16*_scaleRatio,16*_scaleRatio, radius, radius);
            //p.fillRect(QRect(0,(this->height()-16*_scaleRatio)/2,16*_scaleRatio,16*_scaleRatio),QColor(212,234,252));
        } else {
            p.setPen(QColor(75,96,151));
            p.drawRoundedRect(0,(this->height()-16*_scaleRatio)/2,16*_scaleRatio,16*_scaleRatio, radius, radius);
            //p.fillRect(QRect(0,(this->height()-16*_scaleRatio)/2,16*_scaleRatio,16*_scaleRatio),QColor(255,255,255));
        }
    } else {
        p.setBrush(QBrush(QColor(69, 69, 70)));
        p.setPen(QColor(69,69,70));
        p.drawRoundedRect(0,(this->height()-16*_scaleRatio)/2,16*_scaleRatio,16*_scaleRatio, radius, radius);
        //p.fillRect(QRect(0,(this->height()-16*_scaleRatio)/2,16*_scaleRatio,16*_scaleRatio),QColor(200,200,200));
    }

    if(_checkDown) { // 画勾
        p.setPen(QColor(75,96,151));
        QPixmap map;
        if(_enable) {
            map.load(":/res/png/check");
        } else
            map.load(":/res/png/checkE");
        p.drawPixmap(QRect(0,(this->height()-16*_scaleRatio)/2,16*_scaleRatio,16*_scaleRatio),
                     map.scaled(16*_scaleRatio,16*_scaleRatio, Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }

    font.setFamily(Global->getFontFamily());
    font.setBold(_fontBlod);
    p.setFont(font);

    if(!_checkText.isEmpty()) {
        if(_enable) {
            if(_textColor.isEmpty())
                p.setPen(QColor(255,255,255));
            else
                p.setPen(QColor(_textColor));
        } else {
            p.setPen(QColor(69,69,70));
        }
        if(b_rightText)
            p.drawText(QRect(18*_scaleRatio,0,this->width()-18*_scaleRatio,this->height()), Qt::AlignRight | Qt::AlignVCenter, _checkText);
        else
            p.drawText(QRect(18*_scaleRatio,0,this->width()-18*_scaleRatio,this->height()), Qt::AlignLeft | Qt::AlignVCenter, _checkText);
    }
}
