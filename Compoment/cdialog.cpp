#include "cdialog.h"
#include <QLabel>
#include "svgbutton.h"
#include "updateinterface.h"
#include "globalarg.h"
#include <QtDebug>
#include <QMouseEvent>
#include <QPropertyAnimation>

#include <QScreen>
#include <QSignalMapper>
#include <QApplication>

#define SPACE_SIZE 8
#define LOGO_WIDTH 80

CDialog::CDialog(bool showLogo, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    _ratio = 1.0;
    _space = SPACE_SIZE;
    b_space = true;
    m_bMove = true;
    b_showHead = true;
    _pressed = false;
    _curScreen  = NULL;
    _contentW   = NULL;
    _logoLabel  = NULL;
    _titleLabel = NULL;

    _showAnimal = new QPropertyAnimation(this, "geometry", this);
    _showAnimal->setDuration(200);

    _closeAnimal = new QPropertyAnimation(this, "windowOpacity", this);
    _closeAnimal->setDuration(800);
    connect(_closeAnimal, SIGNAL(finished()), SLOT(reject()));

    _screenList = qApp->screens();
    QSignalMapper *signalMapper = new QSignalMapper(this);
    foreach (QScreen *screen, _screenList)
    {
        //connect(screen, SIGNAL(geometryChanged(QRect)), signalMapper, SLOT(map()));//分辨率
        connect(screen, SIGNAL(logicalDotsPerInchChanged(qreal)), signalMapper, SLOT(map()));//缩放
        signalMapper->setMapping(screen, screen);
    }
    connect(signalMapper, SIGNAL(mapped(QObject*)), SLOT(screenChanged(QObject*)));

    if(showLogo)
    {
        _logoLabel = new QLabel(this);
        _logoLabel->setAttribute(Qt::WA_DeleteOnClose);
        _logoLabel->setScaledContents(true);
        _logoLabel->setFrameShape(QFrame::NoFrame);
        _logoLabel->setFrameShadow(QFrame::Plain);
        _logoLabel->setStyleSheet("background:transparent");
        _logoLabel->setFixedSize(LOGO_WIDTH, LOGO_WIDTH);
        _logoLabel->setPixmap(QPixmap(":/res/png/application.png"));
    }

    _headW = new QWidget(this);
    _closeBtn = new SvgButton(this);
    _closeBtn->initSize(26,22);
    _closeBtn->setImagePrefixName("close");

#if 0
    connect(_closeBtn, SIGNAL(clicked()), SLOT(startCloseAnimal()));
#else
    connect(_closeBtn, SIGNAL(clicked()), SLOT(reject()));
#endif

    this->setStyleSheet("background: rgb(32,32,33)");
    _headW->setStyleSheet("background: rgb(27,27,28)");

    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setWindowState(Qt::WindowActive);
    //this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

    this->installEventFilter(this);
}

void CDialog::setNoClose(bool isHide)
{
    _closeBtn->setVisible(isHide);
}

void CDialog::setTitle(const QString &str)
{
    if(_titleLabel == NULL)
    {
        _titleLabel = new QLabel(_headW);
        _titleLabel->setAttribute(Qt::WA_DeleteOnClose);
        _titleLabel->setFrameShape(QFrame::NoFrame);
        _titleLabel->setFrameShadow(QFrame::Plain);
        _titleLabel->setStyleSheet("background:transparent");

        QPalette p = _titleLabel->palette();
        p.setColor(QPalette::WindowText, Qt::white);

        QFont font;
        font.setFamily(Global->getFontFamily());
        font.setPixelSize(14);
        font.setBold(true);

        _titleLabel->setPalette(p);
        _titleLabel->setFont(font);
    }
    _titleLabel->setText(str);
    _titleLabel->adjustSize();
}

void CDialog::setContentWidget(QWidget *w)
{
    if(w)
    {
        w->setParent(this);
        _contentW = w;
        _closeBtn->raise();
    }
}

void CDialog::DontShowHead()
{
    b_showHead = false;
    _headW->setVisible(false);
}

void CDialog::disableMove()
{
    m_bMove = false;
}

void CDialog::initSize(const int &w, const int &h)
{
    int headH = 0;

    _headW->setGeometry(0, 0, w, _closeBtn->height() + _space);

    if(b_showHead)
        headH = _headW->height();

    //_closeBtn->move(w - _closeBtn->width() - _space, (headH - _closeBtn->height())/2);
    _closeBtn->move(w - _closeBtn->width(), 0);

    if(_titleLabel)
    {
        _titleLabel->move((w-_titleLabel->width())/2, (headH-_titleLabel->height())/2);
    }

    int space = 0;
    if(b_space)
    {
        space = _space;
    }
    int cx = space;
    const int cy = headH + space;
    if(_logoLabel)
    {
        _logoLabel->move(cx, cy);
        cx = _logoLabel->width() + space * 2;
    }

    if(_contentW)
    {
        if(b_showHead)
            _contentW->setGeometry(cx, cy, w - cx - space, h - cy);
        else
            _contentW->setGeometry(0, 0, w, h);
    }
    this->resize(w, h);
}

void CDialog::updateUI(const qreal &scaleRatio)
{
    _space = SPACE_SIZE * scaleRatio;
    _closeBtn->updateUI(scaleRatio);

    if(_titleLabel)
    {
        QFont font;
        font.setFamily(Global->getFontFamily());
        font.setPixelSize(14 * scaleRatio);
        font.setBold(true);

        _titleLabel->setFont(font);
        _titleLabel->adjustSize();
    }

    if(_logoLabel)
        _logoLabel->setFixedSize(LOGO_WIDTH * scaleRatio, LOGO_WIDTH * scaleRatio);

    if(_contentW)
    {
        UpdateInterfaceWidget * contentW = qobject_cast<UpdateInterfaceWidget *>(_contentW);
        if(contentW)
        {
            contentW->updateUI(scaleRatio);
        }
    }

    const int w = this->width() / _ratio * scaleRatio;
    const int h = this->height() / _ratio * scaleRatio;

    initSize(w, h);
    this->move(this->x()+(_pressPoint.x()*(1.0-scaleRatio/_ratio)),
               this->y()+(_pressPoint.y()*(1.0-scaleRatio/_ratio)));

    if(_pressed)
    {
        _pressPoint *= scaleRatio/_ratio;
    }

    _ratio = scaleRatio;
}

void CDialog::recount(const QPoint &point)
{
    if(_curScreen) {
        if(_curScreen->geometry().contains(point, false)) {
            return;
        }
    }
    foreach (QScreen *screen, _screenList)
    {
        if(
                (point.x() >= screen->geometry().x() &&
                 point.x() <  screen->geometry().x() + screen->geometry().width()) &&
                (point.y() >= screen->geometry().y() &&
                 point.y() <  screen->geometry().y() + screen->geometry().height())
                )
        {
            if(_curScreen == screen)
                break;

            _curScreen = screen;
            changeWidgets();
            break;
        }
    }
}

void CDialog::screenChanged(QObject *obj)
{
    if(_curScreen == NULL) return;

    QScreen * screen = qobject_cast<QScreen *>(obj);
    if(_curScreen != screen)
        return;
    changeWidgets();
}

void CDialog::startCloseAnimal()
{
    _closeAnimal->setStartValue(1.0);
    _closeAnimal->setEndValue(0.0);
    _closeAnimal->start();
}

void CDialog::changeWidgets()
{
    if(_curScreen == NULL)
        return;

    updateUI(_curScreen->logicalDotsPerInch() / 96);
}

bool CDialog::eventFilter(QObject *obj, QEvent *e)
{
    switch(e->type())
    {
    case QEvent::MouseButtonPress:
    {
        if(m_bMove)
        {
            _pressed = true;
            _pressPoint = static_cast<QMouseEvent*>(e)->pos();
        }
        return true;
    }
    case QEvent::MouseMove:
    {
        if(_pressed)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            const QPoint p = mouseEvent->pos() - _pressPoint;
            this->move(this->pos() + p);
            //计算是否切换屏幕
            recount(mapToGlobal(mouseEvent->pos()));
        }
        return true;
    }
    case QEvent::MouseButtonRelease:
    {
        _pressed = false;
        return true;
    }
    case QEvent::Show:
    {
        recount(this->pos());
        _showAnimal->setStartValue(QRect(this->x(), this->y(), 0, this->height()));
        _showAnimal->setEndValue(this->geometry());
        _showAnimal->start();
    }
    default: break;
    }

    return QDialog::eventFilter(obj, e);
}
