#include "editdialog.h"
#include "playermainwidget.h"
#include <QKeyEvent>
#include <QPixmap>
#include <QPainter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include "editrightwidget.h"
#include "librarydata.h"
#include "dymediaplayer.h"
#include "globalarg.h"
#include "systemfunc.h"

#include <QDebug>

EditDialog::EditDialog(QWidget *parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);

    _scaleRatio     = Global->_screenRatio;
    QString file    = LibData->getcurmediaPath();
    int64_t curTime = MediaPlayer->getPosTime();
    _bottomWidget   = new EditCutWidget(file,0,this);
    _rightWidget    = new EditRightWidget(this);
    _inited = false;
    i_cursorTimerId = 0;
    _mousePress     = false;

    //点击关闭按钮,关闭窗口
    connect(_rightWidget,SIGNAL(closeEditWindow()),this,SLOT(close()));
    connect(_bottomWidget,SIGNAL(sendSelPts(double,double)),
            _rightWidget,SLOT(selectPtsChange(double,double)));
    connect(_rightWidget,SIGNAL(upWidgetGeometry()),SLOT(upGeometry()));

    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setWindowState(Qt::WindowActive);

    this->setMouseTracking(true);
    this->installEventFilter(this);

    this->setGeometry(PMW->geometry());
    setShowAnimation();
    setBorderWidth();
}

EditDialog::~EditDialog()
{
    if(_bottomWidget)
        _bottomWidget->deleteLater();
    if(_rightWidget)
        _rightWidget->deleteLater();
}

void EditDialog::updateUI(const qreal &scaleRatio)
{
    _rightWidget->updateUI(scaleRatio);
    _bottomWidget->updateUI(scaleRatio);

    _scaleRatio = scaleRatio;
    this->initGeometry(PMW->x(),PMW->y(),PMW->width(),PMW->height());
}

void EditDialog::setShowAnimation()
{
    int rwidth = (float)280 * _scaleRatio;
    int rheight = (float)PMW->geometry().height();
    _rightWidget->resize(rwidth,rheight);
    _rightWidget->show();
    _rightWidget->activateWindow();

    int width   = PMW->width() - rwidth;
    int height  = 108 * _scaleRatio;
    _bottomWidget->resize(width,height);
    _bottomWidget->show();
    _bottomWidget->activateWindow();
    QPropertyAnimation *aniRight, *aniBottom;
    QParallelAnimationGroup *aniGroup;
    const int duration = 500;
    aniRight = new QPropertyAnimation(_rightWidget, "pos", this);
    aniRight->setDuration(duration);
    aniRight->setStartValue(QPoint(this->width(),0));
    aniRight->setEndValue(QPoint(this->width() - 280*_scaleRatio,0));
    aniRight->setEasingCurve(QEasingCurve::OutCubic);

    aniBottom = new QPropertyAnimation(_bottomWidget, "pos", this);
    aniBottom->setDuration(duration);
    aniBottom->setStartValue(QPoint(0,this->height()));
    aniBottom->setEndValue(QPoint(0,this->height() - 108*_scaleRatio));
    aniBottom->setEasingCurve(QEasingCurve::OutCubic);

    aniGroup = new QParallelAnimationGroup(this);
    aniGroup->addAnimation(aniRight);
    aniGroup->addAnimation(aniBottom);
    aniGroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(aniGroup,&QParallelAnimationGroup::finished,[&]{
                _inited = true;
            });
}

void EditDialog::upGeometry()
{
    initGeometry(PMW->x(),PMW->y(),PMW->width(),PMW->height());
}

void EditDialog::initGeometry(const int &x, const int &y, const int &w, const int &h)
{
    if(PMW->showMax()) {
        this->setGeometry(x,y,w,h-1);
    } else {
        this->setGeometry(x,y,w,h);
    }

    int rwidth = 280 * _scaleRatio;
    _rightWidget->setGeometry(w - rwidth,
                              0,
                              rwidth,h);

    int bWidth   = PMW->width() -  rwidth;
    int bHeight  = 108 * _scaleRatio;
    _bottomWidget->setGeometry(0,
                               this->height() - bHeight,
                               bWidth,bHeight);
    _bottomWidget->upUI();
    setBorderWidth();
}

EditDialog::WidgetRegion EditDialog::hitTest(const QPoint &pos)
{
    for(int i=0; i<REGION_COUNT; ++i)
    {
        if(_regions[i].contains(pos))
            return EditDialog::WidgetRegion(i);
    }
    return Unknown;
}

void EditDialog::setBorderWidth()
{
    const uint borderWidth = 4*_scaleRatio;
    const int width  = this->width();
    const int height = this->height();

    _regions[Top]			= QRect(borderWidth, 0, width - borderWidth - borderWidth, borderWidth);
    _regions[TopRight]		= QRect(width - borderWidth, 0, borderWidth, borderWidth);
    _regions[Right]		    = QRect(width - borderWidth, borderWidth, borderWidth, height - borderWidth*2);
    _regions[RightBottom]	= QRect(width - borderWidth, height - borderWidth, borderWidth, borderWidth);
    _regions[Bottom]		= QRect(borderWidth, height - borderWidth, width - borderWidth*2, borderWidth);
    _regions[LeftBottom]	= QRect(0, height - borderWidth, borderWidth, borderWidth);
    _regions[Left]			= QRect(0, borderWidth, borderWidth, height - borderWidth*2);
    _regions[LeftTop]		= QRect(0, 0, borderWidth, borderWidth);
    _regions[Inner]		    = QRect(borderWidth, borderWidth, width - borderWidth*2, height - borderWidth*2);
}

void EditDialog::setCursorSharpe(const EditDialog::WidgetRegion &regin)
{
    switch (regin)
    {
    case Top:
        my_setCursor(CursorType::Top);
        break;
    case TopRight:
        my_setCursor(CursorType::TopRight);
        break;
    case Right:
        my_setCursor(CursorType::Right);
        break;
    case RightBottom:
        my_setCursor(CursorType::RightBottom);
        break;
    case Bottom:
        my_setCursor(CursorType::Bottom);
        break;
    case LeftBottom:
        my_setCursor(CursorType::LeftBottom);
        break;
    case Left:
        my_setCursor(CursorType::Left);
        break;
    case LeftTop:
        my_setCursor(CursorType::LeftTop);
        break;
    case Inner:
    default:
        my_setCursor(CursorType::Arrow);
        break;
    }
}

void EditDialog::startCursorTimer()
{
    stopCursorTimer();
    i_cursorTimerId = this->startTimer(50);
}

void EditDialog::stopCursorTimer()
{
    if (i_cursorTimerId != 0)
    {
        this->killTimer(i_cursorTimerId);
        i_cursorTimerId = 0;
    }
}

bool EditDialog::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        const QPoint curPos = mouseEvent->pos();
        _regionPressed = hitTest(curPos); //获取鼠标所在区域
        if (mouseEvent->button() == Qt::LeftButton)
        {
            stopCursorTimer();

            _mousePress = true;
            setCursorSharpe(_regionPressed);  //设置鼠标样式
            if(!PMW->isFullScreen())
                PMW->dialogPressedMove(this->mapToGlobal(curPos));
        }
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        const QPoint mousePos = this->mapToGlobal(mouseEvent->pos());
        if(!PMW->isFullScreen())
        {
            PMW->mouseReleaseWidget(mousePos);
            initGeometry(PMW->x(),PMW->y(),PMW->width(),PMW->height());
            PMW->dialogReleaseMove();
        }
        _mousePress = false;
        _regionPressed = Unknown;
        setCursorSharpe(Inner);
        _bottomWidget->dragSizeChange(false);
        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        const QPoint curPosLocal = mouseEvent->pos();
        if(!_mousePress) {
            setCursorSharpe(hitTest(curPosLocal));
            startCursorTimer();
        } else {
            if(!PMW->isFullScreen())
            {
                const QPoint curPosGlobal = this->mapToGlobal(curPosLocal);
                if(_regionPressed == Inner) {
                    if(_inited) {
                        PMW->dialogMoved(curPosGlobal);
                        initGeometry(PMW->x(),PMW->y(),PMW->width(),PMW->height());
                    }
                    this->activateWindow();
                } else {
                    PMW->dialogResize(curPosGlobal,_regionPressed);
                    initGeometry(PMW->x(),PMW->y(),PMW->width(),PMW->height());
                    _bottomWidget->dragSizeChange(true);
                }
            }
        }
        break;
    }
    case QEvent::Timer:
    {
        QTimerEvent* timerEvent = static_cast<QTimerEvent*>(event);
        if (timerEvent->timerId() == i_cursorTimerId)
        {
            const QPoint point = this->mapFromGlobal(QCursor::pos());
            if(this->rect().contains(point) && !_bottomWidget->rect().contains(_bottomWidget->mapFromGlobal(QCursor::pos()))) {
                if (_regions[Inner].contains(point)) {
                    my_setCursor(CursorType::Arrow);
                }
            }
            stopCursorTimer();
        }
        break;
    }
    default:
        break;
    }
    return QDialog::eventFilter(obj,event);
}

void EditDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPolygon poly;
    poly.append(QPoint(0,0));
    poly.append(QPoint(width(),0));
    poly.append(QPoint(width(),_rightWidget->geometry().top()));
    poly.append(QPoint(_rightWidget->geometry().left(),_rightWidget->geometry().top()));
    poly.append(QPoint(_rightWidget->geometry().left(),_bottomWidget->geometry().top()));
    poly.append(QPoint(0,_bottomWidget->geometry().top()));
    QPainterPath path;
    path.addPolygon(poly);
    painter.fillPath(path,QColor(0,0,0,10));
    QDialog::paintEvent(event);
}


