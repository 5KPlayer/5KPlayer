#include "presvgbutton.h"

#include <QDebug>
#include <QFileInfo>
#include <QEvent>
#include <QMouseEvent>

#define SVG_PATH ":/res/svg/"

PreSvgButton::PreSvgButton(QWidget *parent)
    : QSvgWidget(parent)
{
    init();
    _ratio = 1.0;
    this->installEventFilter(this);
}

PreSvgButton::~PreSvgButton()
{

}

void PreSvgButton::setImagePrefixName(const QString &prefixName)
{
    if(prefixName.isEmpty())
    {
        qWarning() << "不能为空" << __PRETTY_FUNCTION__;
        return;
    }

    setEnterImage(SVG_PATH + prefixName + "_on.svg");
    setPressImage(SVG_PATH + prefixName + "_down.svg");
    setNormalImage(SVG_PATH + prefixName + "_nor.svg");
    setDisableImage(SVG_PATH + prefixName + "_dis.svg");

    setCurShowedImage(_normalImage);
}

void PreSvgButton::initSize(int w, int h)
{
    _originalWidth = w;
    _originalHeight = h;

    updateUI(1.0);
}

void PreSvgButton::updateUI(const qreal &scaleRatio)
{
    const int w = _originalWidth * scaleRatio;
    const int h = _originalHeight * scaleRatio;
    this->setFixedSize(w, h);
}

void PreSvgButton::init()
{
    _isPressed  = false;
    _isState    = false;
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setStyleSheet("background-color:white");
}

void PreSvgButton::setCurShowedImage(const QString &img)
{
    if(!QFileInfo::exists(img))
    {
        qWarning() << img << "不存在" << __PRETTY_FUNCTION__;
        return;
    }
    _curShowImg = img;
    this->load(img);
}

bool PreSvgButton::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseMove:
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint curPos = mouseEvent->pos();
        emit btnMove(curPos.x() - _mousePoint.x());
        break;
    }
    case QEvent::MouseButtonPress:
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::LeftButton &&
           this->rect().contains(mouseEvent->pos()))
        {
            _mousePoint = mouseEvent->pos();
            _isPressed  = true;
            setCurShowedImage(_pressImage);
        }
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        _isPressed = false;
        setCurShowedImage(_enterImage);
        emit btnRelease();
        break;
    }
    case QEvent::Enter:
    {
        setCurShowedImage(_enterImage);
        break;
    }
    case QEvent::Leave:
    {
        setCurShowedImage(_normalImage);
        break;
    }
    case QEvent::Show:
    {
        setCurShowedImage(_normalImage);
        break;
    }
    default:
    {

    }
    }
    return QSvgWidget::eventFilter(obj, event);
}
