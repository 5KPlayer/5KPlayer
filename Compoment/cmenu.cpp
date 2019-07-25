#include "cmenu.h"
#include "openfile.h"
#include "playermainwidget.h"
#include "librarydata.h"
#include "dymediaplayer.h"
#include <QtDebug>
#include <QMenu>
#include <QFileInfo>
#include <QHostInfo>
#include <QTimer>
#include <QActionGroup>
#include "airmenuwidget.h"
#include "udpserver.h"
#include "globalarg.h"
CMenu::CMenu(QObject *parent) : QObject(parent)
{
    _showCount = 0;
    initMenuGroup();
    initSubMenu();
    initSetMenu();
    initRightMenu();
    initAirPlayMenu();
    initRecentFiles();
    resetMenu();
    connect(LibData,SIGNAL(upUdpClientList()),SLOT(resetAirMenu()));
}

CMenu::~CMenu()
{
    _fileMenu->close();
    clearMenu(_fileMenu);
    delete _fileMenu;

    _helpMenu->close();
    clearMenu(_helpMenu);
    delete _helpMenu;

    _langMenu->close();
    clearMenu(_langMenu);
    delete _langMenu;

    _setAc->disconnect();
    delete _setAc;

    _setMenu->close();
    clearMenu(_setMenu);
    delete _setMenu;

    _airMenu->close();
    clearMenu(_airMenu);
    delete _airMenu;

    _rightMenu->close();
    clearMenu(_rightMenu);
    delete _rightMenu;

    _audioMenu->close();
    clearMenu(_audioMenu);
    delete _audioMenu;

    _videoMenu->close();
    clearMenu(_videoMenu);
    delete _videoMenu;

    _chapterMenu->close();
    clearMenu(_chapterMenu);
    delete _chapterMenu;
}

//初始化菜单组
void CMenu::initMenuGroup()
{
    _audioMenuGroup = new QActionGroup(this);
    _audioMenuGroup->setExclusive(true);

    _airMenuGroup   = new QActionGroup(this);
    _airMenuGroup->setExclusive(true);

    _videoMenuGroup = new QActionGroup(this);
    _videoMenuGroup->setExclusive(true);

    _titleMenuGroup = new QActionGroup(this);
    _titleMenuGroup->setExclusive(true);

    _chapterMenuGroup = new QActionGroup(this);
    _chapterMenuGroup->setExclusive(true);

    _subtitleMenuGroup = new QActionGroup(this);
    _subtitleMenuGroup->setExclusive(true);

    _deinterlaceGroup = new QActionGroup(this);
    _deinterlaceGroup->setExclusive(true);

    _aspectRatioGroup = new QActionGroup(this);
    _aspectRatioGroup->setExclusive(true);

    _equalizerMenuGroup = new QActionGroup(this);
    _equalizerMenuGroup->setExclusive(true);

    _stereoModeMenuGroup = new QActionGroup(this);
    _stereoModeMenuGroup->setExclusive(true);

    _stayGroup = new QActionGroup(this);
    _stayGroup->setExclusive(true);

    _languageGroup = new QActionGroup(this);
    _languageGroup->setExclusive(true);
}

//菜单初始化
bool CMenu::audioMenuIsEnable()
{
    return _audioMenu->isEnabled();
}

bool CMenu::subtitleMenuIsEnable()
{
    return _subtitleMenu->isEnabled();
}

bool CMenu::chapterMenuIsEnable()
{
    return _chapterMenu->isEnabled();
}

void CMenu::setAudioTrackId(const int &id)
{
    if(id < 0)
        return;

    if(MediaPlayer->audio()->track() == id)
        return;

    MediaPlayer->audio()->setTrack(id);

    clearMenu(_audioMenu);
    initAudioMenu();
}

void CMenu::updateLanguage()
{
    updateUI(Global->_screenRatio);
    _audioMenu->setTitle(Lge->getLangageValue("Menu/track"));
    _videoMenu->setTitle(Lge->getLangageValue("Menu/track"));
    _chapterMenu->setTitle(Lge->getLangageValue("Menu/chapter"));
    _subtitleMenu->setTitle(Lge->getLangageValue("Menu/subtitle"));

    _rVideoMenu->setTitle(Lge->getLangageValue("Menu/video"));
    _rAudioMenu->setTitle(Lge->getLangageValue("Menu/audio"));
    _rStayMenu->setTitle(Lge->getLangageValue("Menu/stay"));
    _rDeinterlaceMenu->setTitle(Lge->getLangageValue("Menu/deinter"));
    _rAspectRatio->setTitle(Lge->getLangageValue("Menu/ratio"));
    _rEqualizerMenu->setTitle(Lge->getLangageValue("Menu/equalizer"));
    _rStereoModeMenu->setTitle(Lge->getLangageValue("Menu/stereo"));

    clearMenu(_rStereoModeMenu);
    initStereoMenu();

    _titleMenu->setTitle(Lge->getLangageValue("Menu/title"));
    _recentMenu->setTitle(Lge->getLangageValue("Menu/recent"));
    _fileMenu->setTitle(Lge->getLangageValue("Menu/file"));
    // 更新_fileMenu中的QAction
    upQActionText(_fileMenu);

    _helpMenu->setTitle(Lge->getLangageValue("Menu/help"));
    // 更新_helpMenu中的QAction
    upQActionText(_helpMenu);

    _langMenu->setTitle(Lge->getLangageValue("Menu/language"));

    _setAc->setText(Lge->getLangageValue("Menu/set"));
    _hwaccelAc->setText(Lge->getLangageValue("Menu/hw"));

    _rPlayMenu->setTitle(Lge->getLangageValue("Main/play"));
    // 更新_rPlayMenu中的QAction
    upQActionText(_rPlayMenu);

    _rTransformMenu->setTitle(Lge->getLangageValue("Menu/trans"));
    // 更新_rTransformMenu中的QAction
    upQActionText(_rTransformMenu);

    //音频
    _muteAc->setText(Lge->getLangageValue("Menu/mute"));
    _addVolumeAc->setText(Lge->getLangageValue("Menu/voladd"));
    _decVolumeAc->setText(Lge->getLangageValue("Menu/voldec"));

    // 更新_rStayMenu中的QAction
    upQActionText(_rStayMenu);

    _openFile_set->setText(Lge->getLangageValue("Menu/OFile"));
    _openFile_right->setText(Lge->getLangageValue("Menu/OFile"));
    _syncAc->setText(Lge->getLangageValue("Menu/synon"));
    _fullAc->setText(Lge->getLangageValue("Menu/fuscr"));
    _convert->setText(Lge->getLangageValue("Menu/convt"));
    _infoAc->setText(Lge->getLangageValue("Menu/minfo"));
    _clearRecent->setText(Lge->getLangageValue("Menu/clearM"));
    if(_tuner)
        _tuner->setText(Lge->getLangageValue("Menu/tuner"));
    if(_subtitleMenu->actions().count() > 0)
        _loadSub->setText(Lge->getLangageValue("Menu/ldsub"));

    _relation->setText(Lge->getLangageValue("PlayMain/relation"));
}

void CMenu::resetMenu()
{
    if(p_timer)
        p_timer->stop();
    b_isInitVideoMenu = false;
    b_isInitMusicMenu = false;
    clearMenu(_audioMenu);
    clearMenu(_videoMenu);
    clearMenu(_chapterMenu);
    clearMenu(_subtitleMenu);
    clearMenu(_titleMenu);

    if(_rAspectRatio && _rAspectRatio->actions().count() > 0)
        _rAspectRatio->actions().at(0)->setChecked(true);

    if(_rDeinterlaceMenu && _rDeinterlaceMenu->actions().count() > 0)
        _rDeinterlaceMenu->actions().at(0)->setChecked(true);

    _rPlayMenu->setEnabled(false);
    _rVideoMenu->setEnabled(false);
    _rAudioMenu->setEnabled(false);
    _subtitleMenu->setEnabled(false);

    _syncAc->setEnabled(false);
    _fullAc->setEnabled(false);
    //_snapshotAc->setEnabled(false);
    _infoAc->setEnabled(false);
}

//启用视频菜单
void CMenu::enableVideoMenu()
{
    if(b_isInitVideoMenu) return;
    //菜单初始化
    clearMenu(_videoMenu);
    initVideoMenu();

    clearMenu(_titleMenu);
    initTitleMenu();

    clearMenu(_chapterMenu);
    initChapterMenu();

    clearMenu(_subtitleMenu);
    initSubtitleMenu();

    _rVideoMenu->setEnabled(true);

    _syncAc->setEnabled(true);
    _fullAc->setEnabled(true);
    //_snapshotAc->setEnabled(true);

    b_isInitVideoMenu = true;

    enableAudioMenu();

    emit updateShowState();
}

//启用音频菜单
void CMenu::enableAudioMenu()
{
    if(b_isInitMusicMenu) return;

    clearMenu(_rStereoModeMenu);
    initStereoMenu();

    clearMenu(_audioMenu);
    initAudioMenu();

    _rPlayMenu->setEnabled(true);
    _rAudioMenu->setEnabled(true);
    _infoAc->setEnabled(true);

    b_isInitMusicMenu = true;
}


void CMenu::clearMenu(QMenu *menu)
{
    menu->setEnabled(false);
    QList<QAction *> aclist = menu->actions();
    foreach (QAction *ac, aclist) {
        ac->disconnect();
        menu->removeAction(ac);
        ac->deleteLater();
        ac = NULL;
    }
}

//*************************************show*********************************************//

void CMenu::showSetMenu(const QPoint &pos)
{
    _setMenu->popup(pos);
}

void CMenu::showRightMenu(const QPoint &pos)
{
    _rightMenu->popup(pos);
}

void CMenu::showAudioMenu(const QPoint &pos)
{
    _audioMenu->popup(pos);
}

void CMenu::showSubtitleMenu(const QPoint &pos)
{
    _subtitleMenu->popup(pos);
}

void CMenu::showAirMenu(const QPoint &pos)
{
    return;
    _airMenu->popup(pos);
}

void CMenu::showChapterMenu(const QPoint &pos)
{
    _chapterMenu->popup(pos);
}

void CMenu::resetTitleMenu(const int &)
{
    if(p_timer == NULL) {
        p_timer = new QTimer(this);
        p_timer->setInterval(350);
        p_timer->setSingleShot(true);
        connect(p_timer, &QTimer::timeout, [this](){
            //清空 加载 显示
            clearMenu(_titleMenu);
            initTitleMenu();

            clearMenu(_chapterMenu);
            initChapterMenu();

            clearMenu(_subtitleMenu);
            initSubtitleMenu();

            clearMenu(_audioMenu);
            initAudioMenu();

            emit updateShowState();
        });
    }
    p_timer->stop();
    p_timer->start();
}

void CMenu::resetTrackMenu()
{
    clearMenu(_videoMenu);
    initVideoMenu();

    clearMenu(_rStereoModeMenu);
    initStereoMenu();

    clearMenu(_audioMenu);
    initAudioMenu();

    emit updateShowState();
}

void CMenu::initRecentFiles()
{
    clearRecentMenu();
    QStringList recentList = LibData->getRecent();
    _recentMenu->setEnabled(true);
    if(recentList.size() > 0) {
        _clearRecent->setEnabled(true);
    }

    foreach (QString path, recentList) {
        addRecentFile(path);
    }
}

void CMenu::hideRegistMenu()
{
    _helpMenu->removeAction(_registAc);
}

void CMenu::resetAirMenu()
{
    clearMenu(_airMenu);

    initAirPlayMenu();
}

void CMenu::addRecentFile(const QString &file)
{
    QFileInfo fi(file);

    QAction *ac = _recentMenu->addAction(fi.fileName());
    ac->setData(file);
    _recentMenu->insertAction(_clearRecent, ac);
    connect(ac, &QAction::triggered, [=]() {
            OpenFileDialog->openFile(ac->data().toString());
    });
}

void CMenu::clearRecentMenu()
{
    clearMenu(_recentMenu);
    _clearRecent = _recentMenu->addAction(Lge->getLangageValue("Menu/clearM"), this,
                                          SLOT(clearRecentFile()));
    _clearRecent->setEnabled(false);
}

void CMenu::clearRecentFile()
{
    clearMenu(_recentMenu);
    _clearRecent = _recentMenu->addAction(Lge->getLangageValue("Menu/clearM"), this,
                                          SLOT(clearRecentFile()));
    _recentMenu->setEnabled(true);
    _clearRecent->setEnabled(false);
    LibData->clearRecent();
}

void CMenu::muteMenu(const bool &b)
{
    _muteAc->setChecked(b);

    _addVolumeAc->setEnabled(!b);
    _decVolumeAc->setEnabled(!b);
}

void CMenu::showMenu()
{
    _showCount++;
    if(_showCount == 1) {
        PMW->setPopDialog(true);
        //PMW->setTimerState(false);
        PMW->showMainWidget();
    }
}

void CMenu::hideMenu()
{
    _showCount--;
    if(_showCount==0) {
        PMW->setPopDialog(false);
        //PMW->setTimerState(true);
        PMW->showMainWidget();
    }
}

void CMenu::updateUI(const qreal &scaleRatio)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);

    _setMenu  ->setFont(font);
    _rightMenu->setFont(font);
    _audioMenu->setFont(font);
    _videoMenu->setFont(font);
    _chapterMenu ->setFont(font);
    _subtitleMenu->setFont(font);
    _airMenu->setFont(font);

    _rPlayMenu  ->setFont(font);
    _rVideoMenu ->setFont(font);
    _rAudioMenu ->setFont(font);
    _rStayMenu  ->setFont(font);
    _rDeinterlaceMenu->setFont(font);
    _rAspectRatio->setFont(font);
    _rTransformMenu  ->setFont(font);
    _rEqualizerMenu  ->setFont(font);
    _rStereoModeMenu ->setFont(font);
    _titleMenu ->setFont(font);
    _fileMenu  ->setFont(font);
    _recentMenu ->setFont(font);
    _helpMenu ->setFont(font);
    _langMenu ->setFont(font);
}

void CMenu::updateStayOnTop(const int &index)
{
    switch (index)
    {
    case 0: _alwaysAc->setChecked(true);
        break;
    case 1: _whplayAc->setChecked(true);
        break;
    case 2: _neversAc->setChecked(true);
        break;
    default: break;
    }
}

//*************************************初始化*********************************************//

//初始化 子 菜单
void CMenu::initSubMenu()
{
    _setMenu   = new QMenu;
    _rightMenu = new QMenu;
    _audioMenu = new QMenu(Lge->getLangageValue("Menu/track"));
    _videoMenu = new QMenu(Lge->getLangageValue("Menu/track"));
    _chapterMenu  = new QMenu(Lge->getLangageValue("Menu/chapter"));
    _subtitleMenu = new QMenu(Lge->getLangageValue("Menu/subtitle"));
    _airMenu   = new QMenu;

    connect(_setMenu, SIGNAL(aboutToShow()), SLOT(showMenu()));
    connect(_setMenu, SIGNAL(aboutToHide()), SLOT(hideMenu()));

    connect(_airMenu, SIGNAL(aboutToShow()), SLOT(showMenu()));
    connect(_airMenu, SIGNAL(aboutToHide()), SLOT(hideMenu()));

    connect(_rightMenu, SIGNAL(aboutToShow()), SLOT(showMenu()));
    connect(_rightMenu, SIGNAL(aboutToHide()), SLOT(hideMenu()));

    connect(_audioMenu, SIGNAL(aboutToShow()), SLOT(showMenu()));
    connect(_audioMenu, SIGNAL(aboutToHide()), SLOT(hideMenu()));

    connect(_chapterMenu, SIGNAL(aboutToShow()), SLOT(showMenu()));
    connect(_chapterMenu, SIGNAL(aboutToHide()), SLOT(hideMenu()));

    connect(_subtitleMenu, SIGNAL(aboutToShow()), SLOT(showMenu()));
    connect(_subtitleMenu, SIGNAL(aboutToHide()), SLOT(hideMenu()));

    _rPlayMenu  = new QMenu(Lge->getLangageValue("Main/play"), _rightMenu);
    _rVideoMenu = new QMenu(Lge->getLangageValue("Menu/video"), _rightMenu);
    _rAudioMenu = new QMenu(Lge->getLangageValue("Menu/audio"), _rightMenu);
    _rStayMenu  = new QMenu(Lge->getLangageValue("Menu/stay"), _rightMenu);
    _rAspectRatio     = new QMenu(Lge->getLangageValue("Menu/ratio"),_rVideoMenu);
    _rDeinterlaceMenu = new QMenu(Lge->getLangageValue("Menu/deinter"), _rVideoMenu);
    _rTransformMenu   = new QMenu(Lge->getLangageValue("Menu/trans"), _rVideoMenu);
    _rEqualizerMenu   = new QMenu(Lge->getLangageValue("Menu/equalizer"), _rAudioMenu);
    _rStereoModeMenu  = new QMenu(Lge->getLangageValue("Menu/stereo"), _rAudioMenu);

    _titleMenu = new QMenu(Lge->getLangageValue("Menu/title"));

    //文件菜单
    _fileMenu = new QMenu(Lge->getLangageValue("Menu/file"));

    //打开文件历史纪录菜单
    _recentMenu = new QMenu(Lge->getLangageValue("Menu/recent"), _fileMenu);
    _clearRecent = _recentMenu->addAction(Lge->getLangageValue("Menu/clearM"), this, SLOT(clearRecentFile()));
    _clearRecent->setEnabled(false);

    //_subtitleMenu->addSeparator();
    _loadSub = _subtitleMenu->addAction(Lge->getLangageValue("Menu/ldsub"), OpenFileDialog, SLOT(openSubtitleFileDialog()));
    //_fileMenu->addAction("Open Video",    OpenFileDialog, SLOT(openVideoDialog()));
    //QAction* mus = _fileMenu->addAction(Lge->getLangageValue("Menu/OMusic"),    OpenFileDialog, SLOT(openMusicDialog()));
    //mus->setObjectName("Menu/OMusic");
    QAction* dvd = _fileMenu->addAction(Lge->getLangageValue("Menu/ODVD"),      OpenFileDialog, SLOT(openDvdDialog()));
    dvd->setObjectName("Menu/ODVD");
    _fileMenu->addAction(Lang("Menu/OVideoTS"), OpenFileDialog, SLOT(openVideoTSDialog()));
    _fileMenu->addMenu(_recentMenu);

    //帮助
    _helpMenu = new QMenu(Lge->getLangageValue("Menu/help"));
    QAction* help = _helpMenu->addAction(Lge->getLangageValue("Menu/help"),    OpenFileDialog, SLOT(openHelpUrl()));
    help->setObjectName("Menu/help");
    QAction* fdk = _helpMenu->addAction(Lge->getLangageValue("Menu/fdk&sug"),  OpenFileDialog, SLOT(openSendEmail())); //ToDo
    fdk->setObjectName("Menu/fdk&sug");
    QAction* ckUp = _helpMenu->addAction(Lge->getLangageValue("Menu/ckUp"),    OpenFileDialog, SLOT(openUpdateDialog()));
    ckUp->setObjectName("Menu/ckUp");
    _registAc = _helpMenu->addAction(Lge->getLangageValue("Menu/reg"),OpenFileDialog, SLOT(openRegisterDialog()));
    _registAc->setObjectName("Menu/reg");
    QAction* about = _helpMenu->addAction(Lge->getLangageValue("Menu/about"),  OpenFileDialog, SLOT(openAboutDialog()));
    about->setObjectName("Menu/about");

    //语言 ToDo
    _langMenu = new QMenu(Lge->getLangageValue("Menu/language"));
    QList<QString> langs = Global->languageList();
    foreach (QString lang, langs) {
        QString action = "English";
        if(lang == "zh") {
            action = "中文繁體";
        } else if(lang == "jp") {
            action = "日本語";
        } else if(lang == "fr") {
            action = "Français";
        } else if(lang == "es") {
            action = "Español";
        } else if(lang == "de") {
            action = "Deutsch";
        } else if(lang == "it") {
            action = "Italiano";
        } else if(lang == "pt") {
            action = "Português";
        } else {
            action = "English";
        }
        QAction *ac = _langMenu->addAction(action);
        ac->setData(lang);
        ac->setCheckable(true);
        ac->setActionGroup(_languageGroup);
        ac->setChecked(Global->getLanguage() == i.key());
    }

    connect(_langMenu, &QMenu::triggered, [](QAction *ac){
        PMW->languageChange(ac->data().toString());
    });

    updateUI(1.0);

    //设置菜单风格

    _rightMenu ->setStyleSheet(Global->menuStyleSheet());
    _setMenu->setStyleSheet(Global->menuStyleSheet());
    _audioMenu->setStyleSheet(Global->menuStyleSheet());
    _videoMenu->setStyleSheet(Global->menuStyleSheet());
    _chapterMenu->setStyleSheet(Global->menuStyleSheet());
    _subtitleMenu->setStyleSheet(Global->menuStyleSheet());
    _fileMenu->setStyleSheet(Global->menuStyleSheet());
    _helpMenu->setStyleSheet(Global->menuStyleSheet());
    _langMenu->setStyleSheet(Global->menuStyleSheet());
    //"border-bottom:1px solid #DBDBDB;"/*为菜单项之间添加横线间隔*/
}

//初始化设置菜单
void CMenu::initSetMenu()
{
    _openFile_set = _setMenu->addAction(Lge->getLangageValue("Menu/OFile"), OpenFileDialog, SLOT(openVideoDialog()));
    _setMenu->addMenu(_fileMenu);
    _setMenu->addSeparator();
    _setAc = _setMenu->addAction(Lge->getLangageValue("Menu/set"),      OpenFileDialog, SLOT(openSettingsDialog()), QKeySequence("F2"));
    _hwaccelAc = _setMenu->addAction(Lge->getLangageValue("Menu/hw"),   OpenFileDialog, SLOT(openHWAccelDialog()));
    _setMenu->addSeparator();
    _setMenu->addMenu(_langMenu);
    _setMenu->addMenu(_helpMenu);
}

//初始化右键菜单
void CMenu::initRightMenu()
{
    //播放控制
    QAction* play = _rPlayMenu->addAction(Lge->getLangageValue("Menu/playpaush"),   MediaPlayer, SLOT(togglePause()));
    play->setObjectName("Menu/playpaush");
    QAction* stop = _rPlayMenu->addAction(Lge->getLangageValue("Menu/stop"),        MediaPlayer, SLOT(stop()));
    stop->setObjectName("Menu/stop");
    QAction* prev = _rPlayMenu->addAction(Lge->getLangageValue("Menu/previous"),    MediaPlayer, SLOT(playPre()));
    prev->setObjectName("Menu/previous");
    QAction* next = _rPlayMenu->addAction(Lge->getLangageValue("Menu/next"),        MediaPlayer, SLOT(playNext()));
    next->setObjectName("Menu/next");

    //反交错
    int i=0;
    foreach(QString str, DYVlc::deinterlacing())
    {
        QAction *ac = _rDeinterlaceMenu->addAction(str, [str](){MediaPlayer->video()->setDeinterlace(str);});
        ac->setActionGroup(_deinterlaceGroup);
        ac->setCheckable(true);
        if(i == 0)
            ac->setChecked(true);
        ++i;
    }

    //宽高比
    i=0;
    foreach(QString str, DYVlc::ratio())
    {
        QAction *ac = _rAspectRatio->addAction(str, [str](){MediaPlayer->video()->setAspeceRatio(str);});
        ac->setActionGroup(_aspectRatioGroup);
        ac->setCheckable(true);
        if(i == 0)
            ac->setChecked(true);
        ++i;
    }

    //旋转
    QAction* none = _rTransformMenu->addAction(Lge->getLangageValue("Menu/none"),
                                               [](){MediaPlayer->rotate()->closeFlip();});
    none->setObjectName("Menu/none");
    _rTransformMenu->addSeparator();
    QAction* horz = _rTransformMenu->addAction(Lge->getLangageValue("Menu/horz"),
                                               [](){MediaPlayer->rotate()->hflip();});
    horz->setObjectName("Menu/horz");
    QAction* vert = _rTransformMenu->addAction(Lge->getLangageValue("Menu/vert"),
                                               [](){MediaPlayer->rotate()->vflip();});
    vert->setObjectName("Menu/vert");

    //均衡器
    auto equalizer = MediaPlayer->equalizer();
    for (int j = -1; j < (int)equalizer->presetCount(); ++j) {
        QAction *ac = _rEqualizerMenu->addAction(equalizer->presetNameAt(j), [j](){
            MediaPlayer->equalizer()->loadFromPreset(j);
        });
        ac->setActionGroup(_equalizerMenuGroup);
        ac->setCheckable(true);
        if(j == -1) {
            ac->setChecked(true);
        }
    }

    //视频
    _rVideoMenu->addMenu(_videoMenu);
    _rVideoMenu->addMenu(_titleMenu);
    _rVideoMenu->addSeparator();
    _rVideoMenu->addMenu(_rAspectRatio);    //宽高比
    _rVideoMenu->addMenu(_rDeinterlaceMenu);//反交错菜单
    _rVideoMenu->addMenu(_rTransformMenu);  //旋转菜单
    _rVideoMenu->addSeparator();
    _tuner = _rVideoMenu->addAction(Lge->getLangageValue("Menu/tuner"), OpenFileDialog, SLOT(openVideoTunerDialog()));

    //音频
    _rAudioMenu->addMenu(_audioMenu);
    _rAudioMenu->addMenu(_rEqualizerMenu);//均衡器
    _rAudioMenu->addMenu(_rStereoModeMenu);
    _rAudioMenu->addSeparator();
    _muteAc = _rAudioMenu->addAction(Lge->getLangageValue("Menu/mute"), MediaPlayer->audio(), SLOT(toggleVolunMute()));
    _muteAc->setCheckable(true);
    _addVolumeAc = _rAudioMenu->addAction(Lge->getLangageValue("Menu/voladd"), MediaPlayer->audio(), SLOT(addVolums()));
    _decVolumeAc = _rAudioMenu->addAction(Lge->getLangageValue("Menu/voldec"), MediaPlayer->audio(), SLOT(decVolums()));


    //层次
    _alwaysAc = _rStayMenu->addAction(Lge->getLangageValue("Menu/always"),[](){PMW->stayAlwaysOnTop();});
    _whplayAc = _rStayMenu->addAction(Lge->getLangageValue("Menu/WPlay"), [](){PMW->stayPlayingOnTop();});
    _neversAc = _rStayMenu->addAction(Lge->getLangageValue("Menu/never"), [](){PMW->stayNeverOnTop();});
    _alwaysAc->setCheckable(true);
    _alwaysAc->setObjectName("Menu/always");
    _alwaysAc->setActionGroup(_stayGroup);
    _neversAc->setCheckable(true);
    _neversAc->setObjectName("Menu/never");
    _neversAc->setActionGroup(_stayGroup);
    _whplayAc->setCheckable(true);
    _whplayAc->setObjectName("Menu/WPlay");
    _whplayAc->setActionGroup(_stayGroup);


    //右键菜单
    _openFile_right = _rightMenu->addAction(Lge->getLangageValue("Menu/OFile"), OpenFileDialog, SLOT(openVideoDialog()));
    _rightMenu->addMenu(_fileMenu);
    _rightMenu->addSeparator();
    _rightMenu->addMenu(_rPlayMenu);
    _rightMenu->addSeparator();
    _rightMenu->addMenu(_rVideoMenu);
    _rightMenu->addMenu(_rAudioMenu);
    _rightMenu->addMenu(_subtitleMenu);

    _syncAc = _rightMenu->addAction(Lge->getLangageValue("Menu/synon"), OpenFileDialog, SLOT(openTrackSyncDialog()));
    _rightMenu->addSeparator();

    _rightMenu->addMenu(_rStayMenu);
    _rightMenu->addSeparator();

    _fullAc = _rightMenu->addAction(Lge->getLangageValue("Menu/fuscr"), [](){
        if(PMW->isFullScreen() || PMW->isMaximized())
            PMW->showNormaled();
        else
            PMW->showFullScreened();}, QKeySequence("Enter"));
    //_snapshotAc = _rightMenu->addAction("Snapshot", MediaPlayer, SLOT(snapShot()), QKeySequence("F8"));
    _convert = _rightMenu->addAction(Lge->getLangageValue("Menu/convt"), OpenFileDialog, SLOT(openConvertUrl()));
    _rightMenu->addSeparator();
    _relation = _rightMenu->addAction(Lge->getLangageValue("PlayMain/relation"), OpenFileDialog, SLOT(openRelationDialog()));
    _rightMenu->addAction(_setAc);
    _rightMenu->addAction(_hwaccelAc);
    _infoAc = _rightMenu->addAction(Lge->getLangageValue("Menu/minfo"), OpenFileDialog, SLOT(openMediaInfoDialog()));
}

//初始化音轨
void CMenu::initAudioMenu()
{
    int curId = MediaPlayer->audio()->track();
    QMap<int, QString> map = MediaPlayer->audio()->tracks();

    _audioMenu->setEnabled(false);
    if(map.count() > 0)
    {
        QMapIterator<int, QString> it(map);
        while(it.hasNext())
        {
            it.next();
            QAction *ac = _audioMenu->addAction(it.value());
            ac->setData(it.key());
            ac->setActionGroup(_audioMenuGroup);
            ac->setCheckable(true);
            if(curId == it.key())
            {
                ac->setChecked(true);
            }
            connect(ac, &QAction::toggled, [=](bool checked) {
                if(checked)
                    MediaPlayer->audio()->setTrack(ac->data().toInt());
            });
        }
        _audioMenu->setEnabled(true);
    }
}

void CMenu::initStereoMenu()
{
    const int curId = MediaPlayer->audio()->channel();

    QStringList stereoList = Lge->getLangageValue("Menu/stereoList").split("|");
    for(int j = 1; j<=5; ++j) {
        QAction *ac = _rStereoModeMenu->addAction(stereoList[j-1], [j](){
            MediaPlayer->audio()->setChannel(j);
        });
        ac->setActionGroup(_stereoModeMenuGroup);
        ac->setCheckable(true);
        if(j == curId)
            ac->setChecked(true);
    }
    _rStereoModeMenu->setEnabled(true);
}

//初始化视频轨道
void CMenu::initVideoMenu()
{
    int curId = MediaPlayer->video()->track();
    QMap<int, QString> map = MediaPlayer->video()->tracks();

    _videoMenu->setEnabled(false);
    if(map.count() > 0)
    {
        QMapIterator<int, QString> it(map);
        while(it.hasNext())
        {
            it.next();
            QAction *ac = _videoMenu->addAction(it.value());
            ac->setData(it.key());
            ac->setActionGroup(_videoMenuGroup);
            ac->setCheckable(true);
            if(curId == it.key())
            {
                ac->setChecked(true);
            }
            connect(ac, &QAction::toggled, [=](bool checked) {
                if(checked)
                    MediaPlayer->video()->setvideoTrack(ac->data().toInt());
            });
        }
        _videoMenu->setEnabled(true);
    }
}

//初始化Title
void CMenu::initTitleMenu()
{
    int curId = MediaPlayer->video()->title();
    QMap<int, QString> map = MediaPlayer->video()->titles();

    _titleMenu->setEnabled(false);
    if(map.count() > 0)
    {
        QMapIterator<int, QString> it(map);
        while(it.hasNext())
        {
            it.next();
            QAction *ac = _titleMenu->addAction(it.value());
            ac->setData(it.key());
            ac->setActionGroup(_titleMenuGroup);
            ac->setCheckable(true);
            if(curId == it.key())
            {
                ac->setChecked(true);
            }
            connect(ac, &QAction::toggled, [=](bool checked) {
                if(checked)
                    MediaPlayer->video()->setTitle(ac->data().toInt());
            });
        }
        _titleMenu->setEnabled(true);
    }
}

//初始化章节
void CMenu::initChapterMenu()
{
    _chapterMenu->setEnabled(false);

    const int titleId = MediaPlayer->video()->title();
    if(titleId < 1)
        return;

    const int curId = MediaPlayer->video()->chapter();
    QMap<int, QString> map = MediaPlayer->video()->chapters();

    if(map.count() > 0)
    {
        QMapIterator<int, QString> it(map);
        while(it.hasNext())
        {
            it.next();
            QAction *ac = _chapterMenu->addAction(it.value());
            ac->setData(it.key());
            ac->setActionGroup(_chapterMenuGroup);
            ac->setCheckable(true);
            if(curId == it.key())
            {
                ac->setChecked(true);
            }
            connect(ac, &QAction::toggled, [=](bool checked) {
                if(checked)
                    MediaPlayer->video()->setChapter(ac->data().toInt());
            });
        }
        _chapterMenu->setEnabled(true);
    }
}

//初始化字幕
#include <QDir>
void CMenu::initSubtitleMenu()
{
    QFileInfo fileinfo(LibData->getplayinfo().filepath);
    bool autodetect = false;
    _folderSubList.clear();
    _pathSubList.clear();
    _folderSubList = LibData->getSubFilePathOnFolder(QDir::toNativeSeparators(fileinfo.absolutePath()),autodetect);
    _subtitleMenu->setEnabled(false);

    // 文件目录下符合加载条件的字幕路径
    bool setSub = true;
    if(_folderSubList.count() > 0) {
        foreach (QString val, _folderSubList) {
            QFileInfo subFile(val);
            QAction *ac = _subtitleMenu->addAction(subFile.fileName());
            ac->setData(val);
            ac->setActionGroup(_subtitleMenuGroup);
            ac->setCheckable(true);
            if(subFile.fileName().endsWith("en",Qt::CaseInsensitive) || subFile.fileName().endsWith("english",Qt::CaseInsensitive) ) {
                setSub = false;
                ac->setChecked(true);
                MediaPlayer->video()->setSubtitleFile(ac->data().toString());
            }
            if(autodetect && setSub) {
                setSub = false;
                ac->setChecked(true);
                MediaPlayer->video()->setSubtitleFile(ac->data().toString());
            }
            connect(ac, &QAction::toggled, [=](bool checked) {
                if(checked)
                    MediaPlayer->video()->setSubtitleFile(ac->data().toString());
            });
        }
        _subtitleMenu->addSeparator();
    }

    int curId = MediaPlayer->video()->subtitle();
    QMap<int, QString> map = MediaPlayer->video()->subtitles();

    if(map.count() > 0)
    {
        QMapIterator<int, QString> it(map);
        while(it.hasNext())
        {
            it.next();
            QAction *ac = _subtitleMenu->addAction(it.value());
            ac->setData(it.key());
            ac->setActionGroup(_subtitleMenuGroup);
            ac->setCheckable(true);
            if(curId == it.key() && !autodetect)
            {
                ac->setChecked(true);
            }
            connect(ac, &QAction::toggled, [=](bool checked) {
                if(checked)
                    MediaPlayer->video()->setSubtitle(ac->data().toInt());
            });
        }
    } else {
        if(_folderSubList.count() > 0) {
            QAction *ac = _subtitleMenu->addAction("Disable");
            ac->setCheckable(true);
            ac->setActionGroup(_subtitleMenuGroup);
            connect(ac, &QAction::toggled, [=](bool checked) {
                if(checked)
                    MediaPlayer->video()->setSubtitle(-1);
            });
        }
    }
    _loadSub = _subtitleMenu->addAction(Lge->getLangageValue("Menu/ldsub"), OpenFileDialog, SLOT(openSubtitleFileDialog()));
    _subtitleMenu->setEnabled(true);
}

void CMenu::initAirPlayMenu()
{
    QList<QZeroConfService*> clientList = LibData->getSevList();
    AirMenuWidget *ac = new AirMenuWidget(":/res/png/computer_icon.png","Computer",true,_airMenu);
    ac->setData("Computer");
    ac->setCheckable(true);
    ac->setActionGroup(_airMenuGroup);
    _airMenu->addAction(ac);
    connect(ac, &QAction::toggled, [=](bool checked) {
        if(checked) {
            //UdpSev->sendMsgToConCPT(UDPDISCON);
            LibData->airDisConnect();
        }
    });
    if(clientList.size() > 0) {
        for(int i=0;i<clientList.size();++i) {
            AirMenuWidget *ac = new AirMenuWidget(":/res/png/computer_icon.png",clientList.at(i)->name,false,_airMenu);
            ac->setData(clientList.at(i)->name + "+" + clientList.at(i)->ip.toString());
            ac->setCheckable(true);
            ac->setActionGroup(_airMenuGroup);
            _airMenu->addAction(ac);
            connect(ac, &QAction::toggled, [=](bool checked) {
                if(checked) {
                    ac->setChecked(true);
//                    if(LibData->b_isConCPT) {
//                        UdpSev->sendMsgToConCPT(UDPDISCON);
//                    }
                    LibData->setUdpClient(clientList.at(i));
//                    UdpSev->sendMsgToConCPT(UDPCONNECT + QHostInfo::localHostName());
                } else {
                    ac->setChecked(false);
                }
            });
        }
    }
}

void CMenu::upQActionText(QMenu *menu)
{
    if(!menu)
        return;
    for(int i=0;i<menu->actions().count();++i) {
        QAction* ac = menu->actions().at(i);
        if(!ac->objectName().isEmpty())
            ac->setText(Lge->getLangageValue(ac->objectName()));
    }
}

void CMenu::selectSubtitle(QString subPath)
{
    QList<QAction *> aclist = _subtitleMenu->actions();
    foreach (QAction *ac, aclist) {
        if(ac->data().toString() == subPath) {
            ac->setChecked(true);
            break;
        }
    }
}

bool CMenu::isLoadSubtitle(QString subPath)
{
    bool isAutoLoad = false;
    foreach (QString val, _folderSubList) {
        if(val == subPath) {
            isAutoLoad = true;
            break;
        }
    }
    foreach (QString path, _pathSubList) {
        if(path == subPath) {
            isAutoLoad = true;
        }
    }
    return isAutoLoad;
}

void CMenu::loadSubpath2Subtitle(QString subPath)
{
    _pathSubList.append(subPath);
    QList<QAction *> aclist = _subtitleMenu->actions();
    int pos = 0;
    foreach (QAction *ac, aclist) {
        if(pos >= _folderSubList.count()) {
            QFileInfo subFile(subPath);
            QAction *newAc = new QAction(subFile.fileName());
            newAc->setData(subPath);
            newAc->setActionGroup(_subtitleMenuGroup);
            newAc->setCheckable(true);
            _subtitleMenu->insertAction(ac,newAc);
            newAc->setChecked(true);
            MediaPlayer->video()->setSubtitleFile(subPath);
            connect(newAc, &QAction::toggled, [=](bool checked) {
                if(checked)
                    MediaPlayer->video()->setSubtitleFile(newAc->data().toString());
            });
            // 没有视频自带字幕和自动添加字幕，在手动添加字幕时，加入disable的menu
            if(pos == 0 && aclist.count() == 1) {
                QAction *disac = new QAction("Disable");
                disac->setCheckable(true);
                disac->setActionGroup(_subtitleMenuGroup);
                _subtitleMenu->insertAction(ac,disac);
                connect(disac, &QAction::toggled, [=](bool checked) {
                    if(checked)
                        MediaPlayer->video()->setSubtitle(-1);
                });
            }
            break;
        }
        pos++;
    }
}
