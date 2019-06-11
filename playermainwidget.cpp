#include "playermainwidget.h"
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QScreen>
#include <QCursor>
#include <QMimeData>
#include <QSignalMapper>
#include <QApplication>
#include <QtDebug>
#include <QMenu>
#include <QTimer>
#include <Windows.h>
#include <Dbt.h>
#include <winuser.h>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>
#include "ffmpegconvert.h"
#include "youtubedown.h"

#include "mainwidget.h"
#include "openfile.h"
#include "dymediaplayer.h"
#include "globalarg.h"
#include "headwidget.h"
#include "cmenu.h"
#include "virtuallinewidget.h"
#include "playcontrolwidget.h"
#include "librarydata.h"
#include "bonjourfind.h"
#include "updatalogic.h"
#include "upgradeytb.h"
#include "adlogic.h"

#define MARGIN_SIZE 4*_ratio

#define MIN_WIDTH 620
#define MIN_HEIGHT 400

#define NORMAL_WIDTH 900
#define NORMAL_HEIGHT 600

#define NO_CHANGE_SIZE_

//一天的毫秒数
#define ONEDAY_MSEC 1*24*60*60*1000

char FirstDriveFromMask(ULONG unitmask);

PlayerMainWidget::PlayerMainWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    MediaPlayer;
    p_videoWidget = NULL;
    p_trayIcon    = NULL;
    _trayMenu     = NULL;
    i_cursorTimerId = 0;

    b_init  = false;
    b_close = false;
    b_popDialog = false;
    b_startTimer = true;
    b_mousePressed = false;
    b_isControlPressed = false;
    b_isStartUpMain = Global->getMainUpFlag();
    this->setAcceptDrops(true);

    _ratio = 1.0;

    p_hideTimer = new QTimer(this);
    p_hideTimer->setInterval(2.5 * 1000);
    p_hideTimer->setSingleShot(true);
    connect(p_hideTimer, SIGNAL(timeout()), SLOT(hideMainWidget()));

    p_ytbupTimer = new QTimer(this);
    connect(p_ytbupTimer, SIGNAL(timeout()), SLOT(ytbUpgradeTimer()));
    p_ytbupTimer->setInterval(300 * 1000);
    p_ytbupTimer->setSingleShot(true);

    p_mainUpTimer = new QTimer(this);
    connect(p_mainUpTimer,SIGNAL(timeout()),SLOT(mainUpgradeTimer()));
    p_mainUpTimer->setInterval(10 * 1000);
    p_mainUpTimer->setSingleShot(true);

    p_controlWidget = new PlayControlWidget(this);
    p_controlWidget->ignoreLeaveEvent();//现在是2层界面  底层的鼠标事件有点儿不一样
    p_controlWidget->setVisible(false);

    p_mainWidget = new MainWidget(this);
    p_mainWidget->setVisible(false);
    this->setVisible(false);
    this->setWindowTitle("5KPlayer");

    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, QColor(0,0,0));
    this->setPalette(pal);
    this->setAutoFillBackground(true);

    this->setMouseTracking(true);
    p_mainWidget->setMouseTracking(true);
    p_controlWidget->setMouseTracking(true);

    this->installEventFilter(this);
    p_mainWidget->installEventFilter(this);
    p_controlWidget->installEventFilter(this);

    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    setWidgetSize(NORMAL_WIDTH, NORMAL_HEIGHT);
    //this->resize(NORMAL_WIDTH, NORMAL_HEIGHT);
    //p_mainWidget->resize(NORMAL_WIDTH, NORMAL_HEIGHT);
    setBorderWidth();
    Ad->startAdByAuto();
}

void PlayerMainWidget::createTrayIcon()
{
    if(!QSystemTrayIcon::isSystemTrayAvailable())
        return;
    p_trayIcon = new QSystemTrayIcon(this);
    p_trayIcon->setToolTip("5KPlayer");
    p_trayIcon->setIcon(QIcon(":/res/application.ico"));

    _trayMenu = new QMenu;
    QAction* open = _trayMenu->addAction(Lge->getLangageValue("PlayMain/open"), this, SLOT(showOut()));
    open->setObjectName("PlayMain/open");

    QAction* set = _trayMenu->addAction(Lge->getLangageValue("PlayMain/set"),
                                        OpenFileDialog, SLOT(openSettingsDialog()), QKeySequence("F2"));
    set->setObjectName("PlayMain/set");

    _trayMenu->addSeparator();
    QAction* quit = _trayMenu->addAction(Lge->getLangageValue("PlayMain/quit"), this, SLOT(closeWithConfig()));
    quit->setObjectName("PlayMain/quit");

    _trayMenu->setStyleSheet(Global->menuStyleSheet());
    p_trayIcon->setContextMenu(_trayMenu);
    //p_trayIcon->setVisible(true);
    //p_trayIcon->show();

    //p_trayIcon->showMessage("5KPlayer", "欢迎使用5KPlayer", QSystemTrayIcon::Information, 3000);

    connect(p_trayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason){
        switch(reason)
        {
        case QSystemTrayIcon::DoubleClick:
            this->showOut();
            break;
        default:
            break;
        }
    });

    //connect(p_trayIcon, &QSystemTrayIcon::messageClicked, [](){
    //    qDebug("-------------");
    //});

}

void PlayerMainWidget::changeWidgets()
{

#ifdef NO_CHANGE_SIZE
    //屏幕的
    const QRect geometry = _curScreen->geometry();
    const qreal scale1 = ((qreal)1920)/geometry.width();
    const qreal scale2 = ((qreal)1080)/geometry.height();
    const qreal scale = scale1 < scale2 ? scale1 : scale2;

    const qreal ratio = 1.0/scale;

    Global->_screenRatio = ratio;

    //字体缩放比例不能超过屏幕比例，否则容易造成字体大小超出控件大小，显示不全
    Global->_fontRatio   = _curScreen->logicalDotsPerInch() / 96.0;
    if(Global->_fontRatio > ratio)
        Global->_fontRatio = ratio;
#else
    const qreal ratio    =  qApp->logicalDotsPerInch() / 96.0;
    Global->_fontRatio   = ratio;
    Global->_screenRatio = ratio;
#endif

    updateUI(ratio);
}

bool PlayerMainWidget::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Move:
    {
        //修复切换主屏幕时， 窗口分离的bug
        if(obj == this)
        {
            QMoveEvent* e = static_cast<QMoveEvent*>(event);
            p_mainWidget->move(e->pos());
        }
        return true;
    }
    case QEvent::MouseMove:
    {
        if(!b_mousePressed)//鼠标没按下
            showMainWidget();
        break;
    }
    case QEvent::Wheel:
    {
        //在 5Kplayer下 并且 在播放video 并且 没弹出窗 并且不是全屏状态
        if(!b_popDialog && p_mainWidget->isPlayVideo() && !this->showMax() &&
                p_mainWidget->_head->isShowPlayerWidget())
        {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            const int numberSteps = wheelEvent->angleDelta().y() / 120;//120一格
            int cw = 0;
            if(numberSteps > 0) //放大
            {
                cw = this->width() + 32;
            }
            else //缩小
            {
                cw = this->width() - 32;
            }
            //如果显示了外部控制栏
            int ch = cw * Global->_hRatio / Global->_wRatio;
            bestSize(cw, ch, _ratio);
            if(p_controlWidget->isVisible())
            {
                ch += p_controlWidget->height() + MARGIN_SIZE;
            }
            this->setWidgetGeometry(this->x(), this->y(), cw, ch);
        }
        return true;
    }
    case QEvent::KeyPress:
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->isAutoRepeat()) {
            static int times = 0;
            ++times;
            if(times % 6 == 0) {
                times = 0;
                if(p_mainWidget->_head->isShowPlayerWidget() && !p_mainWidget->_playEnded)
                {
                    if(keyEvent->key() == Qt::Key_Right) //快进
                    {
                        MediaPlayer->fastForward();
                    }
                    else if(keyEvent->key() == Qt::Key_Left) //快退
                    {
                        MediaPlayer->fastRewind();
                    }
                    else if(keyEvent->key() == Qt::Key_Up)
                    {
                        MediaPlayer->audio()->addVolums();
                    }
                    else if(keyEvent->key() == Qt::Key_Down)
                    {
                        MediaPlayer->audio()->decVolums();
                    }
                }
            }
            return true;
        }
        if(keyEvent->key() == Qt::Key_Escape) //防止关闭 对话框
        {
            if(!OpenFileDialog->closeDialog()) {
                if(showMax() && !p_mainWidget->_playEnded)
                    this->showNormaled();
            }
            return true;
        }
        else if(keyEvent->key() == Qt::Key_F2) //设置界面
        {
            OpenFileDialog->openSettingsDialog();
            return true;
        }
        else if(keyEvent->modifiers() == Qt::ControlModifier)
        {
            switch (keyEvent->key())
            {
            case Qt::Key_O:
                OpenFileDialog->openVideoDialog();
                break;
            case Qt::Key_M:
                OpenFileDialog->openMusicDialog();
                break;
            case Qt::Key_D:
                OpenFileDialog->openDvdDialog();
                break;
            case Qt::Key_T:
                OpenFileDialog->openVideoTSDialog();
                break;
            default: break;
            }
            return true;
        }
        if(p_mainWidget->_head->isShowPlayerWidget() && !p_mainWidget->_playEnded)
        {
            if(keyEvent->key() == Qt::Key_Right) //快进
            {
                MediaPlayer->fastForward();
            }
            else if(keyEvent->key() == Qt::Key_Left) //快退
            {
                MediaPlayer->fastRewind();
            }
            else if(keyEvent->key() == Qt::Key_Up)
            {
                MediaPlayer->audio()->addVolums();
            }
            else if(keyEvent->key() == Qt::Key_Down)
            {
                MediaPlayer->audio()->decVolums();
            }
            else if(keyEvent->key() == Qt::Key_Space) //播放/暂停
            {
                MediaPlayer->togglePause();
            }
            else if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) //全屏
            {
                if(p_mainWidget->isPlayVideo()) {
                    normalToFull();
                    //showMax() ? showNormaled() : showFullScreened();
                }
            }
        }
        return true;
    }
    default: break;
    }


    if(obj == p_controlWidget)
    {
        switch(event->type())
        {
        case QEvent::MouseButtonPress:
        {
            if(!this->showMax()) {
                b_isControlPressed = true;
                dx_posRatio = static_cast<QMouseEvent *>(event)->pos().x() / (double)p_controlWidget->width();
                dy_posRatio = static_cast<QMouseEvent *>(event)->pos().y() / (double)p_controlWidget->height();
            }
            event->accept();
            return true;
        }
        case QEvent::MouseMove:
        {
            if(b_isControlPressed)
            {
                const QPoint curPos = static_cast<QMouseEvent *>(event)->pos();

                QPoint movePoint = p_controlWidget->mapToParent(curPos) -
                        QPoint(p_mainWidget->_playControl->width()*dx_posRatio,
                               p_mainWidget->_playControl->height()*dy_posRatio);
                if(movePoint.x() < 0)
                    movePoint.setX(0);
                else if(movePoint.x() + p_mainWidget->_playControl->width() > this->width())
                    movePoint.setX(this->width() - p_mainWidget->_playControl->width());
                if(movePoint.y() < p_mainWidget->headHeight())
                    movePoint.setY(p_mainWidget->headHeight());
                else if(movePoint.y() + p_mainWidget->_playControl->height() * 1 > this->height())
                    movePoint.setY(this->height() - p_mainWidget->_playControl->height() * 1);

                //设置线框位置
                if(curPos.y() < 0) {
                    if(p_mainWidget->_vrW->isHidden()) {
                        p_mainWidget->_vrW->resize(p_mainWidget->_playControl->width(),
                                                   p_mainWidget->_playControl->height());
                        //p_mainWidget->_vrW->setLineColor(Qt::red);
                        p_mainWidget->_vrW->setLineColor(QColor(225,63,59));
                        p_mainWidget->_vrW->show();
                    }
                    p_mainWidget->_vrW->move(this->mapToGlobal(movePoint));
                }
                else {
                    p_mainWidget->_vrW->hide();
                }
            }
            event->accept();
            return true;
        }
        case QEvent::MouseButtonRelease:
        {
            dx_posRatio = 0.0;
            dy_posRatio = 0.0;
            releaseMouse();

            event->accept();
            return true;
        }
        case QEvent::WindowDeactivate:
        {
            b_isControlPressed = false;
            p_mainWidget->_vrW->hide();
            event->accept();
            return true;
        }
        default: break;
        }
        return QWidget::eventFilter(obj, event);
    }

    switch (event->type())
    {
    case QEvent::MouseMove:
    {
        //如果界面在5KPlayer下 并且 在播放,隐藏相关控制
        if(showMax()) break;

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        const QPoint curPosLocal = mouseEvent->pos();

        if (!b_mousePressed)	// 鼠标未按下
        {
            setCursorSharpe(hitTest(curPosLocal));

            startCursorTimer();
        }
        else
        {
            const QPoint curPosGlobal = this->mapToGlobal(curPosLocal);

            switch (_regionPressed)
            {
            case Inner:
            {
                moveWidget(m_originRect.topLeft() + curPosGlobal - m_originPosGlobal);
                recount(curPosGlobal);
                break;
            }
            case Top:
            {
                resizeGeometry(QPoint(m_originPosGlobal.x(), curPosGlobal.y()), 1, -1);
                break;
            }
            case TopRight:
            {
                resizeGeometry(curPosGlobal, 1, -1);
                break;
            }
            case Right:
            {
                resizeGeometry(QPoint(curPosGlobal.x(), m_originPosGlobal.y()), 1, 1);
                break;
            }
            case RightBottom:
            {
                resizeGeometry(curPosGlobal, 1, 1);
                break;
            }
            case Bottom:
            {
                resizeGeometry(QPoint(m_originPosGlobal.x(), curPosGlobal.y()), 1, 1);
                break;
            }
            case LeftBottom:
            {
                resizeGeometry(curPosGlobal, -1, 1);
                break;
            }
            case Left:
            {
                resizeGeometry(QPoint(curPosGlobal.x(), m_originPosGlobal.y()), -1, 1);
                break;
            }
            case LeftTop:
            {
                resizeGeometry(curPosGlobal, -1, -1);
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case QEvent::MouseButtonPress:
    {
        if(!showMax())
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                stopCursorTimer();

                b_mousePressed = true;
                const QPoint curPos = mouseEvent->pos();
                _regionPressed = hitTest(curPos); //获取鼠标所在区域
                setCursorSharpe(_regionPressed);  //设置鼠标样式

                m_pressedPoint = curPos;
                m_originPosGlobal = this->mapToGlobal(curPos);//转为屏幕坐标
                m_originRect = this->geometry();
            }
        }
        return true;
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        b_mousePressed = false;
        _regionPressed = Unknown;
        setCursorSharpe(Inner);
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if(mouseEvent->button() == Qt::RightButton &&
                this->rect().contains(mouseEvent->pos()) &&
                obj != p_mainWidget) //弹出右键菜单
        {
            Menu->showRightMenu(cursor().pos());
        }
        else if(!this->showMax() && mouseEvent->button() == Qt::LeftButton)
        {
            //判断鼠标是否在边缘
            const QPoint mousePos = this->mapToGlobal(mouseEvent->pos());
            int edge = 0;

            if(edge == 0) //鼠标在边缘位置，把窗口贴边
            {
                const int sx = qApp->availableGeometry().x();
                const int sy = qApp->availableGeometry().y();
                const int sw = qApp->availableGeometry().width();
                const int sh = qApp->availableGeometry().height();

                const int cx = this->geometry().x();
                const int cy = this->geometry().y();
                const int cw = this->geometry().width();
                const int ch = this->geometry().height();

                int x=cx, y=cy;
                if(cx < sx)
                    x = sx;
                else if(cx + cw > sx + sw)
                    x = sx + sw - cw;

                if(cy < sy)
                    y = sy;
                else if(cy + ch > sy +sh)
                    y = sy + sh - ch;

                this->moveWidget(x, y);
            }
            else if(edge == 1) //鼠标在屏幕内
            {
                //判断程序是否意外飞出屏幕。
                bool in = true;
                const QPoint pos1 = this->geometry().topLeft();
                const QPoint pos2 = this->geometry().bottomLeft();
                const QPoint pos3 = this->geometry().topRight();
                const QPoint pos4 = this->geometry().bottomRight();

                if(!in)
                {
                    //移动到屏幕中间
                    int x = qApp->availableGeometry().x() +
                            (qApp->availableGeometry().width() - this->width())/2;
                    int y = qApp->availableGeometry().y() +
                            (qApp->availableGeometry().height() - this->height())/2;
                    this->moveWidget(x, y);
                }
            }
        }
        return true;
    }
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::LeftButton && !p_mainWidget->_playEnded)
        {
            //showMax() ? showNormaled() : showFullScreened();
            normalToFull();
        }
        return true;
    }
    case QEvent::Timer:
    {
        if(obj != p_mainWidget)
        {
            QTimerEvent* timerEvent = static_cast<QTimerEvent*>(event);
            if (timerEvent->timerId() == i_cursorTimerId)
            {
                const QPoint point = this->mapFromGlobal(QCursor::pos());
                if(this->rect().contains(point)) {
                    if (_regions[Inner].contains(point)) {
                        ::SetCursor(LoadCursor(NULL, IDC_ARROW));
                    }
                }
                else { //隐藏鼠标
                    if(p_mainWidget->isPlayVideo() && p_mainWidget->_head->isShowPlayerWidget())
                        hideMainWidget();
                }
                stopCursorTimer();
            }
            return true;
        }
        break;
    }
    case QEvent::DragEnter:
    {
        if(obj == p_mainWidget)
            return true;
        QDragEnterEvent *e = static_cast<QDragEnterEvent *>(event);
        if(e->mimeData()->urls().count() == 1)
            e->setDropAction(Qt::MoveAction);
        else
            e->setDropAction(Qt::IgnoreAction);
        e->accept();
        return true;
    }
    case QEvent::Drop:
    {
        if(obj == p_mainWidget)
            return true;
        QDropEvent *e = static_cast<QDropEvent *>(event);
        OpenFileDialog->openFile(e->mimeData()->urls().at(0).toLocalFile());
        e->accept();
        return true;
    }
    case QEvent::Close:
    {
        if(b_close) {
            startClose();
        } else {
            closeWithConfig(true);
            if(b_close)
                startClose();
            else
                event->ignore();
        }
        return true;
    }
    default: break;
    }

    return QWidget::eventFilter(obj, event);
}

#ifdef Q_OS_MACX
void PlayerMainWidget::initSysMenu()
{
    //1. 获取菜单栏
    QMenuBar *menuBar = new QMenuBar(0);

    m_File      = new QMenu(Lang("Menu/file"));

    QAction* ofe = m_File->addAction(Lge->getLangageValue("Menu/OFile"), OpenFileDialog, SLOT(openVideoDialog()));
    ofe->setShortcut(QKeySequence("Ctrl+O"));
    ofe->setObjectName("Menu/OFile");
    QAction* dvd = m_File->addAction(Lge->getLangageValue("Menu/ODVD"),      OpenFileDialog, SLOT(openDvdDialog()));
    dvd->setShortcut(QKeySequence("Ctrl+D"));
    dvd->setObjectName("Menu/ODVD");
    QAction* vts = m_File->addAction(Lang("Menu/OVideoTS"), OpenFileDialog, SLOT(openVideoTSDialog()));
    vts->setShortcut(QKeySequence("Shift+Ctrl+D"));
    vts->setObjectName("Menu/OVideoTS");
    m_File->addMenu(Menu->getrecentMenu());
    m_File->addSeparator();
    QAction *close =  m_File->addAction(Lang("Menu/close"));
    close->setObjectName("Menu/close");
    close->setShortcut(QKeySequence("Ctrl+W"));
    connect(close, &QAction::triggered, [=](){
        PMW->closeBtnClick();
    });

    m_View      = new QMenu(Lang("Menu/view"));
    connect(m_View,SIGNAL(aboutToShow()),SLOT(showView()));
    ac_horz = m_View->addAction(Lge->getLangageValue("Menu/horz"),[](bool check){MediaPlayer->rotate()->hflip(check);});
    ac_horz->setObjectName("Menu/horz");
    ac_horz->setShortcut(QKeySequence("Shift+Ctrl+H"));
    ac_horz->setCheckable(true);
    ac_vert = m_View->addAction(Lge->getLangageValue("Menu/vert"),[](bool check){MediaPlayer->rotate()->vflip(check);});
    ac_vert->setObjectName("Menu/vert");
    ac_vert->setShortcut(QKeySequence("Shift+Ctrl+V"));
    ac_vert->setCheckable(true);
    m_View->addSeparator();

    ac_leftRota = m_View->addAction(Lge->getLangageValue("MainHead/left"),[](){PMW->rotateLeft();},QKeySequence("Shift+Ctrl+L"));
    ac_leftRota->setObjectName("MainHead/left");
    ac_righRota = m_View->addAction(Lge->getLangageValue("MainHead/right"),[](){PMW->rotateRight();},QKeySequence("Shift+Ctrl+R"));
    ac_righRota->setObjectName("MainHead/right");
    m_View->addSeparator();

//    QAction* half = m_View->addAction(Lang("Menu/half"),[](){PMW->showNormaled();},QKeySequence("Ctrl+1"));
//    half->setObjectName("Menu/half");
//    QAction* orig = m_View->addAction(Lang("Menu/orig"),[](){MediaPlayer->rotate()->vflip();},QKeySequence("Ctrl+2"));
//    orig->setObjectName("Menu/orig");
    ac_fullScn = m_View->addAction(Lang("Menu/full"),[](){PMW->showFullScreened();},QKeySequence("Ctrl+F"));
    ac_fullScn->setObjectName("Menu/full");

    m_PlayBack  = new QMenu(Lang("Menu/playback"));
    ac_play = m_PlayBack->addAction(Lang("Menu/play"),[](){PMW->menuPlayClick();},QKeySequence("Space"));
    ac_play->setObjectName("Menu/play");
    ac_stop = m_PlayBack->addAction(Lang("Menu/stop"),[](){MediaPlayer->stop();},QKeySequence("Ctrl+."));
    ac_stop->setObjectName("Menu/stop");
    m_PlayBack->addSeparator();
    ac_stepF = m_PlayBack->addAction(Lang("Menu/stepF"),[](){MediaPlayer->fastForward();},QKeySequence("Right"));
    ac_stepF->setObjectName("Menu/stepF");
    ac_stepB = m_PlayBack->addAction(Lang("Menu/stepB"),[](){MediaPlayer->fastRewind();},QKeySequence("Left"));
    ac_stepB->setObjectName("Menu/stepB");
    m_PlayBack->addSeparator();
    ac_prev   = m_PlayBack->addAction(Lang("Menu/previous"),[](){MediaPlayer->playPre();},QKeySequence("Ctrl+Left"));
    ac_prev->setObjectName("Menu/previous");
    ac_next   = m_PlayBack->addAction(Lang("Menu/next"),[](){MediaPlayer->playNext();},QKeySequence("Ctrl+Right"));
    ac_next->setObjectName("Menu/next");
    m_PlayBack->addSeparator();
    ac_incVal  = m_PlayBack->addAction(Lang("Menu/voladd"),[](){MediaPlayer->audio()->decVolums();},QKeySequence("Up"));
    ac_incVal->setObjectName("Menu/voladd");
    ac_decVal  = m_PlayBack->addAction(Lang("Menu/voldec"),[](){MediaPlayer->audio()->addVolums();},QKeySequence("Down"));
    ac_decVal->setObjectName("Menu/voldec");
    ac_mute    = m_PlayBack->addAction(Lang("Menu/mute"),[](){MediaPlayer->audio()->toggleVolunMute();},QKeySequence("Alt+Ctrl+Down"));
    ac_mute->setObjectName("Menu/mute");

    m_Window    = new QMenu(Lang("Menu/window"));
    //层次
    QActionGroup* _stayGroup = new QActionGroup(this);
    _stayGroup->setExclusive(true);
    ac_always = m_Window->addAction(Lge->getLangageValue("Menu/always"),[](){PMW->stayAlwaysOnTop();},QKeySequence("Ctrl+T"));
    ac_whplay = m_Window->addAction(Lge->getLangageValue("Menu/WPlay"), [](){PMW->stayPlayingOnTop();},QKeySequence("Shift+Ctrl+T"));
    ac_nevers = m_Window->addAction(Lge->getLangageValue("Menu/never"), [](){PMW->stayNeverOnTop();},QKeySequence("Ctrl+B"));
    ac_always->setCheckable(true);
    ac_always->setObjectName("Menu/always");
    ac_always->setActionGroup(_stayGroup);
    ac_whplay->setCheckable(true);
    ac_whplay->setObjectName("Menu/never");
    ac_whplay->setActionGroup(_stayGroup);
    ac_nevers->setCheckable(true);
    ac_nevers->setObjectName("Menu/WPlay");
    ac_nevers->setActionGroup(_stayGroup);

    switch (Global->stayOnTop())
    {
    case CGlobal::General_Stay::Stay_Always:
        ac_always->setChecked(true);
        Menu->updateStayOnTop(0);
        break;
    case CGlobal::General_Stay::Stay_Never:
        ac_nevers->setChecked(true);
        Menu->updateStayOnTop(2);
        break;
    case CGlobal::General_Stay::Stay_Playing:
        ac_whplay->setChecked(true);
        Menu->updateStayOnTop(1);
        break;
    default:
        ac_nevers->setChecked(true);
        Menu->updateStayOnTop(2);
        break;
    }

    m_Window->addSeparator();
    QString title = Lang("Menu/showtitle");
    QString objName = "Menu/showtitle";
   if(Global->getShowPlayTitle()) {
       title = Lang("Menu/hidetitle");
       objName = "Menu/hidetitle";
   }

    ac_showTitle = m_Window->addAction(title,[](){PMW->showPlayTitle();});
    ac_showTitle->setObjectName(objName);

    QString sumeTitle = Lang("Menu/enResume");
    QString sumeName = "Menu/enResume";
    if(Global->getResumePlayback()) {
        sumeTitle = Lang("Menu/dsResume");
        sumeName = "Menu/dsResume";
    }
    ac_resume = m_Window->addAction(sumeTitle,[](){PMW->enResume();});
    ac_resume->setText(sumeTitle);
    ac_resume->setObjectName(sumeName);


    m_Window->addSeparator();
    ac_mediainfo = m_Window->addAction(Lang("Menu/minfo"),OpenFileDialog, SLOT(openMediaInfoDialog()),QKeySequence("Ctrl+|"));
    ac_mediainfo->setObjectName("Menu/minfo");
    ac_vTuner= m_Window->addAction(Lge->getLangageValue("Menu/tuner"), OpenFileDialog, SLOT(openVideoTunerDialog()));
    ac_vTuner->setObjectName("Menu/tuner");
    m_Window->addSeparator();
    m_mini = m_Window->addAction(Lang("Menu/min"),[](){PMW->showMinimied();},QKeySequence("Ctrl+M"));
    m_mini->setObjectName("Menu/min");
    m_mini->setEnabled(false);

    m_help = new QMenu(Lge->getLangageValue("Menu/help"));
    QAction* help = m_help->addAction(Lge->getLangageValue("Menu/help"),    OpenFileDialog, SLOT(openHelpUrl()));
    help->setObjectName("Menu/help");
    QAction* fdk = m_help->addAction(Lge->getLangageValue("Menu/fdk&sug"),  OpenFileDialog, SLOT(openSendEmail())); //ToDo
    fdk->setObjectName("Menu/fdk&sug");
    m_help->addMenu(Menu->getLanguMenu());


    m_mainMenu = menuBar->addMenu("mainmenu");
    ac_about = m_mainMenu->addAction(Lge->getLangageValue("Menu/about"),  OpenFileDialog, SLOT(openAboutDialog()));
    ac_about->setObjectName("Menu/about");
    ac_registAc = m_mainMenu->addAction(Lge->getLangageValue("Menu/reg"),OpenFileDialog, SLOT(openRegisterDialog()));
    ac_registAc->setObjectName("Menu/reg");
    ac_checkUp = m_mainMenu->addAction(Lge->getLangageValue("Menu/ckUp"),    OpenFileDialog, SLOT(openUpdateDialog()),QKeySequence("Ctrl+U"));
    ac_checkUp->setObjectName("Menu/ckUp");
    ac_Prefer = m_mainMenu->addAction(Lge->getLangageValue("Menu/pref"),    OpenFileDialog, SLOT(openSettingsDialog()));
    ac_Prefer->setObjectName("Menu/pref");


    ac_about->setMenuRole(QAction::AboutRole);

    ac_Prefer->setMenuRole(QAction::PreferencesRole);
    ac_registAc->setMenuRole(QAction::ApplicationSpecificRole);
    ac_checkUp->setMenuRole(QAction::ApplicationSpecificRole);
    //upMenuTitle();
    menuBar->addMenu(m_File);
    menuBar->addMenu(m_View);
    menuBar->addMenu(m_PlayBack);
    menuBar->addMenu(m_Window);
    //menuBar->addMenu(Menu->getLanguMenu());
    menuBar->addMenu(m_help);
    menuBar->addMenu(m_mainMenu);

    upMenuEnable(false,1);
}
#endif
bool PlayerMainWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG *msg = static_cast<MSG *>(message);
    switch (msg->message) {
    case WM_COPYDATA:
    {
        COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT*>(msg->lParam);
        QString strMessage = QString::fromUtf8(reinterpret_cast<char*>(cds->lpData),
                                               cds->cbData);
        *result = 1;
        if(strMessage.toLower() == "exit_9")
            closeWithConfig();
        return true;
    }
    case WM_DEVICECHANGE:
    {
        PDEV_BROADCAST_HDR lpdb=(PDEV_BROADCAST_HDR)msg->lParam;

        if(lpdb && msg->wParam == DBT_DEVICEARRIVAL)
        {
            if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)//逻辑卷
            {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;

                if(lpdbv == NULL) return true;

                char ch = FirstDriveFromMask(lpdbv->dbcv_unitmask);
                QString path = QString(ch).append(":/");

                uint flag = GetDriveTypeA(path.toStdString().c_str());

                if(flag == DRIVE_CDROM)
                {
                    QString file = path + "VIDEO_TS/VIDEO_TS.IFO";
                    if(QFileInfo::exists(file))
                    {
                        this->showOut();
                        Ad->startAdByEventType("DVD","");
                    }
                }
            }
        }
        return true;
    }
    default: break;
    }
    return QWidget::nativeEvent(eventType, message, result);
}

char FirstDriveFromMask(ULONG unitmask)
{
    char i;
    for (i = 0; i < 26; ++i)
    {
        if (unitmask & 0x1)
            break;
        unitmask = unitmask >> 1;
    }
    return (i + 'A');
}

bool PlayerMainWidget::showMax() const
{
    return this->isFullScreen() || this->isMaximized();
}

int PlayerMainWidget::headHeight() const
{
    return p_mainWidget->headHeight();
}

void PlayerMainWidget::startConnectedMode()
{
    //如果正在播放 记住当前播放的文件  类型  radio  dar
    p_mainWidget->joinConnectMode();
    //OpenFileDialog->sendNetworkInfo(文件  类型  radio  dar);
}

void PlayerMainWidget::closeConnectedMode()
{
    p_mainWidget->closeConnectMode();
}

void PlayerMainWidget::playUrl(const QString &url)
{
    p_mainWidget->airplayVideoStart(url);
}

void PlayerMainWidget::startClose()
{
    p_hideTimer->stop();
    p_ytbupTimer->stop();
    //关闭弹出框
    OpenFileDialog->closeDialog();

    //隐藏界面
    this->p_mainWidget->accept();
    this->hide();
    if(p_trayIcon)
        p_trayIcon->hide();

    //断开信号
    this->disconnect();
    Menu->disconnect();
    LibData->disconnect();
    MediaPlayer->disconnect();
    p_controlWidget->disconnect();

    //恢复屏保设置
    LibData->recScreenprotection();

    CMenu::clear();

    MediaPlayer->stop();
    DYMediaPlayer::clear();

    FFmpegConvert::clear();
    YoutubeDown::clear();

    p_controlWidget->removeEventFilter(this);

    if(p_mainWidget)
    {
        p_mainWidget->disconnect();
        p_mainWidget->removeEventFilter(this);
        p_mainWidget->accept();
        p_mainWidget->deleteLater();
        p_mainWidget = NULL;
    }
    qApp->quit();
}

QSize PlayerMainWidget::minimumSize() const
{
    return QSize(minimumWidth(), minimumHeight());
}

int PlayerMainWidget::minimumWidth() const
{
#if 0
    return MIN_WIDTH * _ratio;
#else
    return MIN_WIDTH;
#endif
}

int PlayerMainWidget::minimumHeight() const
{
#if 0
    return MIN_HEIGHT * _ratio;
#else
    return MIN_HEIGHT;
#endif
}

void PlayerMainWidget::showOut()
{
    if(p_trayIcon)
        p_trayIcon->show();
    if(this->isHidden()) {
        //this->showNormalCenter();
        //this->showNormaled();
        this->showCenter(false);
        this->setWidgetGeometry(m_normalRect);

        this->setVisible(true);
        p_mainWidget->setVisible(true);
    }
    else if(this->isMinimized())
        this->showNormal();

    this->activateWidget();

    qApp->processEvents();

    init();
}

void PlayerMainWidget::initMediaplayer(const QString &file)
{
    b_init = true;
    //BJFind;//开启 bonjour服务
    connect(MediaPlayer, SIGNAL(positionChanged(float)), p_controlWidget, SLOT(setCurTime(float)));
    connect(MediaPlayer, SIGNAL(lengthChanged(int)),     p_controlWidget, SLOT(setTotalTime(int)));

    MediaPlayer->setVideoWidget(this);

    p_mainWidget->initMediaplayerSignals();

    createTrayIcon();

    if(file != "-auto" && file != "/auto" && file != "auto")
    {
        if(p_trayIcon)
            p_trayIcon->show();
        if(!init()) return;
        if(!file.isEmpty())
        {
            OpenFileDialog->openFile(file);
        }
    }
    else
    {
        //开机启动后  延迟5秒显示图标
        if(p_trayIcon)
            QTimer::singleShot(1000 * 5, p_trayIcon, SLOT(show()));
    }

    //读取配置文件 是否启动airplay
    if(Global->airplayStarted() && Global->bonjourServiceState() == 1)
    {
        startAirplay();
    }
    if(!b_isStartUpMain)
        p_mainUpTimer->start();
    p_ytbupTimer->start();
}

bool PlayerMainWidget::init()
{
    if(Global->getMainUpFlag()) {
        autoCheckUpMain();
    }
    static int i = 0;
    if(i == 1) return true;
    ++i;

    //每次启动 检测关联
    if(Global->autoAssociation()) {
        QtConcurrent::run(this, autoAssociation);
    }

    if(!Global->checkRegistCode(Global->registCode())) {
        if(Global->openLogin() == CGlobal::General_Login::Login_Yes)
            OpenFileDialog->openRegisterDialog();
    } else {
        //隐藏菜单
        Menu->hideRegistMenu();
    }

    if(b_close) return false; //防止刚显示出来 点击退出

    switch (Global->stayOnTop())
    {
    case CGlobal::General_Stay::Stay_Always:
        this->stayAlwaysOnTop();
        break;
    case CGlobal::General_Stay::Stay_Never:
        this->stayNeverOnTop();
        break;
    case CGlobal::General_Stay::Stay_Playing:
        this->stayPlayingOnTop();
        break;
    default:
        this->stayNeverOnTop();
        break;
    }
    return true;
}
#include "filerelation.h"
void PlayerMainWidget::autoAssociation()
{
    QStringList suffixs;
    QStringList args;
    args << "-reg";
    //读取配置文件
    QString fileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    fileName.append("/uninstall.dat");
    QFile fi(fileName);
    if(fi.open(QIODevice::ReadOnly)) {
        suffixs = QString(fi.readAll()).split(" ", QString::SkipEmptyParts);
        fi.close();
    }

    bool isRegist = true;

    QString path(Global->_cur_path);
    path.replace("/", "\\");
    FileRelation relation(path);
    for(QString suffix : suffixs) {
        isRegist = true;
        suffix.remove("(");
        suffix.remove(")");

        if(suffix != "DVD")
            isRegist = relation.isFileRelation(suffix);
        else
            isRegist = relation.isRegistDvd();

        if(!isRegist)
            args << suffix;
    }

    if(args.count() > 2)
        QProcess::startDetached(Global->_cur_path + "/FileRelation.exe", args, ".");
}

void PlayerMainWidget::autoCheckUpMain()
{
    UpLogic->disconnect(SIGNAL(upAutoUP(const QByteArray &)));
    connect(UpLogic, &UpdataLogic::upAutoUP, [=](QByteArray data){
        if(!this->isHidden()) {
            //OpenFileDialog->openAutoUpdateDialog(data);
            Ad->startAdByEventType("Upgrade","");
        } else {
            Global->setMainUpFlag(true);
        }
    });
    UpLogic->startCheckVersion(true);
}

void PlayerMainWidget::normalToFull()
{
    // 双击第一次，切换到视频原始分辨率
    // 双击第二次，切换到全屏
    // 双击第三次，切换到默认大小
    if(showMax())
    {
        showNormaled();
        return;
    }
    const int w = this->width();
    const int h = this->height();
    const int sw = qApp->availableGeometry().width();
    const int sh = qApp->availableGeometry().height();


    if( (Global->_w <= 0 || Global->_h <= 0) ||
        (Global->_w <= MIN_WIDTH*_ratio || Global->_h <= MIN_HEIGHT*_ratio) ||
        (w >= Global->_w*_ratio || h >= Global->_h*_ratio) ||
        (Global->_w*_ratio >= sw || Global->_h*_ratio >= sh))
    {
        const QRect rect = m_normalRect;
        showFullScreened();
        m_normalRect = rect;
    }
    else
    {
        showMainWidget();//防止隐藏后全屏 不自动隐藏
        p_mainWidget->showNormal();
        this->showNormal();

        const QRect rect = this->geometry();
        const int w = Global->_w*_ratio;
        const int h = Global->_h*_ratio;
        const int x = rect.x() + (rect.width() - w)/2;
        const int y = rect.y() + (rect.height() - h)/2;

        this->setWidgetGeometry(x,y,w,h);
        //this->setWidgetSize(Global->_w*_ratio, Global->_h*_ratio);
        //this->showCurrentCenter(Global->_w, Global->_h);
        m_normalRect = rect;
    }
    //showMax() ? showNormaled() : showFullScreened();
}

void PlayerMainWidget::languageChange(const QString &language)
{
    Global->setLanguage(language);
    Lge->initLangage(language);

    updateLanguage();
}
#include "airplay.h"
void PlayerMainWidget::playPre()
{
    if(p_mainWidget->b_airplayMusic)
        p_mainWidget->p_airPlay->sendMusicControl(2);
    else
        MediaPlayer->playPre();
}

void PlayerMainWidget::playNext()
{
    if(p_mainWidget->b_airplayMusic)
        p_mainWidget->p_airPlay->sendMusicControl(1);
    else
        MediaPlayer->playNext();
}

void PlayerMainWidget::toggleMute()
{
    if(p_mainWidget->b_airplayMusic)
        p_mainWidget->p_airPlay->sendMusicControl(5);
    MediaPlayer->audio()->toggleVolunMute();
}

void PlayerMainWidget::togglePause(const bool &b)
{
    if(p_mainWidget->b_airplayMusic)
        p_mainWidget->p_airPlay->sendMusicControl(3);
    else if(p_mainWidget->b_airplayVideo) {
        if(MediaPlayer->state() == DYVlc::Paused)
            p_mainWidget->p_airPlay->sendVideoControl(2);
        else
            p_mainWidget->p_airPlay->sendVideoControl(3);
    }
    if(b)
        MediaPlayer->togglePause();
}

void PlayerMainWidget::moveWidget(const QPoint &p)
{
    moveWidget(p.x(), p.y());
}

void PlayerMainWidget::moveWidget(const int &x, const int &y)
{
    this->move(x, y);
}

void PlayerMainWidget::setWidgetSize(const int &w, const int &h)
{
    this->resize(w, h);
    sizeChanged(w, h);
}

void PlayerMainWidget::setWidgetGeometry(const QRect &rect)
{
    setWidgetGeometry(rect.x(), rect.y(),
                      rect.width(), rect.height());
}

void PlayerMainWidget::setWidgetGeometry(const int &x, const int &y, const int &w, const int &h)
{
    this->setGeometry(x, y, w, h);
    sizeChanged(w, h);
}

bool PlayerMainWidget::airplayStarted()
{
    return p_mainWidget->airplayStarted();
}

bool PlayerMainWidget::startAirplay()
{
    return p_mainWidget->startAirPlay();
}

void PlayerMainWidget::closeAirplay()
{
    p_mainWidget->stopAirPlay();
}

void PlayerMainWidget::dialogPressedMove(const QPoint &point)
{
    b_mousePressed = true;
    m_pressedPoint = this->mapFromGlobal(point);
    m_originPosGlobal = point;
    m_originRect = this->geometry();
}

void PlayerMainWidget::dialogMoved(const QPoint &point)
{
    moveWidget(m_originRect.topLeft() + point - m_originPosGlobal);
    recount(point);
}

void PlayerMainWidget::dialogReleaseMove()
{
    b_mousePressed = false;
}

void PlayerMainWidget::changePlayModeImage()
{
    p_controlWidget->updatePlayModeImage();
    p_mainWidget->_playControl->updatePlayModeImage();
}

void PlayerMainWidget::closeWithConfig(bool checkConfig)
{
    bool b_tmp_close = true;
    if(checkConfig)
    {
        switch (Global->closeType())
        {
        case CGlobal::General_Close::Close_Mini:
        {
            b_tmp_close = false;
            OpenFileDialog->closeDialog();//关闭对话框

            if(Global->stayOnTop() == CGlobal::General_Stay::Stay_Playing)
                this->offTop();

            this->hidePlayControl();//隐藏外部控制栏
            p_mainWidget->accept();
            this->hide();

            this->stopVlc(false);
            break;
        }
        case CGlobal::General_Close::Close_Exit:
        default: break;
        }
    }
    if(b_tmp_close) {
        if(!OpenFileDialog->isOpenSysDialog()) {
            b_close = true;
            this->close();
        } else {
            this->activateWidget();
        }
    }
}

void PlayerMainWidget::sizeChanged(const int &w, const int &h)
{
    setBorderWidth();

    int videoHeight = h;
    if(p_controlWidget->isVisible()) //保持外部控制条 与 窗口宽度一致
    {
        if(this->showMax())
        {
            p_controlWidget->setGeometry(0, h - p_controlWidget->height(),
                                         w, p_controlWidget->height());
            p_controlWidget->raise();
        }
        else
        {
            const int margin = MARGIN_SIZE;
            p_controlWidget->setGeometry(margin, h - p_controlWidget->height() - margin,
                                         w - 2 * margin, p_controlWidget->height());

            videoHeight = p_controlWidget->y();
        }
    }
    if(p_videoWidget)
    {
        p_videoWidget->setGeometry(0,0, w, videoHeight);
    }

    p_mainWidget->resize(w,h);
}

void PlayerMainWidget::setBorderWidth()
{
    const uint borderWidth = MARGIN_SIZE;
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

PlayerMainWidget::WidgetRegion PlayerMainWidget::hitTest(const QPoint &pos)
{
    for(int i=0; i<REGION_COUNT; ++i)
    {
        if(_regions[i].contains(pos))
            return PlayerMainWidget::WidgetRegion(i);
    }
    return Unknown;
}

void PlayerMainWidget::resizeGeometry(const QPoint &newPos, const int &directX, const int &directY)
{
    if(!b_mousePressed) return;

    const int minWidth  = MIN_WIDTH  * _ratio;
    const int minHeight = MIN_HEIGHT * _ratio;
    const int maxWidth  = qApp->primaryScreen();en->availableGeometry().width();
    const int maxHeight = qApp->availableGeometry().height();

    const QPoint delta = newPos - m_originPosGlobal;

    int tmpW,tmpH;

    if(directX >= 0)
        tmpW = m_originRect.width() + delta.x();
    else
        tmpW = m_originRect.width() - delta.x();

    if(directY >= 0)
        tmpH = m_originRect.height() + delta.y();
    else
        tmpH = m_originRect.height() - delta.y();

    const int bottom_margin = MARGIN_SIZE;

    if(p_mainWidget->isPlayVideo())
    {
        // 1 2
        // 3 4
        //RightBottom
        /*x ->w
         *y ->h
         *              |
         * x - x>y?x:y  | x +
         * y -          | y -
        // ------------------------
        *  x -          | x + x>y?x:y
        *  y +          | y +
        *
        */
        //LeftBottom
        /*               |
         *   x - x>y?x:y |  x +
         *   y -         |  y -
        // ------------------------
        *    x -         |  x + x>y?x:y
        *    y +         |  y +
        */
        // Right Left 通过w算H dx = true;
        // Bottom Top 通过h算W dx = false;
        bool dx = false;
        if(_regionPressed == Right || _regionPressed == Left)
            dx = true;
        else if(_regionPressed == Bottom || _regionPressed == Top)
            dx = false;
        else if(_regionPressed == RightBottom)
            dx = (delta.x() * Global->_hRatio) > (delta.y() * Global->_wRatio);
        else if(_regionPressed == LeftBottom)
        {
            if(delta.x() * delta.y() <= 0)
            {
                if(delta.x() > 0)
                    dx = qAbs(delta.x() * Global->_hRatio) < qAbs(delta.y() * Global->_wRatio);
                else
                    dx = qAbs(delta.x() * Global->_hRatio) > qAbs(delta.y() * Global->_wRatio);
            }
            else if(delta.x() >= 0)
                dx = false;
            else
                dx = true;
        }
        if(dx) {
            //如果外控制栏显示 则加上它的高度
            tmpH = tmpW * Global->_hRatio / Global->_wRatio;
            if(p_controlWidget->isVisible())
            {
                tmpH += p_controlWidget->height() + bottom_margin;
            }
        } else {
            int subH = tmpH;
            if(p_controlWidget->isVisible())
            {
                subH -= p_controlWidget->height() + bottom_margin;
            }
            tmpW = subH * Global->_wRatio / Global->_hRatio;
        }
    }

    //宽度相关的判断
    if(tmpW < minWidth || tmpW > maxWidth) {
        tmpW = tmpW < minWidth ? minWidth : maxWidth;
        if(p_mainWidget->isPlayVideo())
        {
            tmpH = tmpW * Global->_hRatio / Global->_wRatio;
            if(p_controlWidget->isVisible())
            {
                tmpH += p_controlWidget->height() + bottom_margin;
            }
        }
    }

    //高度相关的判断
    //如果播放music
    if(p_mainWidget->isPlayMusic())
    {
        if(p_controlWidget->isVisible())
        {
            if(tmpH <  464 * _ratio + p_controlWidget->height() + bottom_margin)
                tmpH = 464 * _ratio + p_controlWidget->height() + bottom_margin;
        }
        else
        {
            if(tmpH < 464 * _ratio)
                tmpH = 464 * _ratio;
        }
    }
    if(tmpH < minHeight || tmpH > maxHeight) {
        tmpH = tmpH < minHeight ? minHeight : maxHeight;
        if(p_mainWidget->isPlayVideo())
        {
            int subH = tmpH;
            if(p_controlWidget->isVisible())
            {
                subH -= p_controlWidget->height() + bottom_margin;
            }
            tmpW = subH * Global->_wRatio / Global->_hRatio;
        }
    }


    if(p_mainWidget->isPlayVideo())
    {
        int h = tmpH;
        if(p_controlWidget->isVisible()) {
            h = tmpH - (p_controlWidget->height() + bottom_margin);
        }
        if(h < minHeight)
            tmpH = minHeight + (p_controlWidget->height() + bottom_margin);
        //if((tmpW < minWidth || tmpW > maxWidth) || (h > maxHeight  || h < minHeight))
        //    return;
    }
    if(tmpW < minWidth)
        tmpW = minWidth;
    else if(tmpW > maxWidth)
        tmpW = maxWidth;

    if(tmpH < minHeight)
        tmpH = minHeight;
    else if(tmpH > maxHeight)
        tmpH = maxHeight;


    int tmpX = m_originRect.x();
    int tmpY = m_originRect.y();

    if(directX < 0)
        tmpX +=  m_originRect.width() - tmpW;
    if(directY < 0)
        tmpY += m_originRect.height() - tmpH;

    this->setWidgetGeometry(tmpX, tmpY, tmpW, tmpH);
}

void PlayerMainWidget::startCursorTimer()
{
    stopCursorTimer();
    i_cursorTimerId = this->startTimer(50);
}

void PlayerMainWidget::stopCursorTimer()
{
    if (i_cursorTimerId != 0)
    {
        this->killTimer(i_cursorTimerId);
        i_cursorTimerId = 0;
    }
}

void PlayerMainWidget::setCursorSharpe(const PlayerMainWidget::WidgetRegion &regin)
{
    switch (regin)
    {
    case Top:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
        ::SetCursor(LoadCursor(NULL, IDC_SIZENS));
        break;
    case TopRight:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
        ::SetCursor(LoadCursor(NULL, IDC_SIZENESW));
        break;
    case Right:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
        ::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        break;
    case RightBottom:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
        ::SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
        break;
    case Bottom:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
        ::SetCursor(LoadCursor(NULL, IDC_SIZENS));
        break;
    case LeftBottom:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
        ::SetCursor(LoadCursor(NULL, IDC_SIZENESW));
        break;
    case Left:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
        ::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        break;
    case LeftTop:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
        ::SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
        break;
    case Inner:
    default:
        //QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
        ::SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;
    }
}

void PlayerMainWidget::updateLanguage()
{
    p_mainWidget->updateLanguage();
    p_controlWidget->updateLanguage();
    //更新菜单
    if(_trayMenu)
        Menu->upQActionText(_trayMenu);
    Ad->languageChange();
}

void PlayerMainWidget::updateUI(const qreal &scaleRatio)
{
    if(_ratio == scaleRatio)
        return;

    Menu->updateUI(scaleRatio);
    p_mainWidget->updateUI(scaleRatio);
    p_controlWidget->updateUI(scaleRatio);

    //判断是否超过最大 和 最小  --  需要等比缩放
    const qreal cw = this->width();
    const qreal ch = this->height();

    int w = this->width() * (scaleRatio / _ratio);
    int h = this->height()* (scaleRatio / _ratio);

    qDebug() << "当前大小:" << cw << "x" << ch << " - 计算后的大小:" << w << "x" << h;

    bestSize(w, h, scaleRatio);

    qDebug() << "最终合适大小:" <<  w << "x" << h;

    m_normalRect.setWidth(w);
    m_normalRect.setHeight(h);

    setWidgetSize(w, h);
    moveWidget(this->x()+(m_pressedPoint.x()*(1.0-w/cw)),
               this->y()+(m_pressedPoint.y()*(1.0-h/ch)));

    if(b_mousePressed)
    {
        //_pressedPoint *= scaleRatio / _ratio;
        m_pressedPoint = QPoint(m_pressedPoint.x() * w/cw, m_pressedPoint.y() * h/ch);
        m_originPosGlobal = this->mapToGlobal(m_pressedPoint);
        m_originRect = this->geometry();
    }

    _ratio = scaleRatio;

    setBorderWidth();

    OpenFileDialog->updateDialog2();
}
#include "httprequest.h"
void PlayerMainWidget::showControl(bool b)
{
    QString el("user");
    const QString uuid = Global->getUuid();
    if(!b)
    {
        el = "auto";
        //指定到某个屏幕
        this->recount(QPoint(10, 10));
    }
    else
    {
        this->recount(QCursor::pos());
    }

    if(Global->todayOnce(el)) {
        const QString url = QString("https://ssl.google-analytics.com/collect?"
                                    "ec=win-5KPlayer&ev=0&cid=%1&el=%2&tid=UA-58082298-1"
                                    "&t=event&v=1&ea=Open").arg(uuid).arg(el);
        Http->get(url);
    }
    this->setVisible(b);
    p_mainWidget->setVisible(b);

    this->showCenter();

    p_mainWidget->setGeometry(this->geometry());
}

WId PlayerMainWidget::request()
{
    if(p_videoWidget == NULL)
    {
        p_videoWidget = new QWidget(this);

        p_videoWidget->setPalette(this->palette());
        p_videoWidget->setAutoFillBackground(true);
        p_videoWidget->setMouseTracking(true);
        p_videoWidget->setGeometry(0,0,this->width(),this->height());
        p_videoWidget->raise();
    }

    p_videoWidget->setVisible(true);
    return p_videoWidget->winId();
}

void PlayerMainWidget::release()
{
    if (p_videoWidget) {
        p_videoWidget->setVisible(false);//防止stop后 还有一帧在界面上
    }
    b_mousePressed = false;//播放结束后  鼠标弹起  防止在播放中，按下鼠标操作，播放结束后bug
    //this->update();
}

void PlayerMainWidget::recount(const QPoint &point)
{
    changeWidgets();
}

void PlayerMainWidget::screenChanged(QObject *obj)
{
    changeWidgets();
}

void PlayerMainWidget::showCenter(bool b)
{

    const QRect rect = qApp->availableGeometry();

    int w = this->width();
    int h = this->height();
    if(!b) {
        w = NORMAL_WIDTH   * _ratio;
        h = NORMAL_HEIGHT  * _ratio;
    }

    const int cx = rect.x() + (rect.width() - w)/2;
    const int cy = rect.y() + (rect.height() - h)/2;

    moveWidget(cx, cy);

    m_normalRect.setX(cx);
    m_normalRect.setY(cy);
    m_normalRect.setWidth(w);
    m_normalRect.setHeight(h);
}

void PlayerMainWidget::showNormalCenter()
{
    const int cw = NORMAL_WIDTH * _ratio;
    const int ch = NORMAL_HEIGHT * _ratio;

    checkResetGeometry(cw, ch, true);
}

void PlayerMainWidget::showCurrentCenter(const int &w, const int &h, qreal ratio)
{

    ratio = (ratio == 0 ? _ratio : ratio);

    int cw = w * ratio;
    int ch = h * ratio;
    bestInitSize(cw, ch, _ratio);

    if(p_controlWidget->isVisible()) {
        ch += p_controlWidget->height() + MARGIN_SIZE;
    }

    checkResetGeometry(cw, ch, false);
}

void PlayerMainWidget::checkResetGeometry(int cw, int ch, const bool &b_full_reset)
{
    const QRect rect = qApp->availableGeometry();
    if(cw == rect.width() && ch == rect.height())
    {
        cw -= Global->_wRatio*2;
        ch -= Global->_hRatio*2;
    }
    int cx=0;
    int cy=0;
    const bool b_max = showMax();
#if 0
    cx = rect.x() + (rect.width() - cw)/2;
    cy = rect.y() + (rect.height() - ch)/2;
#elif 1

    if(b_max) {
        cx = m_normalRect.x() + (m_normalRect.width()  - cw) / 2;
        cy = m_normalRect.y() + (m_normalRect.height() - ch) / 2;
    } else {
        cx = this->x() + (this->width() - cw) / 2;
        cy = this->y() + (this->height() - ch) / 2;
    }
#endif
    if(cx < rect.x())
        cx = rect.x();
    else if(cw > rect.width())
        cx = rect.width()-cw;

    if(cx + cw > rect.x() + rect.width())
        cx = rect.x() + rect.width() - cw;

    if(cy < rect.y())
        cy = rect.y();
    else if(ch > rect.height())
        cy = rect.height() - ch;

    if(cy + ch > rect.y() + rect.height())
        cy = rect.y() + rect.height() - ch;

    m_normalRect.setRect(cx, cy, cw, ch);
    //ADD:应该检查是否在屏幕内  否则无法操作
    checkGeometry();
    if(b_full_reset) {
        if(!this->isHidden())
            this->showNormaled();
    } else {
        if(!b_max) {
            this->setWidgetGeometry(m_normalRect);
        }
    }
    OpenFileDialog->updateDialog2();//防止界面变化 弹出窗无变化的bug
}

void PlayerMainWidget::checkGeometry()
{
    const QRect rect = qApp->primaryScreen();
    const QPoint centerPoint = m_normalRect.center();
    if(rect.contains(centerPoint))
        return;

    const int t_x = centerPoint.x();
    const int t_y = centerPoint.y();
    // 九宫格区域判断
    // 1 2 3
    // 4 5 6
    // 7 8 9
    if(t_x < rect.x() && t_y < rect.y()) // 1 区域
    {
        m_normalRect.moveTo(rect.left() - m_normalRect.width() / 2,
                            rect.top() - m_normalRect.height() / 2);
    }
    else if(t_x > rect.topRight().x() && t_y < rect.topRight().y()) // 3 区域
    {
        m_normalRect.moveTo(rect.right() - m_normalRect.width() / 2,
                            rect.top() - m_normalRect.height() / 2);
    }
    else if(t_x < rect.x() && t_y > rect.y()) // 7 区域
    {
        m_normalRect.moveTo(rect.left() - m_normalRect.width() / 2,
                            rect.bottom() - m_normalRect.height() / 2);
    }
    else if(t_x > rect.bottomRight().x() && t_y > rect.bottomRight().y()) // 9区域
    {
        m_normalRect.moveTo(rect.right() - m_normalRect.width() / 2,
                            rect.bottom() - m_normalRect.height() / 2);
    }
    else if(t_y < rect.top()) // 2 区域
    {
        m_normalRect.moveTop(rect.top() - m_normalRect.height() / 2);
    }
    else if(t_x < rect.left()) // 4 区域
    {
        m_normalRect.moveLeft(rect.left() - m_normalRect.width() / 2);
    }
    else if(t_x > rect.right()) // 6 区域
    {
        m_normalRect.moveLeft(rect.right() - m_normalRect.width() / 2);
    }
    else if(t_y > rect.bottom()) // 8 区域
    {
        m_normalRect.moveTop(rect.bottom() - m_normalRect.height() / 2);
    }
}

void PlayerMainWidget::releaseMouse()
{
    b_mousePressed = false;

    b_isControlPressed = false;
    if(!p_mainWidget->_vrW->isHidden()) {
        const QPoint p = this->mapFromGlobal(p_mainWidget->_vrW->pos());
        if(p_mainWidget->_vrW->isVisible() &&
                p.y() < this->height() - p_controlWidget->height()*2)
        {
            p_mainWidget->resetControl(p);

            this->hidePlayControl();
            p_mainWidget->showControl();
            Global->setShowOutControl(false);
        }
        p_mainWidget->_vrW->hide();
    }

    p_mainWidget->releaseMouse();
}

void PlayerMainWidget::activateWidget()
{
    if(Global->winVersion() == 10) {
        this->activateWindow();
    } else {
        HWND id = (HWND)this->winId();
        AttachThreadInput(GetWindowThreadProcessId(GetForegroundWindow(), NULL),
                          GetCurrentThreadId(), true);
        SetForegroundWindow(id);
        SetFocus(id);
        AttachThreadInput(GetWindowThreadProcessId(GetForegroundWindow(), NULL),
                          GetCurrentThreadId(), false);
    }
}

QScreen *PlayerMainWidget::curScreen() const
{
    return QGuiApplication::primaryScreen();
}

void PlayerMainWidget::onTop() const
{
    ::SetWindowPos((HWND)this->winId(), HWND_TOPMOST, 0, 0, 0, 0,
                   SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
}

void PlayerMainWidget::offTop() const
{
    ::SetWindowPos((HWND)this->winId(), HWND_NOTOPMOST, 0, 0, 0, 0,
                   SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
}

void PlayerMainWidget::stopVlc(const bool &async)
{
    p_mainWidget->stopVlc(async);
}

//显示外部控制条  重置大小
void PlayerMainWidget::showPlayControl()
{
    if(p_controlWidget->isHidden()) {
        p_controlWidget->setVisible(true);
        p_controlWidget->raise();

        if(!this->showMax())
        {
            const int x = MARGIN_SIZE;
            p_controlWidget->setGeometry(x,  this->height(),
                                         this->width() - 2 * x,
                                         p_controlWidget->height());

            this->setWidgetSize(this->width(), this->height() + p_controlWidget->height() + x);
        }
    }
}

//隐藏 外部控制条  重置大小
void PlayerMainWidget::hidePlayControl()
{
    if(p_controlWidget->isVisible()) {
        p_controlWidget->setVisible(false);
        if(!this->showMax())
        {
            const int x = MARGIN_SIZE;
            this->setWidgetSize(this->width(), this->height() - p_controlWidget->height() - x);
        }
    }
}

void PlayerMainWidget::rotate()
{

    if(Global->_wRatio < 1 || Global->_hRatio < 1)
        return;

    int w = this->width();
    int h = this->height();
    if(this->showMax())
    {
        w = m_normalRect.width();
        h = m_normalRect.height();
    }

    //旋转后 出现黑边， 防止旋转回来还是有黑边
    if(w == MIN_WIDTH  * _ratio)
        h = w * Global->_wRatio / Global->_hRatio;
    else if(p_controlWidget->isVisible())
        h -= (p_controlWidget->height() + MARGIN_SIZE);

    showCurrentCenter(h, w, 1.0);
}

void PlayerMainWidget::stayAlwaysOnTop()
{
    Menu->updateStayOnTop(0);
    Global->setStayOnTop(CGlobal::Stay_Always);
    onTop();
}

void PlayerMainWidget::stayNeverOnTop()
{
    Menu->updateStayOnTop(2);
    Global->setStayOnTop(CGlobal::Stay_Never);
    offTop();
}

void PlayerMainWidget::stayPlayingOnTop()
{
    Menu->updateStayOnTop(1);
    Global->setStayOnTop(CGlobal::Stay_Playing);

    if(p_mainWidget->isPlayVideo()) {
        onTop();
    } else {
        offTop();
    }
}

void PlayerMainWidget::setPopDialog(const bool &b)
{
    b_popDialog = b;
}

void PlayerMainWidget::setTimerState(const bool &isStart)
{
    b_startTimer = isStart;
}

void PlayerMainWidget::showMainWidget()
{
    if(p_mainWidget == NULL)
        return;
    if(p_hideTimer->isActive()) {
        p_hideTimer->stop();
    }
    if(!this->isHidden())
    {
        p_mainWidget->setVisible(true);
        ::ShowCursor(TRUE);
        this->setCursor(Qt::ArrowCursor);
        //::SetCursor(LoadCursor(NULL, IDC_ARROW));//显示正常鼠标

        if(Global->showOutControl() && showMax() && !p_mainWidget->_playEnded)
        {
            p_controlWidget->setVisible(true);
            p_controlWidget->raise();
            /*
            p_videoWidget->setGeometry(0,0,
                                       this->width(),
                                       this->height() - p_controlWidget->height() - MARGIN_SIZE);
            */
        }
    }

    if(p_mainWidget->_head->rect().contains(this->mapFromGlobal(QCursor::pos())))
        return;

    if( !b_popDialog && b_startTimer && p_mainWidget->isPlayVideo() && /*_mw->_playerW->isVisible() &&*/ //现在改用vlc vout信号，抛弃以前先判断
            p_mainWidget->_head->isShowPlayerWidget() && //之前会出现  在library界面下 也会隐藏
            !(p_controlWidget->getMouseIn() || p_mainWidget->_playControl->getMouseIn()) ) {
        p_hideTimer->start();
    }
}

void PlayerMainWidget::hideMainWidget()
{
    if(b_popDialog)
        return;
    if(b_mousePressed) //鼠标按下 禁止隐藏
        return;
    if(p_mainWidget->isVisible())
    {
        p_mainWidget->setVisible(false);
        //隐藏鼠标指针
        ::ShowCursor(FALSE);
        this->setCursor(Qt::BlankCursor);

        if(Global->showOutControl() && showMax())
        {
            p_controlWidget->setVisible(false);
            //p_videoWidget->setGeometry(this->geometry());
            //p_videoWidget->resize(this->size());
            p_videoWidget->setGeometry(this->rect());
        }
    }
}

void PlayerMainWidget::ytbUpgradeTimer()
{
    p_ytbupTimer->stop();
    p_ytbupTimer->start(7 * ONEDAY_MSEC);

    QString     checkDate   = Global->getYtbLastCheckDate();
    QDateTime   curDate     = QDateTime::currentDateTime();
    const QDateTime lastDTime = QDateTime::fromString(checkDate, "yyyy-MM-dd");

    qint64 day = lastDTime.daysTo(curDate);
    // 设置为一周检查一次ytb的版本信息
    if(day > 7 || day < 0) {
        if(Global->getYtbdownFinish()) {
            if(!Global->_user_ytb)
                UpYTB->ytbExtract();
        } else {
            UpYTB->checkUpGrade(false);
        }
    }
}

void PlayerMainWidget::mainUpgradeTimer()
{
    p_mainUpTimer->stop();

    int  nextCheckTime = ONEDAY_MSEC;//默认每天
    bool upFlag = false;

    switch (Global->checkUpdateMode()) {
    case CGlobal::General_Update::Update_Always:
        upFlag = true;
        break;
    case CGlobal::General_Update::Update_Day:
        if(Global->checkDateForDay() >= 1)
            upFlag = true;
        break;
    case CGlobal::General_Update::Update_Week:
        if(Global->checkDateForWeek() >= 1)
        {
            nextCheckTime = 7 * ONEDAY_MSEC;
            upFlag = true;
        }
        break;
    case CGlobal::General_Update::Update_Month: //2.28 -> 3.1
        if(Global->checkDateForMonth() >= 1)
        {
            nextCheckTime = qPow(2, sizeof(int)*8-1)-1; //防止溢出: start参数int 最大 2147483647
            upFlag = true;
        }
        break;
    default:
        nextCheckTime = 0;
        break;
    }
    if(nextCheckTime > 0)
        p_mainUpTimer->start(nextCheckTime);

    if(upFlag) {
        autoCheckUpMain();
    }
}

void PlayerMainWidget::showNormaled()
{
    showMainWidget();//防止隐藏后全屏 不自动隐藏
    p_mainWidget->showNormal();
    this->showNormal();
    this->setWidgetGeometry(m_normalRect);
}

void PlayerMainWidget::showFullScreened()
{
    if(!this->showMax() && !p_mainWidget->_playEnded)
    {
        showMainWidget();//防止隐藏后全屏 不自动隐藏
        m_normalRect = this->geometry();
        this->showFullScreen();
        if(p_mainWidget->y() == qApp->primaryScreen()->geometry().y() &&
                p_mainWidget->x() == qApp->primaryScreen()->geometry().x())
            p_mainWidget->move(this->x(), this->y()+2);
        p_mainWidget->showFullScreen();
        setWidgetGeometry(this->geometry());
    }
}

static int bestValue(const int &v, const int &m, const bool &isAdd)
{
    if(m == 0)
        return v;

    if(v % m == 0)
        return v;

    if(isAdd)
        return v + m - (v % m);

    return v - (v % m);
}

//计算的是 视频显示的高度，不包含外部控制栏
void PlayerMainWidget::bestSize(int &w, int &h, const qreal &ratio)
{
    const int minWidth  = MIN_WIDTH  * ratio;
    const int minHeight = MIN_HEIGHT * ratio;

    QRect rect = qApp->primaryScreen();
    if(p_controlWidget->isVisible()) {
        rect.setHeight(rect.height() - p_controlWidget->height() - MARGIN_SIZE);
    }
    int cw = w, ch = h;
    const int rw = rect.width(); // *0.8
    const int rh = rect.height();// *0.8

    if(cw >= rw)
        cw = bestValue(rw, 4, false);
    else if(cw < minWidth)
        cw = bestValue(minWidth, 4, true);
    else
        cw = bestValue(cw, 4, true);

    if(ch >= rh)
        ch = bestValue(rh, 4, false);
    else if(ch < minHeight)
        ch = bestValue(minHeight, 4, true);
    else
        ch = bestValue(ch, 4, true);

    if(w > h)
    {
        ch = cw * h / w;
        if(ch < minHeight)
        {
            ch = minHeight;
            cw = ch * w / h;
            if(cw > rw)
                cw = rw;
        }
        else if(ch > rh)
        {
            ch = rh;
        }
    }
    else if(w < h)
    {
        cw = ch * w / h;
        if(cw < minWidth)
        {
            cw = minWidth;
            ch = cw * h / w;
            if(ch > rh)
                ch = rh;
        }
        else if(cw > rw)
        {
            cw = rw;
        }
    }
    else
    {
        cw = qMin(cw, ch);
        ch = cw;
    }

    w = cw;
    h = ch;
}

void PlayerMainWidget::bestInitSize(int &w, int &h, const qreal &ratio)
{

    const int minWidth  = MIN_WIDTH  * ratio;
    const int minHeight = MIN_HEIGHT * ratio;

    QRect rect = qApp->primaryScreen();
    if(p_controlWidget->isVisible()) {
        rect.setHeight(rect.height() - p_controlWidget->height() - MARGIN_SIZE);
    }

    const int rw = rect.width();
    const int rh = rect.height();
    int cw = NORMAL_WIDTH  * ratio, ch = NORMAL_HEIGHT  * ratio;
    if(cw > w)
        cw = w;
    if(ch > h)
        ch = h;

    if(w > h) //宽视频
    {
        ch = cw * h / w;
        if(ch < minHeight)
        {
            ch = minHeight;
            cw = ch * w / h;
            if(cw > rw)
                cw = rw;
        }
        else if(ch > rh)
        {
            ch = rh;
        }
    }
    else if(w < h) //高视频
    {
        cw = ch * w / h;
        if(cw < minWidth)
        {
            cw = minWidth;
            ch = cw * h / w;
            if(ch > rh)
                ch = rh;
        }
        else if(cw > rw)
        {
            cw = rw;
        }
    }
    else
    {
        cw = qMin(cw, ch);
        ch = cw;
    }
    w = cw;
    h = ch;
}
