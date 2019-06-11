#include "mainwidget.h"
#include <QPalette>
#include <QDir>
#include <QTimer>
#include <QLocale>
#include <QMouseEvent>
#include "virtuallinewidget.h"
#include "airplay.h"
#include "openninginfowidget.h"
#include "cpausewidget.h"
#include "headwidget.h"
#include "librarywidget.h"
#include "playcontrolwidget.h"
#include "openmodewidget.h"
#include "svgbutton.h"
#include "dymediaplayer.h"
#include "playermainwidget.h"
#include "librarydata.h"
#include "openfile.h"
#include "cmenu.h"
#include "globalarg.h"
#include "playmusicwidget.h"
#include "transeventwidget.h"
#include <QtDebug>
#include "udpserver.h"
#include "../Library/ffmpeg/displayscreen.h"

#define SPACE_SIZE 20

MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    _margin = 4;
    i_curPostion= 0;
    b_handleVlc = true;
    _initVolume = false;
    _spacing = SPACE_SIZE;

    p_timer = NULL;
    p_airPlay = NULL;

    b_airplayMusic = false;
    b_airplayVideo = false;

    /*
    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    this->setPalette(pal);
    */

    setAttribute(Qt::WA_TranslucentBackground);//设置透明

    init();
    _openSateW = new OpenningInfoWidget(this);
    _openSateW->setVisible(false);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    this->setWindowState(Qt::WindowActive);
    setModal(false);
}

void MainWidget::updateLanguage()
{
    if(_libW != NULL)
        _libW->updateLanguage();
    _head->updateLanguage();
    _openW->updateLanguage();
    _playControl->updateLanguage();
}

void MainWidget::updateUI(const qreal &scaleRatio)
{
    _margin = 4 * scaleRatio;
    _spacing = SPACE_SIZE * scaleRatio;

    _head->updateUI(scaleRatio);
    _openW->updateUI(scaleRatio);
    p_musicW->updateUI(scaleRatio);
    _playControl->updateUI(scaleRatio);
    _playControl->setStyleSheet(QString("border-radius: %1px; background: rgba(5,5,5,100);").arg(8*scaleRatio));
    _closePlay->updateUI(scaleRatio);
    if(_libW != NULL)
        _libW->updateUI(scaleRatio);

    //_pauseW->updateUI(scaleRatio);
}

void MainWidget::showControl()
{
    _playControl->setVisible(true);
}

void MainWidget::resetControl(const QPoint &pos)
{
    _isResetControl = true;
    _resetPos = pos - QPoint(0, headHeight());
}

int MainWidget::headHeight() const
{
    return _head->height();
}

void MainWidget::initMediaplayerSignals()
{
    connect(MediaPlayer, SIGNAL(opening()), SLOT(vlcOpening()));
    connect(MediaPlayer, SIGNAL(playing()), SLOT(vlcPlaying()));
    connect(MediaPlayer, SIGNAL(paused()),  SLOT(vlcPaused()));
    connect(MediaPlayer, SIGNAL(stopped()), SLOT(vlcStoped()));
    connect(MediaPlayer, SIGNAL(end()),     SLOT(vlcEnded()));
    connect(MediaPlayer, SIGNAL(error()),   SLOT(vlcEnded()));//发生错误不一定会结束
    connect(MediaPlayer, SIGNAL(positionChanged(float)), _playControl, SLOT(setCurTime(float)));
    connect(MediaPlayer, SIGNAL(lengthChanged(int)),     _playControl, SLOT(setTotalTime(int)));
    connect(MediaPlayer, SIGNAL(titleChanged(int)), Menu, SLOT(resetTitleMenu(int)));
    connect(MediaPlayer, SIGNAL(vout(int)), SLOT(vlcVout(int)));
    connect(MediaPlayer->audio(), SIGNAL(volumeChanged(int)), SLOT(changeVolumeSlider(int)));
    //更新2个控制栏的mute状态
    connect(MediaPlayer->audio(), SIGNAL(muteChanged(bool)), SLOT(showMuteBtn(bool)));
    connect(MediaPlayer->video(), SIGNAL(changeRatio()), SLOT(changeRatio()));

    connect(UdpSev,  SIGNAL(controlSignal(QString)), SLOT(handleUDPInfo(QString)));

    connect(Menu, SIGNAL(updateShowState()), _head, SLOT(updateShowMenusState()));
}

bool MainWidget::airplayStarted()
{
    if(p_airPlay)
        return p_airPlay->airplayStarted();
    return false;
}

bool MainWidget::startAirPlay()
{
    if(!Global->macAddress().isEmpty())
    {
        if(p_airPlay == NULL)
        {
            p_airPlay = new AirPlay("5KPlayer-" + Global->localName(), Global->macAddress(), 8080, this);

            connect(p_airPlay, SIGNAL(signal_screenMirrorStart(int)),     SLOT(airplayMirrorStart(int)));
            connect(p_airPlay, SIGNAL(signal_screenRecordFinish(QString)),SLOT(airplayRecordFinish(QString)));

            connect(p_airPlay, SIGNAL(signal_audioStart()),              SLOT(airplayAudioStart()));
            connect(p_airPlay, SIGNAL(signal_audioDestroy()),            SLOT(airplayAudioDestroy()));
            connect(p_airPlay, SIGNAL(signal_audioPause()),              SLOT(airplayAudioPause()));
            connect(p_airPlay, SIGNAL(signal_audioPlay(int,int)),        SLOT(airplayAudioPlay(int,int)));
            connect(p_airPlay, SIGNAL(signal_audioImage(QByteArray)),    SLOT(airplayAudioImage(QByteArray)));
            connect(p_airPlay, SIGNAL(signal_audioMetadata(QString)),    SLOT(airplayAudioMetadata(QString)));
            connect(p_airPlay, SIGNAL(signal_audioVolume(int)),          SLOT(airplayAudioVolume(int)));

            connect(p_airPlay, SIGNAL(signal_videoPlay(QString)),        SLOT(airplayVideoStart(QString)));
            connect(p_airPlay, SIGNAL(signal_videoControl(QString)),     SLOT(airplayVideoControl(QString)));
            connect(p_airPlay, SIGNAL(signal_videoSeek(double)),         SLOT(airplayVideoSeek(double)));
        }

        if(p_airPlay->startAirPlay())
        {
            p_airPlay->setMirrorRecordPath(Global->mirrorRecordPath());

            Global->setAirplayStarted(true);

            return true;
        }
    }
    return false;
}

void MainWidget::stopAirPlay()
{
    if(p_airPlay) {
        Global->setAirplayStarted(false);
        p_airPlay->stopAirPlayServer();
    }
}

void MainWidget::releaseMouse()
{
    _isPressed = false;
    if(!_vrW->isHidden()) {
        //允许误差 50-40
        if(!PMW->showMax() &&
                _playerW->mapFromGlobal(_vrW->pos()).y() >
                _playerW->height() - _playControl->height() * 3 / 5)
        {
            _playControl->hide();
            Global->setShowOutControl(true);
            //显示底部控制栏
            PMW->showPlayControl();
        }
        _vrW->hide();
    }
}

bool MainWidget::isPlayVideo()
{
    if(_playEnded)
        return false;

    //现在判断视频是根据 vlc  vout信号
    if(Global->_wRatio < 1 || Global->_hRatio < 1)
        return false;

    if(OpenFileDialog->curPlayType() != OPenFile::Video &&
            OpenFileDialog->curPlayType() != OPenFile::DVD &&
            OpenFileDialog->curPlayType() != OPenFile::DVDMirror)
        return false;

    return true;
}

bool MainWidget::isPlayMusic()
{
    if(_playEnded)
        return false;
    if(OpenFileDialog->curPlayType() == OPenFile::Music ||
       OpenFileDialog->curPlayType() == OPenFile::Live) //音乐和live一样
        return true;
    return false;
}

bool MainWidget::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == _playControl)
    {
        switch(e->type())
        {
        case QEvent::MouseButtonPress:
        {
            _isPressed = true;
            _pressPos = static_cast<QMouseEvent *>(e)->pos();
            e->accept();
            return true;
        }
        case QEvent::MouseMove:
        {
            if(_isPressed)
            {
                const QPoint curPos = static_cast<QMouseEvent *>(e)->pos();
                QPoint tmp = curPos - _pressPos;

                const QRect rect = _playerW->geometry();

                int x = _playControl->x() + tmp.x();
                int y = _playControl->y() + tmp.y();

                if(x < 0)
                    x = 0;
                else if(x + _playControl->width() > rect.width())
                    x = rect.width() - _playControl->width();

                int vr_y = y;         //虚线框的Y坐标
                bool showVRW = false; //是否显示虚线框

                if(y < 0)
                {
                    y = 0;
                }
                else if(y + _playControl->height() > rect.height())
                {
                    showVRW = true;
                    vr_y = y;
                    y = rect.height() - _playControl->height();
                }

                if(showVRW && !PMW->showMax())
                {
                    if(vr_y > rect.height())
                        vr_y = rect.height();
                    if(_vrW->isHidden()) {
                        _vrW->resize(_playerW->width(), _playControl->height());
                        //_vrW->setLineColor(Qt::blue);
                        _vrW->setLineColor(QColor(255,170,51));
                        _vrW->show();
                    }
                    QPoint tmpPos = _playerW->mapToGlobal(QPoint(0, vr_y));
                    _vrW->move(tmpPos.x(), tmpPos.y());
                }
                else
                {
                    _vrW->hide();
                }

                _playControl->move(x, y);
                _xRatio = ((qreal)x)/(_playerW->width() - _playControl->width());
                _yRatio = ((qreal)y)/(_playerW->height() - _playControl->height());
            }
            e->accept();
            return true;
        }
        case QEvent::MouseButtonRelease:
        {
            releaseMouse();
            e->accept();
            return true;
        }
        case QEvent::WindowDeactivate:
        {
            _isPressed = false;
            _vrW->hide();
            e->accept();
            return true;
        }
        default: break;
        }
    }
    return QDialog::eventFilter(obj, e);
}

void MainWidget::showMainWidget()
{
    if(_libW != NULL)
        _libW->setVisible(false);
    _playerW->setVisible(true);

    if(p_musicW->isVisible()) {
        p_musicW->upWidgtet();
    }

    _head->showControl(isPlayVideo());
    this->setFocus();
}

void MainWidget::showLibraryWidget()
{
    if(_libW != NULL) {
        _libW->setVisible(true);
    } else {
        _libW = new LibraryWidget(this);//library界面
        connect(_openW, SIGNAL(itemClicked(QString)), _libW, SIGNAL(itemChange(QString)));
        const int w = this->geometry().width();
        const int h = this->geometry().height();
        const int headH = 44 * Global->_screenRatio;

        _libW->setGeometry(0, headH, w, h-headH+4*Global->_screenRatio);
        _libW->updateUI(Global->_screenRatio);
        _libW->updateLanguage();
        _libW->setVisible(true);
    }
    _playerW->setVisible(false);
    _head->showControl(false);
    this->setFocus();
}

//显示打开方式界面
void MainWidget::showOpenModeWidget()
{
    _head->showControl(false);

    _openW->setVisible(true);

    _closePlay->setVisible(false);
    _playControl->setVisible(false);
}

void MainWidget::changeVolumeSlider(int v)
{
    _playControl->setCurVolume(v);
    PMW->p_controlWidget->setCurVolume(v);

    UdpSev->sendMsgToSerCPT(QString("volume-%1").arg(v));
}

void MainWidget::showMuteBtn(const bool &b)
{
    Menu->muteMenu(b);
    _playControl->showMuteBtn(b);
    PMW->p_controlWidget->showMuteBtn(b);

    //UdpSev->sendMsgToSerCPT(QString("mute-%1").arg(b?1:0));
}

void MainWidget::resizeEvent(QResizeEvent *e)
{
    _openSateW->setGeometry(this->rect());
    //重新布局 子控件 的 坐标
    const int w = this->width();
    const int h = this->height();
    const int headH = 44 * Global->_screenRatio;

    _head->setGeometry(0, 0, w, headH);

    _playerW->setGeometry(0, headH, w, h-headH);

    if(_libW != NULL) {
        _libW->setGeometry(0, headH, w, h-headH+4*Global->_screenRatio);
    }

    //_pauseW->setGeometry(_playerW->rect());

    _closePlay->move(w - _closePlay->width() - _spacing, _spacing);

    _openW->setGeometry(0, 0, w, h-headH);

    p_musicW->setGeometry(0, 0, 360*Global->_screenRatio, 360*Global->_screenRatio);
    p_musicW->move((w - p_musicW->width())/2, (h - headH - p_musicW->height())/2);

    //让 _playControl 等比缩放移动
    if(_isResetControl)
    {
        _isResetControl = false;
        _playControl->move(_resetPos);

        _xRatio = ((qreal)_playControl->x())/(_playerW->width() - _playControl->width());
        _yRatio = ((qreal)_playControl->y())/(_playerW->height() - _playControl->height());
    }
    else
    {
        _playControl->move((_playerW->width()-_playControl->width()) * _xRatio,
                           (_playerW->height()-_playControl->height()) * _yRatio);
    }
    //    _playControl->move((w - _playControl->width()) / 2,
    //                       h - headH - _playControl->height() - 5);

    e->accept();
}

void MainWidget::init()
{
    _playEnded = true;
    b_playingState = false;
    _isResetControl = false;
    _head = new HeadWidget(this);   //标题栏
    _playerW = new QWidget(this);
    _playerW->setAttribute(Qt::WA_TranslucentBackground);
    _libW = NULL;//library界面

    p_musicW = new PlayMusicWidget(_playerW);

    //_pauseW = new CPauseWidget(_playerW);
    _openW = new OpenModeWidget(_playerW);//打开方式界面
    _closePlay = new SvgButton(_playerW);//关闭正在播放界面
    _closePlay->initSize(19, 19);
    _closePlay->setImagePrefixName("sub_close");
    _playControl = new PlayControlWidget(_playerW);//播放控制界面
    _playControl->setStyleSheet("border-radius: 8px; background: rgba(5,5,5,100);");

    _xRatio = 0.50f;
    _yRatio = 0.98f;

    _vrW = new VirtualLineWidget(this);
    _vrW->hide();

    _playControl->installEventFilter(this);

    //_pauseW->setVisible(false);
    _playerW->setVisible(true);
    p_musicW->setVisible(false);
    _openW->setVisible(true);
    _playControl->setVisible(false);
    _closePlay->setVisible(false);
    //_libW->setVisible(false);

    connect(_openW, SIGNAL(itemClicked(QString)), SLOT(openWInitClick(QString)));
    connect(_openW, SIGNAL(itemClicked(QString)), _head, SLOT(clickLeftitem(QString)));

    connect(_head, SIGNAL(showPlayerW()),   SLOT(showMainWidget()));
    connect(_head, SIGNAL(showLibraryW()),  SLOT(showLibraryWidget()));

    connect(_closePlay,  SIGNAL(clicked()), SLOT(stopVlc()));
}

int MainWidget::chooseBestAudio()
{
    const QString name = QLocale::system().bcp47Name();

    QList<Audio *> dtsList;
    QList<Audio *> list6;

    //筛选Audio所有信息
    foreach (Audio *audio, MediaPlayer->mediaInfo()->audioList())
    {
        if(audio->_basic->language.trimmed().toLower() == "dts")
            dtsList << audio;

        if(audio->i_channels == 6)
            list6 << audio;
    }

    //DTS + 本地语言
    foreach (Audio *audio, dtsList) {
        if(audio->_basic->language.trimmed().toLower().startsWith(name.toLower())) {
            return audio->_basic->i_id;
        }
    }

    //6ch + 本地语言
    foreach (Audio *audio, list6) {
        if(audio->_basic->language.trimmed().toLower().startsWith(name.toLower())) {
            return audio->_basic->i_id;
        }
    }

    //DTS + English
    foreach (Audio *audio, dtsList) {
        if(audio->_basic->language.trimmed().toLower().startsWith("en")) {
            return audio->_basic->i_id;
        }
    }
    //DTS
    foreach (Audio *audio, dtsList) {
        return audio->_basic->i_id;
    }

    //6ch + English
    foreach (Audio *audio, list6) {
        if(audio->_basic->language.trimmed().toLower().startsWith("en")) {
            return audio->_basic->i_id;
        }
    }

    // 本地语言
    foreach (Audio *audio, MediaPlayer->mediaInfo()->audioList()) {
        if(audio->_basic->language.trimmed().toLower().startsWith(name.toLower())) {
            return audio->_basic->i_id;
        }
    }

    // 英语
    foreach (Audio *audio, MediaPlayer->mediaInfo()->audioList()) {
        if(audio->_basic->language.trimmed().toLower().startsWith("en")) {
            return audio->_basic->i_id;
        }
    }

    return -1;
}

void MainWidget::showOpenStateWidget()
{
    if(OpenFileDialog->curPlayType() == OPenFile::NetWork ||
            OpenFileDialog->curPlayType() == OPenFile::CONNECTION) {
        _openSateW->setVisible(true);
    }
}

void MainWidget::hideOpenStateWidget()
{
    _openSateW->setVisible(false);
}

void MainWidget::showPlayControl()
{    //外部控制栏
    if(Global->showOutControl())
    {
        _playControl->setVisible(false);
        PMW->showPlayControl();
    }
    else
    {
        _playControl->setVisible(true);
        PMW->hidePlayControl();
    }
}

void MainWidget::hidePlayControl()
{
    PMW->hidePlayControl();
    _playControl->setVisible(false);
}

void MainWidget::vlcVout(const int &count) //视频流  DVD VIDEO RADIO
{
    if(_playEnded) return;//防止出现end在前的信号顺序问题

    Menu->resetTrackMenu();

    if(count < 1) return;

    MediaPlayer->video()->setAspeceRatio("");

    //显示截图按钮
    _playControl->visibleSnapshotBtn(true);
    PMW->p_controlWidget->visibleSnapshotBtn(true);

    _head->enableRotate(true);//开启旋转

    //双击播放切换  自动切换 不切换界面
    if(LibData->b_doubleClickPlay) {
        _head->clickPlayerW();//自动切换文件时 ？？
    }
    if(_head->isShowPlayerWidget())
        _head->showControl(true);

    //    const QPoint point = this->mapFromGlobal(QCursor::pos());
    //    if(!this->rect().contains(point)) {
    //        PMW->hideMainWidget();
    //    }

    //隐藏music界面
    p_musicW->setVisible(false); //防止音频分析错误当视频
    if(OpenFileDialog->curPlayType() == OPenFile::Music ||
            OpenFileDialog->curPlayType() == OPenFile::Live) //防止ffmpeg解析错误
    {
        OpenFileDialog->setPlayType(OPenFile::Video);
    }

    const int mw = PMW->minimumWidth();
    const int mh = PMW->minimumHeight();
    int ch=0;
    int cw=0;
//    Global->_wRatio = 0;
//    Global->_hRatio = 0;

    if(OpenFileDialog->curPlayType() == OPenFile::DVD ||
            OpenFileDialog->curPlayType() == OPenFile::DVDMirror) {
        qDebug() << "play DVD";
        if(MediaPlayer->mediaInfo()->videoCount() > 0) {
            Global->_wRatio = MediaPlayer->mediaInfo()->videoList().at(0)->i_dar_w;
            Global->_hRatio = MediaPlayer->mediaInfo()->videoList().at(0)->i_dar_h;
            cw = MediaPlayer->mediaInfo()->videoList().at(0)->i_width;
            qDebug() << "vlc arg:" << Global->_wRatio << Global->_hRatio << cw;
        }
        if(Global->_wRatio <= 0 || Global->_hRatio <= 0) {
            Global->_wRatio = 4;
            Global->_hRatio = 3;
            qDebug() << "未找到合适的比例,使用默认比例 4:3.";
        }
        if(cw <= 0)
            cw = 720;
        if(cw < mw)
            cw = mw;

        ch = cw * Global->_hRatio / Global->_wRatio;
        if(ch < mh)
        {
            ch = mh;
            cw = ch * Global->_wRatio / Global->_hRatio;
        }
    } else {
        qDebug() << "play Video" << "ch:" <<ch <<"mh:"<< mh
                 << Global->_wRatio << Global->_hRatio;
        ch = mh;
        MEDIAINFO info = LibData->getplayinfo();
        if(MediaPlayer->mediaInfo()->videoCount() > 0) {
            Global->_wRatio = MediaPlayer->mediaInfo()->videoList().at(0)->i_dar_w;
            Global->_hRatio = MediaPlayer->mediaInfo()->videoList().at(0)->i_dar_h;
            if(info.rotate == 90 || info.rotate == 270) {
                Global->rotate();
            }
            ch = MediaPlayer->mediaInfo()->videoList().at(0)->i_height;
            qDebug() << "vlc 参数: dar:" << Global->_wRatio << ":" << Global->_hRatio << " 高度:" << ch;
        }

        if(Global->_wRatio <= 0 || Global->_hRatio <= 0 ) {
            QStringList al = LibData->getVideoStream(info).aspectRatio.split(":", QString::SkipEmptyParts);
            if(al.count() == 2) {
                Global->_wRatio = al.at(0).toInt();
                Global->_hRatio = al.at(1).toInt();
                ch = LibData->getVideoStream(info).resolution.height();
            }
            qDebug() << "ffmpeg 参数: dar:" << Global->_wRatio << ":" << Global->_hRatio << " 高度:" << ch;
        }
        if(ch <= 0)
            ch = 405;

        if(Global->_wRatio <= 0 || Global->_hRatio <= 0 ) {
            Global->_wRatio = 4;
            Global->_hRatio = 3;
            qDebug() << "未找到合适的比例,使用默认比例 4:3.";
        }
        //----如果分辨率大于1920 * 1080 禁用旋转
        Global->b_openPreview = true;
        if(ch * ch * Global->_wRatio / Global->_hRatio > 1920*1080)
        {
            Global->b_openPreview = false;
            //_head->enableRotate(false);
        }

        qDebug() << "ch*mh" << ch << mh;
        if(ch < mh)
            ch = mh;

        cw = ch * Global->_wRatio / Global->_hRatio;
        if(cw < mw)
        {
            cw = mw;
            ch = cw * Global->_hRatio / Global->_wRatio;
        }
    }

    qDebug() << "最终参数:" << cw << "x" << ch;

    Global->_w = cw;
    Global->_h = ch;

    PMW->showCurrentCenter(cw, ch);

    Menu->enableVideoMenu();

    //音频适宜选择
    const int id = chooseBestAudio();
    if(id != -1)
        Menu->setAudioTrackId(id);

    if(Global->stayOnTop() == CGlobal::General_Stay::Stay_Playing)
        PMW->onTop();

    //防止切换后  不自动隐藏
    PMW->setTimerState(true);
    PMW->showMainWidget();


    Global->_wOriginalRatio = Global->_wRatio;
    Global->_hOriginalRatio = Global->_hRatio;
}

void MainWidget::openWInitClick(QString item)
{
    if(_libW == NULL)
    {
        _libW = new LibraryWidget(this);//library界面
        connect(_openW, SIGNAL(itemClicked(QString)), _libW, SIGNAL(itemChange(QString)));
        connect(this, SIGNAL(ItemInitClick(QString)), _libW, SIGNAL(itemChange(QString)));

        const int w = this->geometry().width();
        const int h = this->geometry().height();
        const int headH = 44 * Global->_screenRatio;

        _libW->setGeometry(0, headH, w, h-headH+4*Global->_screenRatio);
        _libW->updateUI(Global->_screenRatio);
        _libW->updateLanguage();
        _libW->setVisible(true);
    }
    emit ItemInitClick(item);
}

//----------------------------airplay相关处理-----------------------------//
void MainWidget::airplayMirrorStart(int state)
{
    if(state == 0)
    {
        PMW->showOut();
    }
    else
    {
        OpenFileDialog->closeDialog();
        if(Global->stayOnTop() == CGlobal::General_Stay::Stay_Playing)
            PMW->offTop();

        PMW->hidePlayControl();
        this->hide();
        PMW->hide();

        stopVlc(false);
    }
}

void MainWidget::airplayRecordFinish(const QString &file)
{
    LibData->addmedia(file, "pAirRecord", true);//添加到library -> airplay record列表
}

//audio
void MainWidget::airplayAudioStart()
{
    stopVlc();
}

void MainWidget::airplayAudioPause()
{
    //现在暂停状态
    _playControl->playClicked(true, false);
    PMW->p_controlWidget->playClicked(true, false);
    if(p_timer)
        p_timer->stop();
}

void MainWidget::airplayAudioVolume(const int &v)
{
    _playControl->setCurVolume(v, true);
    PMW->p_controlWidget->setCurVolume(v, true);
}

void MainWidget::airplayAudioImage(const QByteArray &data)
{
    if(b_airplayMusic)
        p_musicW->setPlayImage(data);
}

void MainWidget::airplayAudioMetadata(const QString &title)
{
    if(b_airplayMusic)
        p_musicW->setPlayTitle(title);
}

void MainWidget::airplayAudioDestroy()
{
    if(b_airplayMusic) {
        b_airplayMusic = false;

        _openW->setVisible(true);
        p_musicW->setVisible(false);
        _closePlay->setVisible(false);
        OpenFileDialog->resetType();

        _playControl->handleVlc(true);
        PMW->p_controlWidget->handleVlc(true);

        hidePlayControl();

        if(p_timer)
            p_timer->stop();
    }
}

void MainWidget::airplayAudioPlay(int start, int end)
{
    b_airplayMusic = true;
    if(p_timer)
        p_timer->stop();

    _openW->setVisible(false);
    p_musicW->setVisible(true);
    _closePlay->setVisible(true);
    OpenFileDialog->setPlayType(OPenFile::AIRPLAY_MUSIC);
    //现在播放状态
    _playControl->handleVlc(false);
    PMW->p_controlWidget->handleVlc(false);

    _playControl->playClicked(false, false);
    PMW->p_controlWidget->playClicked(false, false);

    //隐藏截图按钮
    _playControl->visibleSnapshotBtn(false);
    PMW->p_controlWidget->visibleSnapshotBtn(false);

    showPlayControl();

    if(start < end) {
        _playControl->setTotalTime(end*1000);
        PMW->p_controlWidget->setTotalTime(end*1000);
        //启动定时器 设置当前的进度
        i_timer = start;
        if(p_timer == NULL) {
            p_timer = new QTimer(this);
            p_timer->setInterval(1000);
            connect(p_timer, &QTimer::timeout, [=](){
                _playControl->setCurTime_t(i_timer);
                PMW->p_controlWidget->setCurTime_t(i_timer);
                i_timer++;
            });
        }
        p_timer->start();
    }

    PMW->showOut();
}

//video
void MainWidget::airplayVideoStart(const QString &url)
{
    b_airplayVideo = true;

    OpenFileDialog->setPlayType(OPenFile::NetWork);
    MediaPlayer->playFile(url, false);

    PMW->showOut();
}

void MainWidget::airplayVideoSeek(const double &seek)
{
    if(b_airplayVideo) {
        _playControl->setCurTime(seek, false);
        PMW->p_controlWidget->setCurTime(seek, false);
    }
}

void MainWidget::airplayVideoControl(const QString &state)
{
    if(state == "stop") {
        b_airplayVideo = false;
        stopVlc();
    } else if(state == "play") {
        if(MediaPlayer->state() == DYVlc::Paused)
            MediaPlayer->togglePause();
    } else if(state == "pause") {
        if(MediaPlayer->state() == DYVlc::Playing)
            MediaPlayer->togglePause();
    }
}

//暂时不考虑 本地正在播放状态  不考虑airplay-music方式
void MainWidget::joinConnectMode()
{
    b_handleVlc = false;
    _openW->setVisible(false);

    //重置控制栏

    showPlayControl();

    _closePlay->setVisible(true);
    _playControl->handleVlc(false);
    PMW->p_controlWidget->handleVlc(false);

    //隐藏截图按钮
    _playControl->visibleSnapshotBtn(false);
    PMW->p_controlWidget->visibleSnapshotBtn(false);
}

void MainWidget::closeConnectMode()
{
    b_handleVlc = true;
    _openW->setVisible(true);

    hidePlayControl();

    _closePlay->setVisible(false);
    _playControl->handleVlc(true);
    PMW->p_controlWidget->handleVlc(true);
}

void MainWidget::handleUDPInfo(const QString &msg)
{
    const QStringList list = msg.split('-');
    if(list.count() < 1)
        return;

    if(list[0] == "seek") {
        if(list.count() > 1)
        {
            _playControl->setCurTime(list[1].toFloat(), false);
            PMW->p_controlWidget->setCurTime(list[1].toFloat(), false);
        }
        if(i_curPostion != 0)
        {
            UdpSev->sendMsgToConCPT(QString("seek-%1").arg(i_curPostion));
            i_curPostion = 0;
        }

    } else if(list[0] == "total") {
        if(list.count() > 1)
        {
            _playControl->setTotalTime(list[1].toInt());
            PMW->p_controlWidget->setTotalTime(list[1].toInt());
        }
    } else if(list[0] == "volume") {
        if(list.count() > 1)
        {
            _playControl->setCurVolume(list[1].toInt(), false);
            PMW->p_controlWidget->setCurVolume(list[1].toInt(), false);
        }
    } else if(list[0] == "pause") {
        _playControl->playClicked(true, false);
        PMW->p_controlWidget->playClicked(true, false);
    } else if(list[0] == "play") {
        _playControl->playClicked(false, false);
        PMW->p_controlWidget->playClicked(false, false);
    } else if(list[0] == "end") { //终止  包含:stop end error
        if(b_handleVlc) {//客户端
            stopVlc();
        } else {//服务端
            i_curPostion = 0;
            LibData->clearPlayinfo();
            closeConnectMode();
        }
    } else if(list[0] == "connected") {
        //if(b_handleVlc) return;
        //b_handleVlc = false;

        _head->changeAirplayState(true);
        //_playControl->changeAirplayState(true);
        //PMW->p_controlWidget->changeAirplayState(true);

        //记住播放文件  记住播放进度
        if(_playEnded) return;

        i_curPostion  = _playControl->getCurTime();
        const QString curFile  = LibData->getplayinfo().filepath;

        MEDIAINFO info = LibData->getplayinfo();

        stopVlc();
        b_handleVlc = false;

        if(LibData->checkMediaisAirPlay(info))
        {
            OpenFileDialog->setPlayType(OPenFile::CONNECTION);
            OpenFileDialog->sendNetworkInfo(curFile);
        } else {
            QTimer::singleShot(100, [curFile](){
                OpenFileDialog->openMediaNotSupAirPlay(curFile);
            });
        }
    } else if(list[0] == "disconnected") {
        _head->changeAirplayState(false);
        //_playControl->changeAirplayState(false);
        //PMW->p_controlWidget->changeAirplayState(false);
    }
}
//------------------------airplay相关操作 end-------------------------------//

void MainWidget::vlcOpening()
{
    PMW->setTimerState(false);
    PMW->showMainWidget();
    //if(PMW->showMax())  //防止全屏播放时，外部控制栏在屏幕外部
    //    PMW->showNormal();

    PMW->releaseMouse();
    //openning状态（VLC阻塞） - 显示提示信息
    //showOpenStateWidget();
    p_musicW->setVisible(false);

    //显示播放按钮
    _playControl->showPlayImage();
    PMW->p_controlWidget->showPlayImage();

    //隐藏截图按钮
    _playControl->visibleSnapshotBtn(false);
    PMW->p_controlWidget->visibleSnapshotBtn(false);

    if(_playEnded)
    {
        _playEnded = false;
        _openW->setVisible(false);
        _closePlay->setVisible(false);
    }
    b_playingState = false;

    Global->_wRatio = Global->_hRatio = 0;
    Global->_wOriginalRatio = Global->_hOriginalRatio = 0;

    _head->showControl(false);

    showPlayControl();

}

void MainWidget::vlcPlaying()
{
    PMW->releaseMouse();
    //开始播放 - 隐藏提示
    //hideOpenStateWidget();

    //显示暂停按钮
    _playControl->showPauseImage();
    PMW->p_controlWidget->showPauseImage();

    //暂停->播放 会出现bug
    if(!b_playingState) {

        //读取配置表，设置音量
        if(!_initVolume) {
            _initVolume = true;
            MediaPlayer->audio()->setVolums(Global->volume());
        }

        Menu->resetMenu();
        Menu->enableAudioMenu();

        _closePlay->setVisible(true);

        b_playingState = true;
    }

    //隐藏loading
    //_pauseW->setVisible(false);

    PMW->setTimerState(true);
    PMW->showMainWidget();

    if((OpenFileDialog->curPlayType() == OPenFile::Music ||
            OpenFileDialog->curPlayType() == OPenFile::Live)/* ||
            (OpenFileDialog->curPlayType() == OPenFile::Video && !MediaPlayer->hasVout())*/)
    {
        if(PMW->p_controlWidget->isVisible()) //控制播放music的最低高度
        {
            if(PMW->height() < 468 * PMW->_ratio + PMW->p_controlWidget->height())
                PMW->setWidgetSize(PMW->width(), 468 * PMW->_ratio + PMW->p_controlWidget->height());
        }
        else
        {
            if(PMW->height() < 464 * PMW->_ratio)
                PMW->setWidgetSize(PMW->width(), 464 * PMW->_ratio);
        }
        switch (OpenFileDialog->curPlayType()) {
        /*case OPenFile::Video:*/
        case OPenFile::Music:
            p_musicW->setPlayMedia(LibData->getplayinfo(), false);
            break;
        case OPenFile::Live:
            p_musicW->setLiveInfo(Global->liveUrl(), Global->liveName());
            break;
        default:
            break;
        }

        p_musicW->upWidgtet();
        p_musicW->setVisible(true);
    }
    UdpSev->sendMsgToSerCPT("play");
}

void MainWidget::vlcPaused()
{
    //显示播放按钮
    _playControl->showPlayImage();
    PMW->p_controlWidget->showPlayImage();

    //显示loading
    //_pauseW->setVisible(true);
    PMW->setTimerState(false);
    PMW->showMainWidget();

    UdpSev->sendMsgToSerCPT("pause");
}

void MainWidget::vlcStoped()
{
    PMW->releaseMouse();

    _playControl->clearInfo();
    PMW->p_controlWidget->clearInfo();

    if(!_playEnded)
    {
        _playEnded = true;

        b_playingState = false;

        Global->_w = Global->_h = 0;
        Global->_wRatio = Global->_hRatio = 0;
        Global->_wOriginalRatio = Global->_hOriginalRatio = 0;
        Global->b_openPreview = false;
        OpenFileDialog->resetType();
        //关闭preview
        //DisScreen->closeScreen();

        _openW->openAnimal(false);
        PMW->setTimerState(false);
        PMW->showMainWidget();

        if(Global->stayOnTop() == CGlobal::General_Stay::Stay_Playing && !PMW->isHidden())
            PMW->offTop();

        //_pauseW->setVisible(false);
        PMW->release();
        p_musicW->setVisible(false);
        showOpenModeWidget();

        //如果当前是5KPlayer界面,恢复原始大小
        PMW->hidePlayControl();

        if(_head->isShowPlayerWidget()) {//5Kplayer 界面
            qApp->processEvents();
            PMW->showNormalCenter();
            PMW->repaint();//强制重绘 防止主界面缺省
        }

        LibData->clearPlayinfo();

        UdpSev->sendMsgToSerCPT("end");

        LibData->recScreenprotection();
        _openW->openAnimal(true);
    }
}

void MainWidget::vlcEnded()
{
    PMW->releaseMouse();
    PMW->release();

    //防止切换播放  preview没隐藏
    _playControl->hidePreview();
    PMW->p_controlWidget->hidePreview();

    //播放异常 - 隐藏提示
    if(OpenFileDialog->curPlayType() == OPenFile::NetWork ||
            OpenFileDialog->curPlayType() == OPenFile::CONNECTION) {
        //hideOpenStateWidget();
        MediaPlayer->stop();
        return;
    }

    p_musicW->setVisible(false);
    Menu->resetMenu();
    MediaPlayer->rotate()->closeFlip();
    MediaPlayer->rotate()->closeRotate();

    LibData->b_doubleClickPlay = false;

    if(PMW->isMinimized() && OpenFileDialog->curPlayType() != OPenFile::Music) {
        vlcStoped();
        //PMW->release();
        return;
    }

    MEDIAINFO info;
    if(Global->getPlayMode() != 1) {
        info = LibData->getnextmedia(true);
    }
    if(info.title.isEmpty())
    {
        vlcStoped();
        //PMW->release();
    }
    else
    {
        if(info.mediaType == 1)
            OpenFileDialog->setPlayType(OPenFile::Video);
        else if(info.mediaType == 2)
            OpenFileDialog->setPlayType(OPenFile::Music);
        else {
            OpenFileDialog->resetType();
            return;
        }
        if(OpenFileDialog->curPlayType() != OPenFile::Music)
            PMW->showOut();
        OpenFileDialog->playFile(info.filepath);
    }
}

void MainWidget::stopVlc(const bool &async)
{
    if(b_airplayMusic) {
        p_airPlay->stopAirPlay();
        //p_airPlay->sendMusicControl(4);
        return;
    }
    if(b_airplayVideo) {
        p_airPlay->sendVideoControl(4);
        return;
    }

    if(!b_handleVlc) //服务端
    {
        UdpSev->sendMsgToConCPT("end");
        return;
    }

    if(MediaPlayer->state() == DYVlc::Opening)
        return;

    vlcStoped();
    Menu->resetMenu();
    MediaPlayer->rotate()->closeRotate();
    LibData->b_doubleClickPlay = false;

    //防止切换播放  preview没隐藏
    _playControl->hidePreview();
    PMW->p_controlWidget->hidePreview();

    if(!async)
        MediaPlayer->stop();
    else
        QTimer::singleShot(100, MediaPlayer, SLOT(stop()));

    //#warning(播放在线资源的时候,如果处于opening,调用stop界面会死)
}

void MainWidget::changeRatio()
{
    int cw = 0;
    int ch = 0;
    if(PMW->width() < PMW->height())
    {
        if(PMW->showMax())
            cw = PMW->originalGeometry().width();
        else
            cw = PMW->width();
        ch = cw * Global->_hRatio / Global->_wRatio;
    }
    else
    {
        if(PMW->showMax())
            ch = PMW->originalGeometry().height();
        else
            ch = PMW->height();
        cw = ch * Global->_wRatio / Global->_hRatio;
    }
    Global->_w = cw;
    Global->_h = ch;
    PMW->showCurrentCenter(cw, ch);
}

