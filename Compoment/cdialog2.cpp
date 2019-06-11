#include "cdialog2.h"
#include <globalarg.h>
#include <QLabel>
#include "svgbutton.h"
#include "updateinterface.h"
#include <QtDebug>
#include <QPropertyAnimation>
#include <QEvent>
#include <QMouseEvent>
#include "playermainwidget.h"

#define SPACE_SIZE 8
#define LOGO_WIDTH 80

CDialog2::CDialog2(const QString &str, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    b_pressed = false;
    _space = SPACE_SIZE;
    _contentW   = NULL;
    _titleLabel = NULL;

    //标题
    if(!str.isEmpty())
    {
        QFont font;
        font.setFamily(Global->getFontFamily());
        font.setPixelSize(14);
        font.setBold(true);

        _titleLabel = new QLabel(this);
        _titleLabel->setText(str);
        _titleLabel->setAlignment(Qt::AlignCenter);
        _titleLabel->setAttribute(Qt::WA_DeleteOnClose);
        _titleLabel->setFrameShape(QFrame::NoFrame);
        _titleLabel->setFrameShadow(QFrame::Plain);
        _titleLabel->setFont(font);
        _titleLabel->setStyleSheet("background:rgb(27,27,28); color: white;");
    }

    //关闭按钮
    _closeBtn = new SvgButton(this);
    _closeBtn->initSize(26*1.5, 22*1.5);
    _closeBtn->setImagePrefixName("close");

    //开始动画
    _showAnimal = new QPropertyAnimation(this, "geometry", this);
    _showAnimal->setDuration(400);

    //结束动画
    _closeAnimal = new QPropertyAnimation(this, "geometry", this);
    _closeAnimal->setDuration(400);

    connect(_closeBtn,    SIGNAL(clicked()),  SLOT(reject2()));
    connect(_closeAnimal, SIGNAL(finished()), SLOT(closeFinished()));

    this->setStyleSheet("background: rgb(32,32,33)");
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setWindowState(Qt::WindowActive);

    _ratio = 1.0;
    updateUI(Global->_screenRatio);
    this->installEventFilter(this);
}

void CDialog2::setContentWidget(QWidget *w)
{
    if(w)
    {
        w->setParent(this);
        _contentW = w;
        _closeBtn->raise();
    }
}

void CDialog2::updateUI(const qreal &scaleRatio)
{
    if(scaleRatio != _ratio)
    {

        if(_titleLabel) {
            QFont font;
            font.setFamily(Global->getFontFamily());
            font.setPixelSize(14 * scaleRatio);
            font.setBold(true);
            _titleLabel->setFont(font);
        }

        _space = SPACE_SIZE * scaleRatio;
        _closeBtn->updateUI(scaleRatio);

        if(_contentW)
        {
            UpdateInterfaceWidget * contentW = qobject_cast<UpdateInterfaceWidget *>(_contentW);
            if(contentW)
            {
                contentW->updateUI(scaleRatio);
            }
        }
    }

    const int w = PMW->width();
    int h = PMW->height()-PMW->headHeight();
    if(h < 328*scaleRatio)
        h = 328*scaleRatio;

    this->initGeometry(PMW->x(), PMW->y() + PMW->headHeight(), w, h);

    _ratio = scaleRatio;
}

void CDialog2::initGeometry(const int &x, const int &y, const int &w, const int &h)
{
    int headH = 0;

    if(_titleLabel) {
        headH = _closeBtn->height();
        _titleLabel->setGeometry(0, 0, w, headH);
    }

    _closeBtn->move(w - _closeBtn->width(), 0);

    int height = h;
    if(height < this->minimumHeight())
        height = this->minimumHeight();

    if(_contentW)
    {
        _contentW->setGeometry(0, headH, w, height - headH);
    }

    this->setGeometry(x, y, w, height);
}

void CDialog2::accept()
{
    _state = 0;
    startCloseAnimal();
}

void CDialog2::reject()
{
    _state = 1;
    QDialog::reject();
}

void CDialog2::reject2()
{
    _state = 1;
    startCloseAnimal();
}

void CDialog2::closeFinished()
{
    if(_state == 0)
        QDialog::accept();
    else
        QDialog::reject();
}

void CDialog2::startCloseAnimal()
{
    //this->setDisabled(true);
    _closeAnimal->setStartValue(this->geometry());
    _closeAnimal->setEndValue(QRect(this->x(), this->y(), 0, this->height()));
    _closeAnimal->start();
}

bool CDialog2::eventFilter(QObject *obj, QEvent *e)
{
    switch(e->type())
    {
    case QEvent::MouseButtonPress:
    {
        if(!PMW->showMax()) {
            b_pressed = true;
            PMW->dialogPressedMove(this->mapToGlobal(static_cast<QMouseEvent*>(e)->pos()));
        }
        return true;
    }
    case QEvent::MouseMove:
    {
        if(b_pressed)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);

            PMW->dialogMoved(this->mapToGlobal(mouseEvent->pos()));

            updateUI(Global->_screenRatio);
        }
        return true;
    }
    case QEvent::MouseButtonRelease:
    {
        b_pressed = false;
        PMW->dialogReleaseMove();
        return true;
    }
    case QEvent::Show:
    {
        _showAnimal->setStartValue(QRect(this->x(), this->y(), 0, this->height()));
        _showAnimal->setEndValue(this->geometry());
        _showAnimal->start();
        return true;
    }
    case QEvent::KeyPress:
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
        if(keyEvent->key() == Qt::Key_Escape)
            reject2();
        return true;
    }
    default: break;
    }

    return QDialog::eventFilter(obj, e);
}
