#include "synctrackwidget.h"
#include <QLabel>
#include "cslider2.h"
#include "dylabel.h"
#include "dymediaplayer.h"
#include <QtDebug>
#include "globalarg.h"
#define SLIDER_HEIGHT 40

SyncTrackWidget::SyncTrackWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    //audio
    _audioLabel  = new QLabel(Lge->getLangageValue("TrackSyn/audio"), this);
    _audioLabel->setPalette(p1);

    _audioLabel1  = new QLabel("-10s", this);
    _audioLabel1->setPalette(p1);

    _audioLabel2  = new QLabel("10s", this);
    _audioLabel2->setPalette(p1);

    _audioDetailLabel  = new QLabel(Lge->getLangageValue("TrackSyn/audioDet"), this);
    _audioDetailLabel->setPalette(p1);

    _audioSlider = new CSlider2(this);
    _audioSlider->showAllLine();
    _audioSlider->initSize(300, SLIDER_HEIGHT);
    _audioSlider->setRange(0, 20);
    _audioSlider->setPageStep(1.0);
    //_audioSlider->setValue(10.0);
    int audioDelay = MediaPlayer->audio()->getDelay();
    if(audioDelay > 10 || audioDelay < -10)
        audioDelay = 0;
    _audioSlider->setValue(audioDelay + 10);

    //video
    _subtitleLabel  = new QLabel(Lge->getLangageValue("TrackSyn/sub"), this);
    _subtitleLabel->setPalette(p1);

    _subtitleLabel1  = new QLabel("-10s", this);
    _subtitleLabel1->setPalette(p1);

    _subtitleLabel2  = new QLabel("10s", this);
    _subtitleLabel2->setPalette(p1);

    _subtitleDetailLabel  = new QLabel(Lge->getLangageValue("TrackSyn/subTra"), this);
    _subtitleDetailLabel->setPalette(p1);

    _subtitleSlider = new CSlider2(this);
    _subtitleSlider->showAllLine();
    _subtitleSlider->initSize(300, SLIDER_HEIGHT);
    _subtitleSlider->setRange(0, 20);
    _subtitleSlider->setPageStep(1.0);
    //_subtitleSlider->setValue(10.0);
    int subtitleDelay = MediaPlayer->video()->getSpuDelay();
    if(subtitleDelay > 10 || subtitleDelay < -10)
        subtitleDelay = 0;
    _subtitleSlider->setValue(subtitleDelay + 10);

    //reset
    _resetBtn = new DYLabel(this,true);
    _resetBtn->setText(Lge->getLangageValue("TrackSyn/reset"));
    _resetBtn->setPalette(p1);
    _resetBtn->setAlignment(Qt::AlignCenter);
    connect(_resetBtn, SIGNAL(clicked()), SLOT(resetSlider()));

    connect(_audioSlider, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));
    connect(_subtitleSlider, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));

    updateUI(1.0);
}

void SyncTrackWidget::updateUI(const qreal &scaleRatio)
{
    _margin = 8 * scaleRatio;
    _sliderX = 32 * scaleRatio;

    _audioSlider->updateUI(scaleRatio);
    _subtitleSlider->updateUI(scaleRatio);

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setBold(true);
    font.setPixelSize(12 * scaleRatio);

    _audioLabel->setFont(font);
    _audioLabel1->setFont(font);
    _audioLabel2->setFont(font);
    _audioDetailLabel->setFont(font);
    _subtitleLabel->setFont(font);
    _subtitleLabel1->setFont(font);
    _subtitleLabel2->setFont(font);
    _subtitleDetailLabel->setFont(font);

    _audioLabel->adjustSize();
    _audioLabel1->adjustSize();
    _audioLabel2->adjustSize();
    _audioDetailLabel->adjustSize();
    _subtitleLabel->adjustSize();
    _subtitleLabel1->adjustSize();
    _subtitleLabel2->adjustSize();
    _subtitleDetailLabel->adjustSize();

    _resetBtn->setFont(font);
    _resetBtn->resize(120 * scaleRatio, 32 * scaleRatio);

    this->resize(380 * scaleRatio, 288 * scaleRatio);
}

void SyncTrackWidget::resetSlider()
{
    _audioSlider->setValue(10.0);
    _subtitleSlider->setValue(10.0);
}

void SyncTrackWidget::valueChanged(double v)
{
    v -= 10;
    CSlider2 * slider = qobject_cast<CSlider2 *>(this->sender());
    if(slider == _audioSlider) {
        MediaPlayer->audio()->setDelay(v);
    }
    else if(slider == _subtitleSlider) {
        MediaPlayer->video()->setSpuDelay(v);
    }
}

void SyncTrackWidget::resizeEvent(QResizeEvent *)
{
    //audio 相关坐标
    _audioLabel->move(_margin, _margin);

    _audioDetailLabel->move(_sliderX, _audioLabel->y() + _audioLabel->height() + _margin);

    _audioSlider->move(_sliderX, _audioDetailLabel->y() + _audioDetailLabel->height() + _margin);

    _audioLabel1->move(_sliderX, _audioSlider->y() + _audioSlider->height());
    _audioLabel2->move(_audioSlider->x() + _audioSlider->width() - _audioLabel2->width(),
                       _audioLabel1->y());

    //subtitle 相关坐标
    _subtitleLabel->move(_margin, _audioSlider->y() + _audioSlider->height() + _sliderX - _margin);

    _subtitleDetailLabel->move(_sliderX, _subtitleLabel->y() + _subtitleLabel->height() + _margin);

    _subtitleSlider->move(_sliderX, _subtitleDetailLabel->y() + _subtitleDetailLabel->height() + _margin);

    _subtitleLabel1->move(_sliderX, _subtitleSlider->y() + _subtitleSlider->height());
    _subtitleLabel2->move(_subtitleSlider->x() + _subtitleSlider->width() - _subtitleLabel2->width(),
                       _subtitleLabel1->y());
    //重置按钮
    _resetBtn->move((this->width() - _resetBtn->width()) / 2,
                    _subtitleSlider->y() + _subtitleSlider->height() + _sliderX);
}
