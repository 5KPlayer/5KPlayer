#include "imagebutton.h"
#include <QEvent>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>
#include <QFileInfo>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QtDebug>

#include <Windows.h>
#include <winuser.h>

#define IMAGE_BUTTON ":/res/png/"

ImageButton::ImageButton(QWidget *parent, Qt::WindowFlags f) : QLabel(parent, f)
{
    _isSet = false;
    _leave = true;
    _ratio = 1.0;
    _isAnimalOn = false;
    _animation = Q_NULLPTR;
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setScaledContents(true);
    this->setFrameShape(QFrame::NoFrame);
    this->setFrameShadow(QFrame::Plain);
    this->setStyleSheet("background:transparent");
}

ImageButton::ImageButton(qreal ratio, QWidget *parent, Qt::WindowFlags f) : QLabel(parent, f)
{
    _leave = true;
    _ratio = ratio;
    _isAnimalOn = true;
    _animation = Q_NULLPTR;
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setScaledContents(true);
}

void ImageButton::setImagePrefixName(QString prefixName, QString subfix)
{
    if(prefixName.isEmpty())
    {
        qWarning() << "不能为空" << __PRETTY_FUNCTION__;
        return;
    }

    if(!subfix.startsWith('.'))
        subfix.insert(0, '.');

    setPressImage(IMAGE_BUTTON + prefixName + "_down" + subfix);
    setEnterImage(IMAGE_BUTTON + prefixName + "_on" + subfix);
    setNormalImage(IMAGE_BUTTON + prefixName + "_nor" + subfix);
    setDisableImage(IMAGE_BUTTON + prefixName + "_dis" + subfix);


    if(!_isSet)
    {
        _isSet = true;

        const QSize &size = QPixmap(_normalImage).size();
        _originalWidth = size.width();
        _originalHeight = size.height();

        if(_isAnimalOn)
        {
            this->setMinimumSize(_originalWidth, _originalHeight);
            this->setMaximumSize(_originalWidth * _ratio, _originalHeight * _ratio);
            this->resize(_originalWidth, _originalHeight);
        }
        else
            this->setFixedSize(_originalWidth, _originalHeight);
    }

    if(_leave)
        setCurShowedImage(_normalImage);
    else
        setCurShowedImage(_enterImage);
}

void ImageButton::initSize(const int &w, const int &h)
{
    _isSet = true;
    _originalWidth = w;
    _originalHeight = h;

    if(_isAnimalOn)
    {
        this->setMinimumSize(w, h);
        this->setMaximumSize(w * _ratio, h * _ratio);
        this->resize(w, h);
    }
    else
        this->setFixedSize(w, h);
}

void ImageButton::setshowDis()
{
    _normalImage = _disableImage;
    _enterImage = _disableImage;
    if(_leave)
        setCurShowedImage(_normalImage);
    else
        setCurShowedImage(_enterImage);
}

void ImageButton::updateUI(const qreal &scaleRatio)
{
    const int w = _originalWidth * scaleRatio;
    const int h = _originalHeight * scaleRatio;
    if(_isAnimalOn)
    {
        this->setMinimumSize(w, h);
        this->setMaximumSize(w * _ratio, h * _ratio);
        this->resize(w, h);
    }
    else
        this->setFixedSize(w, h);
}

void ImageButton::leave()
{
    if(_isAnimalOn)
    {
        if(_animation)
        {
            _animation->stop();
            this->setGeometry(_animation->startValue().toRect());
        }
    }

    if(!_leave) {
        _leave = true;
        if(isEnabled())
            setCurShowedImage(_normalImage);
    }
}


void ImageButton::setCurShowedImage(const QString &img)
{
    if(!QFileInfo::exists(img))
    {
        qWarning() << img << "不存在" << __PRETTY_FUNCTION__;
        return;
    }
    this->setPixmap(QPixmap(img));
}

void ImageButton::mousePressEvent(QMouseEvent *e)
{
    _leave = false;
    if(_isAnimalOn)
    {
        if(_animation)
        {
            _animation->stop();
            this->setGeometry(_animation->startValue().toRect());
        }
    }
    setCurShowedImage(_pressImage);
    e->accept();
}

void ImageButton::mouseMoveEvent(QMouseEvent *e)
{
    if(!_leave)
    {
        const QPoint p = e->pos();
        if(p.x() < 0 || p.x() > width() || p.y() < 0 || p.y() > height())
        {
            leave();
        }
    }
    e->accept();
}

void ImageButton::mouseReleaseEvent(QMouseEvent *e) //先释放 后leave
{
    if(!_leave)
    {
        setCurShowedImage(_enterImage);
        emit clicked();
    }
    e->accept();
}

void ImageButton::enterEvent(QEvent *e)
{
    ::SetCursor(LoadCursor(NULL, IDC_ARROW));

    if(_leave && this->isEnabled())
    {
        _leave = false;
        setCurShowedImage(_enterImage);

        if(_isAnimalOn)
        {
            this->raise();
            if(_animation == Q_NULLPTR)
            {
                _animation = new QPropertyAnimation(this);
                _animation->setTargetObject(this);
                _animation->setPropertyName("geometry");
                _animation->setDuration(100);
            }
            _animation->setStartValue(this->geometry());
            _animation->setEndValue(QRect(geometry().x() - (_ratio-1.0)/2 * geometry().width(),
                                          geometry().y() - (_ratio-1.0)/2 * geometry().height() ,
                                          geometry().width()  * _ratio,
                                          geometry().height() * _ratio));
            _animation->start();
        }
    }
    e->accept();
}

void ImageButton::leaveEvent(QEvent *e)
{
    leave();
    e->accept();
}

void ImageButton::changeEvent(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::EnabledChange:
    {
        if(!this->isEnabled())
            setCurShowedImage(_disableImage);
        else{
            setCurShowedImage(_normalImage);
        }
        e->accept();
        return;
        break;
    }
    default: break;
    }
    QLabel::changeEvent(e);
}

