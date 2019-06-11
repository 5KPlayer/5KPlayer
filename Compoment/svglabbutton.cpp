#include "svglabbutton.h"
#include <QSvgWidget>
#include <QLabel>
#include <QMouseEvent>
#include "globalarg.h"

SvgLabButton::SvgLabButton(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    _svgImage = new QSvgWidget(this);
    _textLabel = new QLabel(this);
    _textLabel->setAlignment(Qt::AlignCenter);

    _mouseEnter = false;
    _firstInfo  = true;
    _scaleRatio = Global->_screenRatio;

    _pressColor = "rgb(51, 51, 56)";
    _enterColor = "rgb(70, 70, 72)";
    _leaveColor = "rgb(94, 94, 98)";
    _btnStyle   = QString("border-radius: %1px; background-color: %2;color:rgb(255,255,255)").arg(4*_scaleRatio);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet(_btnStyle.arg(_leaveColor));
}

void SvgLabButton::setInfo(const QString &svgFile, const QString &text)
{
    _svgNorImage    = ":/res/svg/" + svgFile +  "_nor.svg";
    _svghoverImage  = ":/res/svg/" + svgFile +  "_hover.svg";
    _svgdownImage   = ":/res/svg/" + svgFile +  "_down.svg";
    _textLabel->setText(text);
    if(_firstInfo) {
        _firstInfo = false;
        upSvgState(Svg_Nor);
    } else {
        upSvgState(Svg_Hover);
    }
    upUI();
}

void SvgLabButton::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void SvgLabButton::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * _scaleRatio);
    _textLabel->setFont(font);

    int svgX = this->width()/3*2;
    int svgH = this->height()/4*3;
    int svgY = this->height()/8;
    _svgImage->setGeometry(svgX+(svgX/2-svgH)/2,svgY,svgH,svgH);
    int labX = this->width() - _svgImage->x() - svgH;
    _textLabel->setGeometry(labX,0,this->width() - svgH - labX*2,this->height());
}

void SvgLabButton::upSvgState(SvgLabButton::SvgState state)
{
    QString imagePath;
    switch (state) {
    case Svg_Nor:
        imagePath = _svgNorImage;
        break;
    case Svg_Hover:
        imagePath = _svghoverImage;
        break;
    case Svg_Down:
        imagePath = _svgdownImage;
        break;
    default:
        break;
    }
    _svgImage->load(imagePath);
}

void SvgLabButton::resizeEvent(QResizeEvent *)
{
    upUI();
}

void SvgLabButton::enterEvent(QEvent *)
{
    _mouseEnter = true;
    if(this->isEnabled()) {
        this->setStyleSheet(_btnStyle.arg(_enterColor));
        upSvgState(Svg_Hover);
    }
}

void SvgLabButton::leaveEvent(QEvent *)
{
    _mouseEnter = false;
    this->setStyleSheet(_btnStyle.arg(_leaveColor));
    upSvgState(Svg_Nor);
}

void SvgLabButton::mousePressEvent(QMouseEvent *ev)
{
    this->setStyleSheet(_btnStyle.arg(_pressColor));
    upSvgState(Svg_Down);
    ev->accept();
}

void SvgLabButton::mouseMoveEvent(QMouseEvent *e)
{
    if(!this->rect().contains(e->pos())) {
        this->setStyleSheet(_btnStyle.arg(_enterColor));
        upSvgState(Svg_Hover);
    }
    e->accept();
}

void SvgLabButton::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    if(this->rect().contains(e->pos())) {
        this->setStyleSheet(_btnStyle.arg(_enterColor));
        upSvgState(Svg_Hover);
        emit clicked();
    }
}
