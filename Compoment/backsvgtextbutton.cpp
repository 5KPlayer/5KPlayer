#include "backsvgtextbutton.h"
#include "globalarg.h"
#include <QMouseEvent>
BackSvgTextButton::BackSvgTextButton(QWidget *parent, bool isRadiu)
    : QLabel(parent)
{
    _svg    = new TransEventIco(this);
    _text   = new TransEventLabel(this);

    _text->setAlignment(Qt::AlignCenter);
    _isRadius   = isRadiu;
    _btnState   = BUTTON_NOR;
    _scaleRatio = 1.0;
}

void BackSvgTextButton::updateLanguage()
{
    _text->setText(Lang(_textInfo));
    upUI();
}

void BackSvgTextButton::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void BackSvgTextButton::setButtonColor(QString norCol, QString onCol, QString downCol)
{
    _norColor   = norCol;
    _onColor    = onCol;
    _downColor  = downCol;
    upButtonState(_btnState);
}

void BackSvgTextButton::setButtonInfo(QString svg, QString text)
{
    _pressImage  = ":/res/svg/" + svg +  "_down.svg";
    _normalImage = ":/res/svg/" + svg + "_nor.svg";
    _textInfo    = text;
    _text->setText(Lang(text));
    upButtonState(_btnState);
}

void BackSvgTextButton::upUI()
{
    QFont font;
    font.setPixelSize(14*_scaleRatio);
    font.setBold(true);
    font.setFamily(Global->getFontFamily());
    _text->setFont(font);
    int textLenght = _text->fontMetrics().width(Lang(_textInfo));
    _text->resize(textLenght,this->height());
    _svg->resize(16*_scaleRatio,16*_scaleRatio);
    int svgX = (this->width() - _svg->width() - _text->width() - 4*_scaleRatio)/2;
    int svgY = (this->height() - _svg->height()) /2;
    _svg->move(svgX,svgY);
    _text->move(svgX+_svg->width() + 4*_scaleRatio,0);
}

void BackSvgTextButton::upButtonState(int state)
{
    QString backColor = "";
    QString svg = "";
    switch (state) {
    case BUTTON_NOR:
        backColor   = _norColor;
        svg         = _normalImage;
        break;
    case BUTTON_HOVER:
        backColor   = _onColor;
        svg         = _normalImage;
        break;
    case BUTTON_DOWN:
        backColor   = _downColor;
        svg         = _pressImage;
        break;
    default:
        backColor   = _norColor;
        svg         = _normalImage;
        break;
    }

    QString btnStyle;
    if(_isRadius)
        btnStyle = QString("border-radius: %1px; background-color: %2;color:rgb(222,222,222)").arg(4*_scaleRatio)
                .arg(backColor);
    else
        btnStyle = QString("background-color: %1;color:rgb(222,222,222)").arg(backColor);

    _svg->load(svg);
    this->setStyleSheet(btnStyle);
    this->repaint();
}

void BackSvgTextButton::resizeEvent(QResizeEvent *)
{
    upUI();
}

void BackSvgTextButton::mousePressEvent(QMouseEvent *e)
{
    _btnState = BUTTON_DOWN;
    upButtonState(_btnState);
}

void BackSvgTextButton::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    if(this->rect().contains(e->pos())) {
        emit clicked();
        _btnState = BUTTON_HOVER;
        upButtonState(_btnState);
    }
}

void BackSvgTextButton::enterEvent(QEvent *e)
{
    _btnState = BUTTON_HOVER;
    upButtonState(_btnState);
}

void BackSvgTextButton::leaveEvent(QEvent *e)
{
    _btnState = BUTTON_NOR;
    upButtonState(_btnState);
}
