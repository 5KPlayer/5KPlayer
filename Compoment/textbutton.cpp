#include "textbutton.h"
#include "globalarg.h"
#include <QMouseEvent>
#include <QtDebug>

#define RADIUS_SIZE 4
#define FONT_SIZE 14


TextButton::TextButton(const QString &text, const int &w, const int &h,
                       QWidget *parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{
    _isPressed = false;
    _originalWidth = w;
    _originalHeight = h;

    _curPosition = Left;

    _pressedStyleStr = "background:transparent; color: rgb(255,255,255); "
                       "background-color: rgb(104,104,107);";

    _releaseStyleStr = "background:transparent; color: rgb(255,255,255); "
                       "background-color: rgb(47,47,47);";

    _leftStyleStr  = "border-style: solid;border-top-left-radius: %1px;"
                     "border-bottom-left-radius: %1px;";

    _rightStyleStr = "border-style: solid;border-top-right-radius: %1px;"
                     "border-bottom-right-radius: %1px;";

    this->setAlignment(Qt::AlignCenter);

    updateUI(1.0);
}

void TextButton::updateUI(const qreal &scaleRatio)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(FONT_SIZE * scaleRatio);
    //font.setLetterSpacing(QFont::AbsoluteSpacing, 1);
    font.setWeight(QFont::Bold);// QFont::DemiBold
    font.setStyleStrategy(QFont::PreferDefault);
    font.setKerning(false);

    this->setFont(font);

    _radius = RADIUS_SIZE * scaleRatio;

    this->setFixedSize(_originalWidth * scaleRatio, _originalHeight * scaleRatio);
}

void TextButton::setPosition(const TextButton::Position &p)
{
    _curPosition = p;
}

void TextButton::click()
{
    if(!_isPressed)
    {
        _isPressed = true;

        QString str(_pressedStyleStr);
        switch (_curPosition) {
        case Left:
            str += _leftStyleStr.arg(_radius);
            break;
        case Center:
            break;
        case Right:
            str += _rightStyleStr.arg(_radius);
            break;
        }
        emit clicked();
        this->setStyleSheet(str);
    }
}

void TextButton::recover()
{
    _isPressed = false;

    QString str(_releaseStyleStr);
    switch (_curPosition) {
    case Left:
        str += _leftStyleStr.arg(_radius);
        break;
    case Center:
        break;
    case Right:
        str += _rightStyleStr.arg(_radius);
        break;
    }
    this->setStyleSheet(str);
}

void TextButton::mousePressEvent(QMouseEvent *e)
{
    e->accept();
}

void TextButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(this->rect().contains(e->pos()))
    {
        click();
    }
    e->accept();
}
