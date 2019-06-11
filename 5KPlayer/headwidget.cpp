#include "headwidget.h"
#include "textbutton.h"
#include "svgbutton.h"
#include <QActionGroup>
#include "globalarg.h"
#include "dymediaplayer.h"
#include "playermainwidget.h"
#include "openfile.h"
#include <QtDebug>
#include "cmenu.h"
#include "globalarg.h"

HeadWidget::HeadWidget(QWidget *parent) : TransEventWidget(parent), UpdateInterface()
{
    initChangeControl();
    initVideoControl();
    initWidgetControl();

    //_originalHeight = 44;
    //this->setFixedHeight(_originalHeight);
    this->setStyleSheet("background: rgb(0,0,0)");

    showControl(false);
    _playerBtn->click();

    InitLanguage();
    //connect(Menu, SIGNAL(updateShowState()), SLOT(updateShowMenusState()));
}

void HeadWidget::updateLanguage()
{
    _libraryBtn->setText(Lge->getLangageValue("MainHead/library"));
    Menu->updateLanguage();
    _playerBtn->updateUI(Global->_screenRatio);
    _libraryBtn->updateUI(Global->_screenRatio);

    InitLanguage();
    //更新menu
    //更新tooltip
}

void HeadWidget::updateUI(const qreal &scaleRatio)
{
    _playerBtn->updateUI(scaleRatio);
    _libraryBtn->updateUI(scaleRatio);

    _leftRotateBtn->updateUI(scaleRatio);
    _rightRotateBtn->updateUI(scaleRatio);
    _chapterBtn->updateUI(scaleRatio);
    _subtitleBtn->updateUI(scaleRatio);
    _audioTrackBtn->updateUI(scaleRatio);

    //_airBtn->updateUI(scaleRatio);
    _minimalBtn->updateUI(scaleRatio);
    _zoomBtn->updateUI(scaleRatio);
    _closeBtn->updateUI(scaleRatio);
    _setBtn->updateUI(scaleRatio);

    //this->setFixedHeight(_originalHeight * scaleRatio);
}

bool HeadWidget::isShowPlayerWidget() const
{
    return _playerBtn->pressed();
}

bool HeadWidget::isShowLibraryWidget() const
{
    return _libraryBtn->pressed();
}

void HeadWidget::enableRotate(const bool &enable)
{
    _leftRotateBtn->setEnabled(enable);
    _rightRotateBtn->setEnabled(enable);
}

void HeadWidget::changeAirplayState(const bool &b)
{
    Q_UNUSED(b);
    //_airBtn->changeState("airplayer", b);
}

void HeadWidget::clickPlayerW()
{
    _playerBtn->click();
}

void HeadWidget::clickLibraryW()
{
    _libraryBtn->click();
}

void HeadWidget::showControl(bool isShow)
{
    _leftRotateBtn->setVisible(isShow);
    _rightRotateBtn->setVisible(isShow);
    _chapterBtn->setVisible(isShow);
    _subtitleBtn->setVisible(isShow);
    _audioTrackBtn->setVisible(isShow);

    if(isShow)
    {
        updateShowMenusState();
    }
}

void HeadWidget::clickLeftitem(QString)
{
    clickLibraryW();
}

void HeadWidget::showMenus()
{
    Menu->showSetMenu(cursor().pos());
}

void HeadWidget::showAudioMenus()
{
    Menu->showAudioMenu(cursor().pos());
}

void HeadWidget::showSubtitleMenus()
{
    Menu->showSubtitleMenu(cursor().pos());
}

void HeadWidget::showChapterMenus()
{
    Menu->showChapterMenu(cursor().pos());
}

void HeadWidget::showAirPlayList()
{
    Menu->showAirMenu(cursor().pos());
}

void HeadWidget::initChangeControl()
{
    _playerBtn  = new TextButton("5KPlayer", 84, 24, this);
    _libraryBtn = new TextButton(Lge->getLangageValue("MainHead/library"),  84, 24, this);
    _libraryBtn->setPosition(TextButton::Right);

    connect(_playerBtn,  SIGNAL(clicked()), SIGNAL(showPlayerW()));
    connect(_playerBtn,  SIGNAL(clicked()), _libraryBtn, SLOT(recover()));

    connect(_libraryBtn, SIGNAL(clicked()), SIGNAL(showLibraryW()));
    connect(_libraryBtn, SIGNAL(clicked()), _playerBtn, SLOT(recover()));
}

void HeadWidget::initVideoControl()
{
    //增加控制界面(界面旋转)
    _leftRotateBtn = new SvgButton(this);
    _rightRotateBtn = new SvgButton(this);
    _chapterBtn = new SvgButton(this);
    _subtitleBtn = new SvgButton(this);
    _audioTrackBtn = new SvgButton(this);

    _leftRotateBtn->initSize(44, 44);
    _leftRotateBtn->setImagePrefixName("Left_rotation");

    _rightRotateBtn->initSize(44, 44);
    _rightRotateBtn->setImagePrefixName("right_rotation");

    _chapterBtn->initSize(44, 44);
    _chapterBtn->setImagePrefixName("Video_section");

    _subtitleBtn->initSize(44, 44);
    _subtitleBtn->setImagePrefixName("subtitle");

    _audioTrackBtn->initSize(44, 44);
    _audioTrackBtn->setImagePrefixName("Audio");

    connect(_chapterBtn, SIGNAL(clicked()),   SLOT(showChapterMenus()));
    connect(_subtitleBtn, SIGNAL(clicked()),  SLOT(showSubtitleMenus()));
    connect(_audioTrackBtn, SIGNAL(clicked()),SLOT(showAudioMenus()));

    connect(_leftRotateBtn,  SIGNAL(clicked()), SLOT(leftRotate()));
    connect(_rightRotateBtn, SIGNAL(clicked()), SLOT(rightRotate()));
}
#include <QTimer>
void HeadWidget::leftRotate()
{
    Global->rotate();
    PMW->rotate();
    MediaPlayer->rotate()->leftRotate();
}

void HeadWidget::rightRotate()
{
    Global->rotate();
    PMW->rotate();
    MediaPlayer->rotate()->rightRotate();
}

void HeadWidget::updateShowMenusState()
{
    _chapterBtn->setEnabled(Menu->chapterMenuIsEnable());
    _subtitleBtn->setEnabled(Menu->subtitleMenuIsEnable());
    _audioTrackBtn->setEnabled(Menu->audioMenuIsEnable());
}

void HeadWidget::InitLanguage()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*Global->_screenRatio);
    _leftRotateBtn->setFont(font);
    _rightRotateBtn->setFont(font);
    _chapterBtn->setFont(font);
    _subtitleBtn->setFont(font);
    _audioTrackBtn->setFont(font);
    _minimalBtn->setFont(font);
    _zoomBtn->setFont(font);
    _closeBtn->setFont(font);
    _setBtn->setFont(font);
    _leftRotateBtn->setToolTip(Lge->getLangageValue("MainHead/left"));
    _rightRotateBtn->setToolTip(Lge->getLangageValue("MainHead/right"));
    _chapterBtn->setToolTip(Lge->getLangageValue("MainHead/chap"));
    _subtitleBtn->setToolTip(Lge->getLangageValue("MainHead/subTrack"));
    _audioTrackBtn->setToolTip(Lge->getLangageValue("MainHead/audioTrack"));
    _minimalBtn->setToolTip(Lge->getLangageValue("ToolTip/min"));
    _zoomBtn->setToolTip(Lge->getLangageValue("ToolTip/max"));
    _closeBtn->setToolTip(Lge->getLangageValue("ToolTip/colse"));
    _setBtn->setToolTip(Lge->getLangageValue("ToolTip/setting"));
}

void HeadWidget::initWidgetControl()
{
    //增加控制界面(最小化  最大化  关闭)
    //_airBtn     = new SvgButton(this);
    _minimalBtn = new SvgButton(this);
    _zoomBtn = new SvgButton(this);
    _closeBtn = new SvgButton(this);
    _setBtn = new SvgButton(this);

    //_airBtn->initSize(44, 44);
    //_airBtn->setImagePrefixName("airplayer");
    //_airBtn->changeState("airplayer", false);

    _minimalBtn->initSize(26,22);
    _minimalBtn->setImagePrefixName("minimize");

    _zoomBtn->initSize(26,22);
    _zoomBtn->setImagePrefixName("zoom");

    _closeBtn->initSize(26,22);
    _closeBtn->setImagePrefixName("close");

    _setBtn->initSize(26,22);
    _setBtn->setImagePrefixName("settings");

    connect(_closeBtn, &SvgButton::clicked, [](){
        PMW->closeWithConfig(true);
    });

    connect(_setBtn, SIGNAL(clicked()), SLOT(showMenus()));
    //connect(_airBtn,SIGNAL(clicked()),SLOT(showAirPlayList()));

    connect(_minimalBtn, &SvgButton::clicked, [=](){
        this->parentWidget()->showMinimized();
        PMW->showMinimized();
    });

    connect(_zoomBtn,   &SvgButton::clicked, [](){
        if(PMW->isFullScreen() || PMW->isMaximized())
            PMW->showNormaled();
        else
            PMW->showFullScreened();
            //PMW->showMaximized();
    });
}

//重置位置
void HeadWidget::resizeEvent(QResizeEvent *)
{
    const int w = this->width();
    const int h = this->height();

    _libraryBtn->move(w / 2, (h-_libraryBtn->height())/2);
    _playerBtn->move(w / 2 - _playerBtn->width(), (h-_playerBtn->height())/2);

    const int cw = _leftRotateBtn->width();

    _leftRotateBtn->move(0, 0);
    _rightRotateBtn->move(cw*1, 0);
    _chapterBtn->move(cw*2, 0);
    _subtitleBtn->move(cw*3, 0);
    _audioTrackBtn->move(cw*4, 0);

    const int mw = _closeBtn->width();
    const int mh = _closeBtn->height();

    _closeBtn->move(w-mw, 0);
    _setBtn->move(w-mw, mh);
    _zoomBtn->move(w-mw*2, 0);
    _minimalBtn->move(w-mw*2, mh);

    //_airBtn->move(w-mw*2-cw, 0);
}
