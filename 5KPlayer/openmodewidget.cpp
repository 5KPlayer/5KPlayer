#include "openmodewidget.h"
#include "svgbutton.h"
#include "openfile.h"
#include <QBoxLayout>
#include <QtDebug>

OpenModeWidget::OpenModeWidget(QWidget *parent) : TransEventWidget(parent)
{
    //增加控制界面
    _videoBtn   = new SvgButton(1.2, this);
    _dvdBtn     = new SvgButton(1.2, this);
    _musicBtn   = new SvgButton(1.2, this);
    _radioBtn   = new SvgButton(1.2, this);
    _youtubeBtn = new SvgButton(1.2, this);
    _airPlay    = new SvgButton(1.2, this);

    _videoBtn->initSize(164,93);
    _videoBtn->setImagePrefixName("main_video");
    _videoBtn->setSvgText(Lge->getLangageValue("OpenFile/video"));
    _videoBtn->setSvgTextColor(QColor(244,244,244),QColor(126,126,126));

    _dvdBtn->initSize(92,93);
    _dvdBtn->setImagePrefixName("main_dvd");
    _dvdBtn->setSvgText("DVD");
    _dvdBtn->setSvgTextColor(QColor(244,244,244),QColor(126,126,126));

    _musicBtn->initSize(92,93);
    _musicBtn->setImagePrefixName("main_music");
    _musicBtn->setSvgText(Lge->getLangageValue("OpenFile/music"));
    _musicBtn->setSvgTextColor(QColor(244,244,244),QColor(126,126,126));

    _radioBtn->initSize(92,93);
    _radioBtn->setImagePrefixName("main_ridio");
    _radioBtn->setSvgText(Lge->getLangageValue("OpenFile/live"));
    _radioBtn->setSvgTextColor(QColor(244,244,244),QColor(126,126,126));

    _youtubeBtn->initSize(220, 108);
    _youtubeBtn->setImagePrefixName("main_youtube");

    _airPlay->initSize(220, 108);
    _airPlay->setImagePrefixName("main_ariplayer");

    connect(_videoBtn,   SIGNAL(clicked()), SLOT(openVideoDialog()));
    connect(_dvdBtn,     SIGNAL(clicked()), SLOT(openDvdDialog()));
    connect(_musicBtn,   SIGNAL(clicked()), SLOT(openMusicDialog()));
    connect(_radioBtn,   SIGNAL(clicked()), SLOT(openRadioDialog()));
    connect(_youtubeBtn, SIGNAL(clicked()), SLOT(openYouTubeDialog()));
    connect(_airPlay,    SIGNAL(clicked()), SLOT(openAirPlayDialog()));

    this->setMouseTracking(true);
}

void OpenModeWidget::updateLanguage()
{
    _videoBtn->setSvgText(Lge->getLangageValue("OpenFile/video"));
    _musicBtn->setSvgText(Lge->getLangageValue("OpenFile/music"));
    _radioBtn->setSvgText(Lge->getLangageValue("OpenFile/live"));
}

void OpenModeWidget::updateUI(const qreal &scaleRatio)
{
    _videoBtn->updateUI(scaleRatio);
    _dvdBtn->updateUI(scaleRatio);
    _musicBtn->updateUI(scaleRatio);
    _radioBtn->updateUI(scaleRatio);
    _youtubeBtn->updateUI(scaleRatio);
    _airPlay->updateUI(scaleRatio);
}

void OpenModeWidget::openAnimal(bool b)
{
    _videoBtn->openAnimal(b);
    _dvdBtn->openAnimal(b);
    _musicBtn->openAnimal(b);
    _radioBtn->openAnimal(b);
    _youtubeBtn->openAnimal(b);
    _airPlay->openAnimal(b);
}

void OpenModeWidget::openVideoDialog()
{
    OpenFileDialog->openVideoDialog();
}

void OpenModeWidget::openDvdDialog()
{
    OpenFileDialog->openDvdDialog();
}

void OpenModeWidget::openMusicDialog()
{
    //OpenFileDialog->openMusicDialog();
    emit itemClicked("pMusic");
}

void OpenModeWidget::openRadioDialog()
{
    OpenFileDialog->openRadioDialog();
}

void OpenModeWidget::openYouTubeDialog()
{
    emit itemClicked("YouTube");
}

void OpenModeWidget::openAirPlayDialog()
{
    OpenFileDialog->openAirPlayDialog();
}

void OpenModeWidget::resizeEvent(QResizeEvent *)
{
    const int w = this->width();
    const int h = this->height();

    _airPlay->resetMove(w/2, h/2);
    _youtubeBtn->resetMove(w/2 - _youtubeBtn->width(), h/2);

    const int y = h/2-_videoBtn->height();
    _videoBtn->resetMove(w/2 - _youtubeBtn->width(), y);

    _dvdBtn->resetMove(_videoBtn->x()+_videoBtn->width(), y);
    _musicBtn->resetMove(_dvdBtn->x()+_dvdBtn->width(), y);
    _radioBtn->resetMove(_musicBtn->x()+_musicBtn->width(), y);
}



