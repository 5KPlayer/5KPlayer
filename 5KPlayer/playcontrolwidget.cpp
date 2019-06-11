#include "playcontrolwidget.h"
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>
#include <QDateTime>
#include "svgbutton.h"
#include <QDir>
#include <QResizeEvent>
#include "globalarg.h"
#include "dymediaplayer.h"
#include "cslider.h"
#include "cmenu.h"
#include <QLabel>
#include <QFileInfo>
#include <QtDebug>
#include <QDesktopServices>
#include "preview.h"
#include "ffmpeg/displayscreen.h"
#include "openfile.h"
#include "librarydata.h"
#include "DLna/controlmanager.h"
#include "DLna/devicemanager.h"
#include "playermain.h"
#include <QScreen>
#include <QApplication>

#ifdef Q_OS_MACX
#include "airplaywindow.h"
#endif

#define SPACE_SIZE 8
#define LR_MARGIN 10
#define FONT_SIZE 12

PlayControlWidget::PlayControlWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    _space    = SPACE_SIZE;
    _lrMargin = LR_MARGIN;
    _tbMargin = SPACE_SIZE;
    i_totalTime = 0;
    _multiview  = 0;
    _viewPoint  = 0;
    b_mouseIn   = false;
    b_handleVlc = true;

    _muteBtn      = new SvgButton(this);
    //_volumeMinBtn = new SvgButton(this);
    _volumeBarSlider  = new CSlider(Qt::Horizontal, this);
    _volumeBarSlider->setFocusPolicy(Qt::NoFocus);
    _volumnMaxBtn = new SvgButton(this);
    _preBtn       = new SvgButton(this);
    _playBtn      = new SvgButton(this);
    _nextBtn      = new SvgButton(this);
    _snapshotBtn  = new SvgButton(this);
    _playModeBtn  = new SvgButton(this);
    _viewPointBtn = new SvgButton(this);
    _editVideoBtn = new SvgButton(this);
    //_airBtn       = new SvgButton(this);

    //_muteBtn->setVisible(false);
    _muteBtn->initSize(18, 17);
    _muteBtn->setImagePrefixName("mute");

    //_volumeMinBtn->initSize(18, 17);
    //_volumeMinBtn->setImagePrefixName("shengyin1");

    _volumnMaxBtn->initSize(18, 17);
    _volumnMaxBtn->setImagePrefixName("volume");

    _preBtn->initSize(27, 19);
    _preBtn->setImagePrefixName("houtui");

    _playBtn->initSize(38, 32);
    //_playBtn->setToolTip("pause/play");
    _playBtn->setImagePrefixName("bofang"); //":/res/zanting"

    _nextBtn->initSize(27, 19);
    _nextBtn->setImagePrefixName("qianjin");

    _snapshotBtn->initSize(24, 15);
    _snapshotBtn->showAnimal();
    _snapshotBtn->setImagePrefixName("camera");

    _viewPointBtn->initSize(24,15);
    _viewPointBtn->setImagePrefixName("view");

    _editVideoBtn->initSize(24,15);
    _editVideoBtn->setImagePrefixName("edit");
    //记住关闭时的模式 然后初始化
    updatePlayModeImage();
    _playModeBtn->initSize(22, 15);
    //_playModeBtn->setImagePrefixName("Cycle");
    connect(_playModeBtn, SIGNAL(clicked()), SLOT(changePlayMode()));

//    _airBtn->initSize(22, 15);
//    _airBtn->setImagePrefixName("diannao");
//    _airBtn->changeState("diannao", false);
//    _airBtn->setToolTip("AirPlay");

    _volumeBarSlider->initSize(68, 16);//100 4
    _volumeBarSlider->setPageStep(4);
    _volumeBarSlider->setSingleStep(4);
    _volumeBarSlider->setRange(0,144);

    _curTime = new QLabel("00:00:00", this);
    _playSlider  = new CSlider(Qt::Horizontal, this);
    _playSlider->openMouseTrack();
    _playSlider->showRectHandle();
    _playSlider->setFocusPolicy(Qt::NoFocus);
    _totalTime = new QLabel("00:00:00", this);

    p_preview = new Preview(this);
    p_preview->hide();

    _playSlider->initSize(0, 16);
    _playSlider->setPageStep(1 * 1000);
    _playSlider->setSingleStep(3 * 1000);

    QPalette pa = _curTime->palette();
    pa.setColor(QPalette::WindowText, Qt::white);

    QFont f;
    f.setFamily(Global->getFontFamily());
    f.setPixelSize(FONT_SIZE);

    _curTime->setFont(f);
    _curTime->adjustSize();
    _curTime->setPalette(pa);
    _curTime->setStyleSheet("background:transparent");

    //_curVolLabel = new QLabel("100%", this);
    //_curVolLabel->setFont(f);
    //_curVolLabel->adjustSize();
    //_curVolLabel->setPalette(pa);
    //_curVolLabel->setStyleSheet("background:transparent");
    //_curVolLabel->setAlignment(Qt::AlignCenter);

    _totalTime->setFont(f);
    _totalTime->adjustSize();
    _totalTime->setPalette(pa);
    _totalTime->setStyleSheet("background:transparent");

    InitLanguge();

    connect(_playBtn,     SIGNAL(clicked()), SLOT(playClicked()));
    connect(_snapshotBtn, SIGNAL(clicked()), SLOT(snapshot()));
    connect(_snapshotBtn, SIGNAL(clicked_r()), SLOT(openSnapshotDir()));
    connect(_viewPointBtn,SIGNAL(clicked()), SLOT(clickviewPoint()));
    connect(_viewPointBtn,SIGNAL(clicked_r()),SLOT(clickRviewPoint()));
    connect(_editVideoBtn,SIGNAL(clicked()), SLOT(clickEidtBtn()));
    connect(&_previewThead,SIGNAL(sendPicture(QString,int64_t,QByteArray)),SLOT(showPreview(QString,int64_t,QByteArray)));
    connect(_preBtn, &SvgButton::clicked, [=](){
        PMW->playPre();
    });
    connect(_nextBtn, &SvgButton::clicked, [=](){
        PMW->playNext();
    });
//    connect(_airBtn, &SvgButton::clicked, [=](){
//        Menu->showAirMenu(cursor().pos());
//    });


    //静音
    connect(_muteBtn, &SvgButton::clicked, [=](){
        if(b_handleVlc) {
            PMW->toggleMute();
        } else {
            CONTROL->CtrlSetMute(!Global->_mute);
#ifdef Q_OS_MACX
            AirW->setAirplayVol(0);
#endif
            //UdpSev->sendMsgToConCPT("mute");
        }
    });

    //最小音量
    /*
    connect(_volumeMinBtn, &SvgButton::clicked, [=](){
        if(b_handleVlc) {
            //Global->setVolume(0);
            //MediaPlayer->audio()->setVolums(0);
            PMW->toggleMute();
        } else {
            //_volumeBarSlider->setValue(0);
            //UdpSev->sendMsgToConCPT("volume-0");
            UdpSev->sendMsgToConCPT("mute");
        }
    });*/

    //最大音量
    connect(_volumnMaxBtn, &SvgButton::clicked, [=](){
        if(b_handleVlc) {
            Global->setVolume(144);
            MediaPlayer->audio()->setVolums(144);
            Device->deviceVolumeChange(144);
        } else {
            _volumeBarSlider->setValue(144);
            CONTROL->CtlvolumeChange(100);
#ifdef Q_OS_MACX
            AirW->setAirplayVol(100);
#endif
            //UdpSev->sendMsgToConCPT("volume-144");
        }
    });

    //音量滑块
    connect(_volumeBarSlider, &CSlider::pressedValue, [=](int value){
        if(b_handleVlc) {
            Global->setVolume(value);
            MediaPlayer->audio()->setVolums(value);
            Device->deviceVolumeChange(value);
        } else {
            CONTROL->CtlvolumeChange(value);
            upCurVolume(value);
#ifdef Q_OS_MACX
            AirW->setAirplayVol(value);
#endif
            //UdpSev->sendMsgToConCPT(QString("volume-%1").arg(value));
        }
#ifdef Q_OS_MACX
        repaint();
#endif
    });

    connect(_volumeBarSlider, &QAbstractSlider::sliderMoved, [=](int value){
        if(b_handleVlc) {
            Global->setVolume(value);
            MediaPlayer->audio()->setVolums(value);
            Device->deviceVolumeChange(value);
        } else {
            CONTROL->CtlvolumeChange(value);
#ifdef Q_OS_MACX
            AirW->setAirplayVol(value);
#endif
            //UdpSev->sendMsgToConCPT(QString("volume-%1").arg(value));
        }
#ifdef Q_OS_MACX
        repaint();
#endif
    });

    //播放进度滑块
    connect(_playSlider, &CSlider::pressedValue, [=](int value){
        if(b_handleVlc) {
            MediaPlayer->setPosition(value);
            //如果是airplay-video 同步进度
        } else {
            CONTROL->positionChange(value);
            //UdpSev->sendMsgToConCPT(QString("seek-%1").arg(value));
        }
#ifdef Q_OS_MACX
        repaint();
#endif
    });

    connect(_playSlider, &QAbstractSlider::sliderMoved, [=](int value){
        if(b_handleVlc) {
            MediaPlayer->setPosition(value);
            //如果是airplay-video 同步进度
        } else {
            CONTROL->positionChange(value);
            //UdpSev->sendMsgToConCPT(QString("seek-%1").arg(value));
        }
#ifdef Q_OS_MACX
        repaint();
#endif
    });

    connect(_playSlider, &CSlider::curValue, [=](int value){
        //显示preview
        if(OpenFileDialog->curPlayType() == OPenFile::Video)
        {
            if(Global->_wRatio < 1 || Global->_hRatio < 1)
                return;

            const QString file = MediaPlayer->media()->curFile();

            _previewThead.setFilePathandTime(file,value);
            _previewThead.start();

            _previewFile= file;
            _previewTime= value;
            _previewPos = QCursor::pos();
        }
    });

    connect(_playSlider, SIGNAL(leave()), p_preview, SLOT(hide()));

    _originalWidth = 460;//540
    _originalHeight = 64;

    this->resize(_originalWidth, _originalHeight);
    this->setFocusPolicy(Qt::NoFocus);
}

void PlayControlWidget::updateLanguage()
{
    InitLanguge();
}

void PlayControlWidget::updateUI(const qreal &scaleRatio)
{
    QFont f;
    f.setFamily(Global->getFontFamily());
    f.setPixelSize(FONT_SIZE * scaleRatio);

    //_curVolLabel->setFont(f);
    //_curVolLabel->resize(56 * scaleRatio, 16 * scaleRatio);

    _curTime->setFont(f);
    _curTime->adjustSize();
    _totalTime->setFont(f);
    _totalTime->adjustSize();

    _space = SPACE_SIZE * scaleRatio;
    _lrMargin = LR_MARGIN * scaleRatio;
    _tbMargin = SPACE_SIZE * scaleRatio;
    _preBtn->updateUI(scaleRatio);
    _playBtn->updateUI(scaleRatio);
    _nextBtn->updateUI(scaleRatio);
    _snapshotBtn->updateUI(scaleRatio);
    _viewPointBtn->updateUI(scaleRatio);
    _editVideoBtn->updateUI(scaleRatio);
    //_airBtn->updateUI(scaleRatio);
    _playModeBtn->updateUI(scaleRatio);

    _volumeBarSlider->updateUI(scaleRatio);
    _playSlider->updateUI(scaleRatio);

    _muteBtn->updateUI(scaleRatio);
    //_volumeMinBtn->updateUI(scaleRatio);
    _volumnMaxBtn->updateUI(scaleRatio);

    this->resize(_originalWidth * scaleRatio, _originalHeight * scaleRatio);
}

void PlayControlWidget::ignoreLeaveEvent()
{
    _preBtn->ignoreLeaveEvent();
    _playBtn->ignoreLeaveEvent();
    _nextBtn->ignoreLeaveEvent();
    //_airBtn->ignoreLeaveEvent();
    _playModeBtn->ignoreLeaveEvent();
    _muteBtn->ignoreLeaveEvent();
    //_volumeMinBtn->ignoreLeaveEvent();
    _volumnMaxBtn->ignoreLeaveEvent();
    _snapshotBtn->ignoreLeaveEvent();
    _viewPointBtn->ignoreLeaveEvent();
    _editVideoBtn->ignoreLeaveEvent();
}

void PlayControlWidget::visibleSnapshotBtn(const bool &v)
{
    _snapshotBtn->setVisible(v);
    // 没有截图也肯定没有360视频按钮
    // 显示的时候判断是否本地文件
    if(LibData->playLocalMedia()) {
        _viewPointBtn->setVisible(v);
        _editVideoBtn->setVisible(v);
    } else {
        _viewPointBtn->setVisible(false);
        _editVideoBtn->setVisible(false);
    }
}

int PlayControlWidget::getCurTime() const
{
    return _playSlider->value();
}

void PlayControlWidget::changeAirplayState(const bool &/*b*/)
{
    //_airBtn->changeState("diannao", b);
}
#ifdef Q_OS_MACX
void PlayControlWidget::playSliderEnable(bool isMusie)
{
    _playSlider->setEnabled(isMusie);
}

#endif

void PlayControlWidget::handleVlc(const bool &b)
{
    b_handleVlc = b;
}

void PlayControlWidget::showPlayImage()
{
    _playBtn->setImagePrefixName("bofang");//目前处于暂停状态,点一下播放
}

void PlayControlWidget::showPauseImage()
{
    _playBtn->setImagePrefixName("zanting");//目前处于播放状态,点一下暂停
}

void PlayControlWidget::clearInfo()
{
    i_totalTime = 0;
    _curTime->setText("00:00:00");
    _playSlider->setValue(0);
    _playSlider->setRange(0, 0);
    _totalTime->setText("00:00:00");
    Global->_playCurTime    = "00:00:00";
    Global->_playTotalTime  = "00:00:00";
    showPlayImage();
}

void PlayControlWidget::setTotalTime(const int &t)
{
    if(t <= 0) return;
    i_totalTime = t;

    _totalTime->setText(Global->milliToTime(t));
    Global->_playTotalTime = Global->milliToTime(t);
    _playSlider->setRange(0, t);

//    if(b_handleVlc) {
//        //UdpSev->sendMsgToSerCPT(QString("total-%1").arg(t));
    //    }
}

void PlayControlWidget::showMuteBtn(const bool &b)
{
    if(b != Global->_mute) {
        Device->deviceMuteChange(Global->_mute);
    }
    Global->_mute = b;
    //_muteBtn->setVisible(b);
    //_volumeMinBtn->setVisible(!b);
    _volumeBarSlider->setEnabled(!b);
    _volumeBarSlider->updateState(!b);
    _volumnMaxBtn->setEnabled(!b);
}

void PlayControlWidget::snapshot()
{
    //获取当前播放电影名称
    if(!MediaPlayer->hasVout())
        return;
    QFileInfo fi(MediaPlayer->media()->curFile());
    const QString fname = fi.fileName();
    if(fname.isEmpty())
        return;
    QDir dir;
    const QString snapPath = Global->snapShotPath();
    dir.setPath(snapPath);//获取保持路径
    if(!dir.exists())
        if(!dir.mkpath(".")) //目录创建失败
            return;
#ifdef Q_OS_WIN
    const QString nextFile = QString("%1\\%2%3.jpg")
            .arg(QDir::toNativeSeparators(dir.path()))
            .arg(fname)
            .arg(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh-mm-ss.zzz]"));

    if(PMW->checkPMWInScreen()) { // PMW在屏幕内，使用qt截图
        takeScreenShot(nextFile);
    } else if(!MediaPlayer->playUserHead()) { // 软解播放，使用vlc截图
        MediaPlayer->video()->takeSnapshot(nextFile, PMW->width(), PMW->height());
    } else { // vlc硬解播放截图会崩溃，这里使用ffmpeg截图
        _snapThread.setParams(MediaPlayer->media()->curFile(),nextFile,
                             getCurTime(),PMW->width(), PMW->height());
        _snapThread.start();
    }
#else
    const QString nextFile = QString("%1/%2%3.jpg")
            .arg(QDir::toNativeSeparators(dir.path()))
            .arg(fname)
            .arg(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh-mm-ss.zzz]"));

    /*if(PMW->checkPMWInScreen()) { // PMW在屏幕内，使用qt截图
        takeScreenShot(nextFile);
    } else {*/ // 使用vlc截图
    MediaPlayer->video()->takeSnapshot(nextFile, PMW->width(), PMW->height());
    //}
#endif
}
#ifdef Q_OS_WIN
void PlayControlWidget::takeScreenShot(QString fileName)
{
    QScreen *screen = QApplication::primaryScreen();
    PMW->mainWidget()->setVisible(false);   //截屏时，隐藏主窗口
    QImage pix = screen->grabWindow(0,PMW->x(),PMW->y(),PMW->getWidget()->width(),PMW->getWidget()->height()).toImage();

    pix.save(fileName,nullptr,100);
    PMW->mainWidget()->setVisible(true);    //截屏完成，显示主窗口
}
#endif
void PlayControlWidget::hidePreview()
{
    if(p_preview)
        p_preview->hide();
    _previewThead.stopffmpeg();
    _previewTime = 0;
    _previewFile = "";
}

void PlayControlWidget::setCurTime_t(const int &t)
{
    if(t * 1000 > i_totalTime)
        return;
    _curTime->setText(Global->secondToTime(t));
    Global->_playCurTime = Global->secondToTime(t);
    if(!_playSlider->isSliderDown())
    {
        _playSlider->setValue(t*1000);
    }
#ifdef Q_OS_MACX
    repaint();
#endif
}

void PlayControlWidget::setCurTime(const float &t, const bool &b)
{
    if(!b && b_handleVlc)
    {
        MediaPlayer->setPosition(t);
        return;
    }
#ifdef Q_OS_MACX
    AirW->setVideoPosition(t);
#endif
    const int &curT = t * i_totalTime;
    _curTime->setText(Global->milliToTime(curT));
    Global->_playCurTime = Global->milliToTime(curT);
    if(!_playSlider->isSliderDown())
    {
        _playSlider->setValue(curT);
//        if(b_handleVlc) {
//            UdpSev->sendMsgToSerCPT(QString("seek-%1").arg(t));
//        }
    }
#ifdef Q_OS_MACX
    repaint();
#endif
}

void PlayControlWidget::playClicked(const bool &play, const bool &b)
{
    if(b_handleVlc) { //客户端主动点
        if(MediaPlayer->state() == DYVlc::Paused) {
            Device->devicePlayChange();
        } else {
            Device->devicePaushChange();
        }
        PMW->togglePause(true);
        return;
    }
    //服务端主动点击, 只管发送给客户端
    //客户端主动点击/收到数据， 服务端只变化状态
    if(b && !b_handleVlc) { //服务端主动点
        CONTROL->playClick();
        //UdpSev->sendMsgToConCPT("play");
        PMW->togglePause(false);
        return;
    }
    if(play) {
        showPlayImage();
    }
    else {
        showPauseImage();
    }
}

void PlayControlWidget::setCurVolume(const int &v, const bool &b)
{
    if(!b && b_handleVlc) //客户端收到网络数据
    {
        MediaPlayer->audio()->setVolums(v);
    }
    else if(!_volumeBarSlider->isSliderDown())
    {
        const int v = MediaPlayer->audio()->sliderValue();
        _volumeBarSlider->setValue(v);
    }

    //_curVolLabel->setText(QString("%1%").arg(MediaPlayer->audio()->realVolume()));
}

void PlayControlWidget::upCurVolume(const int &v)
{
    _volumeBarSlider->setValue(v);
}

void PlayControlWidget::resizeEvent(QResizeEvent *e)
{
    const int w = e->size().width();
    const int h = e->size().height();

    //_volumeMinBtn->move(_lrMargin, _tbMargin + (h/2 - _volumeMinBtn->height())/2);
    _muteBtn->move(_lrMargin, _tbMargin + (h/2 - _muteBtn->height())/2);

    _volumeBarSlider->move(_muteBtn->x() + _muteBtn->width() + _space,
                           _tbMargin + (h/2-_volumeBarSlider->height())/2);

    //_curVolLabel->move(_volumeBarSlider->x() + (_volumeBarSlider->width() - _curVolLabel->width())/2,
    //                   _volumeBarSlider->y() - _curVolLabel->height() + 4);

    _volumnMaxBtn->move(_volumeBarSlider->x() + _volumeBarSlider->width() + _space,
                        _tbMargin + (h/2 - _volumnMaxBtn->height())/2);

    _playBtn->move(w/2-_playBtn->width()/2,
                   _tbMargin + (h/2-_playBtn->height())/2);

    _preBtn->move(_playBtn->x() - _space - _preBtn->width(),
                  _tbMargin + (h/2-_nextBtn->height())/2);

    _nextBtn->move(_playBtn->x() + _playBtn->width() + _space,
                   _tbMargin + (h/2-_nextBtn->height())/2);

#if 0
    _snapshotBtn->move(w- _lrMargin - _snapshotBtn->width() - 2,
                       _tbMargin + (h/2-_snapshotBtn->height())/2);
    _playModeBtn->move(_snapshotBtn->x() - _playModeBtn->width() - _space * 2, _snapshotBtn->y());
#else
    _playModeBtn->move(w- _lrMargin - _playModeBtn->width(),
                       _tbMargin + (h/2-_playModeBtn->height())/2);
    _snapshotBtn->move(_playModeBtn->x() - _snapshotBtn->width() - _space * 2, _playModeBtn->y());
    _viewPointBtn->move(_snapshotBtn->x() - _viewPointBtn->width() - _space * 2, _playModeBtn->y());
    _editVideoBtn->move(_viewPointBtn->x() - _editVideoBtn->width() - _space * 2,_playModeBtn->y());
#endif
//    _airBtn->move(w- _lrMargin - _airBtn->width(),
//                  _tbMargin + (h/2-_airBtn->height())/2);

    _curTime->move(_lrMargin, h/2 + (h/2 - _curTime->height())/2);

    _totalTime->move(w-_lrMargin-_totalTime->width(),
                     h/2 + (h/2 - _totalTime->height())/2);

    _playSlider->setFixedWidth(_totalTime->x() - _curTime->x() - _curTime->width() - 2*_space);

    _playSlider->move(_curTime->x() + _curTime->width() + _space,
                      h/2 + (h/2 - _playSlider->height())/2);

    e->accept();
}

void PlayControlWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}

void PlayControlWidget::enterEvent(QEvent *)
{
    b_mouseIn = true;
    PMW->showMainWidget();
}

void PlayControlWidget::leaveEvent(QEvent *)
{
    b_mouseIn = false;
    p_preview->hide();
    PMW->showMainWidget();
}

void PlayControlWidget::InitLanguge()
{
    _snapshotBtn->setToolTip(Lge->getLangageValue("ToolTip/snapshot"));
    _nextBtn->setToolTip(Lge->getLangageValue("ToolTip/playNext"));
    _preBtn->setToolTip(Lge->getLangageValue("ToolTip/playPre"));
    _volumnMaxBtn->setToolTip(Lge->getLangageValue("ToolTip/volumeMax"));
    _muteBtn->setToolTip(Lge->getLangageValue("ToolTip/volumemute"));
    _viewPointBtn->setToolTip(Lge->getLangageValue("ToolTip/view"));
    updatePlayModeImage();
}

bool PlayControlWidget::checkMousePoint(QPoint pos)
{
    bool isMouse = false;
    if((_previewPos.x() <= pos.x() + 5 * Global->_screenRatio && _previewPos.x()+ 5 * Global->_screenRatio >= pos.x()) || (_previewPos.x() == pos.x())) {
        isMouse = true;
    }
    if((_previewPos.y() <= pos.y() + 5 * Global->_screenRatio && _previewPos.y()+ 5 * Global->_screenRatio >= pos.y()) || (_previewPos.y() == pos.y() && isMouse)) {
        isMouse = true;
    } else {
        isMouse = false;
    }
    return isMouse;
}

void PlayControlWidget::changViewPoint()
{
    int proMode = LibData->getCurPlayPorjectMode();
    if(!_viewPointBtn->getState()) {
        _viewPointBtn->changeState("view",true);
        _viewPoint = 1;
        if(this->isVisible())
            MediaPlayer->upViewPoint(1);
    } else {
        _viewPointBtn->changeState("view",false);
        _viewPoint = 0;
        if(this->isVisible())
            MediaPlayer->upViewPoint(0);
    }
    if(proMode != 1 && this->isVisible()) {
        MEDIAINFO playMedia = LibData->getPlayMediaInfo();
        if(!playMedia.filepath.isEmpty())
            OpenFileDialog->openFile(playMedia.filepath);
    }
}

void PlayControlWidget::changMultiview(QString mult)
{
    int oldMult = _multiview;
    if(mult == "view2D") {
        _multiview = 0;
    } else if(mult == "view3DLR") {
        _multiview = 1;
    } else if(mult == "view3DTB") {
        _multiview = 2;
    }

    if(oldMult != _multiview) {
        //改变mult的值
        MediaPlayer->upViewMult(_multiview);
        // 强制360°按下状态，，重新播放文件
        if(_viewPointBtn->getState() && this->isVisible()) {
            MEDIAINFO playMedia = LibData->getPlayMediaInfo();
            if(!playMedia.filepath.isEmpty())
                OpenFileDialog->openFile(playMedia.filepath);
        }
    }
}

void PlayControlWidget::clearViewPoint()
{
    _viewPointBtn->changeState("view",false);
    MediaPlayer->upViewPoint(0);
    _viewPoint = 0;
}

bool PlayControlWidget::getViewPointState()
{
    return _viewPointBtn->getState();
}

void PlayControlWidget::upViewPointState()
{
    if(this->isVisible()) {
        MediaPlayer->upViewPoint(_viewPoint);
        MediaPlayer->upViewMult(_multiview);
    }
}

void PlayControlWidget::changePlayMode()
{
    int playMode = Global->getPlayMode();
    playMode++;
    if(playMode < 1 || playMode>4)
        playMode = 1;

    Global->setPlayMode(playMode);
    LibData->musicPlayStatus(LibData->PlayModel);
    PMW->changePlayModeImage();
}

void PlayControlWidget::openSnapshotDir()
{
    QString url = "file:///";
    url.append(QDir::toNativeSeparators(Global->snapShotPath()));
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

void PlayControlWidget::showPreview(QString path, int64_t time, QByteArray data)
{
    if(Global->_wRatio < 1 || Global->_hRatio < 1)
        return;
    const QPoint p = QCursor::pos();
    if(path == _previewFile && _previewTime == time && checkMousePoint(p)) {
        int w = 180 * Global->_screenRatio;
        if(Global->_wRatio < Global->_hRatio)//防止长视频过大
            w = w / 2;
        const int h = w * Global->_hRatio / Global->_wRatio;
        if(data.length() < 1)
            return;
        if(!p_preview->showImage(data, time, w, h))
            return;
        const int x = p.x() - p_preview->width() / 2;
        const int y = p.y() - p_preview->height() - 12;
        p_preview->move(x, y);
        p_preview->show();
    }
}

void PlayControlWidget::clickviewPoint()
{
    PMW->changControlViewState();
}

void PlayControlWidget::clickRviewPoint()
{
    Menu->showViewMenu(cursor().pos());
}

void PlayControlWidget::clickEidtBtn()
{
    int editFlay = LibData->getCurPlayHasEdit();
    if(editFlay == 0) {
        OpenFileDialog->openEditMediaDialog();
    } else if(editFlay == 1) {
        OpenFileDialog->mediaNoSupEditDialog(1);
    } else if(editFlay == 2) {
        OpenFileDialog->mediaNoSupEditDialog(2);
    }
}

void PlayControlWidget::updatePlayModeImage()
{
    int playMode = Global->getPlayMode();
    if(playMode < 1 || playMode>4)
        playMode = 1;

    if(playMode == 1) {
        _playModeBtn->setImagePrefixName("one");
        _playModeBtn->setToolTip(Lge->getLangageValue("ToolTip/modeOnce"));
    }if(playMode == 2) {
        _playModeBtn->setImagePrefixName("single");
        _playModeBtn->setToolTip(Lge->getLangageValue("ToolTip/modeOne"));
    } else if (playMode == 3) {
        _playModeBtn->setImagePrefixName("Cycle");
        _playModeBtn->setToolTip(Lge->getLangageValue("ToolTip/modeall"));
    } else if(playMode == 4){
        _playModeBtn->setImagePrefixName("random");
        _playModeBtn->setToolTip(Lge->getLangageValue("ToolTip/moderand"));
    }
}

