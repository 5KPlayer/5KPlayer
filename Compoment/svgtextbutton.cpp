#include "svgtextbutton.h"
#include <QSvgWidget>
#include <QLabel>
#include "globalarg.h"

SvgTextButton::SvgTextButton(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    b_pressed = false;
    _svgImage = new QSvgWidget(this);
    _textLabel = new QLabel(this);
    _textLabel->setAlignment(Qt::AlignCenter);
}

void SvgTextButton::setInfo(const QString &svgFile, const QString &text)
{
    _pressImage  = ":/res/svg/" + svgFile +  "_on.svg";
    _normalImage = ":/res/svg/" + svgFile + "_nor.svg";

    _textLabel->setText(text);

    showNormalState();
}

void SvgTextButton::initSize(const int &w, const int &h, const int &sw, const int &sh)
{
    _originalWidth = w;
    _originalHeight = h;
    _svgWidth = sw;
    _svgHeight = sh;

    updateUI(1.0);
}

void SvgTextButton::setPressed()
{
    b_pressed = true;
    showPressedState();
}

void SvgTextButton::setRaised()
{
    b_pressed = false;
    showNormalState();
}

void SvgTextButton::updateUI(const qreal &scaleRatio)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setBold(true);
    font.setPixelSize(12 * scaleRatio);

    _textLabel->setFont(font);
    _textLabel->resize(_originalWidth * scaleRatio, 20 * scaleRatio);

    _svgImage->resize(_svgWidth * scaleRatio, _svgHeight * scaleRatio);
    this->resize(_originalWidth * scaleRatio, _originalHeight * scaleRatio);
}

void SvgTextButton::resizeEvent(QResizeEvent *)
{
    const int ly = this->height()-_textLabel->height();
    _svgImage->move((this->width() - _svgImage->width()) /2,
                    (ly - _svgImage->height()) /2 + 4);
    _textLabel->move(0, ly);
}

void SvgTextButton::enterEvent(QEvent *)
{
    if(b_pressed) return;
    showPressedState();
}

void SvgTextButton::leaveEvent(QEvent *)
{
    if(b_pressed) return;
    showNormalState();
}

void SvgTextButton::mousePressEvent(QMouseEvent *)
{
    if(b_pressed) return;
    setPressed();
    emit clicked();
}

void SvgTextButton::showPressedState()
{
    //显示按下图片
    _svgImage->load(_pressImage);

    //改变文字颜色
    _textLabel->setStyleSheet("color: rgb(0,156,255);");
}

void SvgTextButton::showNormalState()
{
    //显示正常图片
    _svgImage->load(_normalImage);

    //改变文字颜色
    _textLabel->setStyleSheet("color: rgb(100,106,115);");
}
