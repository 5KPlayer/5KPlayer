#include "videotunerdialog.h"
#include <QLabel>
#include "cslider2.h"
#include "dylabel.h"
#include "dymediaplayer.h"
#include "globalarg.h"
#include <QtDebug>

#define SLIDER_HEIGHT 40

VideoTunerDialog::VideoTunerDialog(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    _contrastLabel  = new QLabel(Lge->getLangageValue("Videotuner/contrast"), this);
    _contrastLabel->setPalette(p1);
    _contrastSlider = new CSlider2(this);
    _contrastSlider->initSize(300, SLIDER_HEIGHT);
    _contrastSlider->setRange(0, 2);
    _contrastSlider->setPageStep(0.1f);
    _contrastSlider->setValue(MediaPlayer->video()->getContrast());

    _brightnessLabel  = new QLabel(Lge->getLangageValue("Videotuner/Brightness"), this);
    _brightnessLabel->setPalette(p1);
    _brightnessSlider = new CSlider2(this);
    _brightnessSlider->initSize(300, SLIDER_HEIGHT);
    _brightnessSlider->setRange(0, 2);
    _brightnessSlider->setPageStep(0.1f);
    _brightnessSlider->setValue(MediaPlayer->video()->getBrightness());

#ifdef HUE
    _hueLabel  = new QLabel(Lge->getLangageValue("Videotuner/Hue"), this);
    _hueLabel->setPalette(p1);
    _hueSlider = new CSlider2(this);
    _hueSlider->initSize(300, SLIDER_HEIGHT);
    _hueSlider->setRange(0, 6);
    _hueSlider->setPageStep(0.1);
    _hueSlider->setValue(MediaPlayer->video()->getHue());
    connect(_hueSlider, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));
#endif

    _saturationLabel  = new QLabel(Lge->getLangageValue("Videotuner/Saturation"), this);
    _saturationLabel->setPalette(p1);
    _saturationSlider = new CSlider2(this);
    _saturationSlider->initSize(300, SLIDER_HEIGHT);
    _saturationSlider->setRange(0, 3);
    _saturationSlider->setPageStep(0.1f);
    _saturationSlider->setValue(MediaPlayer->video()->getSaturation());

    _gammaLabel  = new QLabel(Lge->getLangageValue("Videotuner/Gamma"), this);
    _gammaLabel->setPalette(p1);
    _gammaSlider = new CSlider2(this);
    _gammaSlider->initSize(300, SLIDER_HEIGHT);
    _gammaSlider->setRange(0, 10);
    _gammaSlider->setPageStep(0.1f);
    _gammaSlider->setValue(MediaPlayer->video()->getGamma());

    _resetBtn = new DYLabel(this,true);
    _resetBtn->setText(Lge->getLangageValue("Videotuner/Reset"));
    _resetBtn->setPalette(p1);
    _resetBtn->setAlignment(Qt::AlignCenter);
    connect(_resetBtn, SIGNAL(clicked()), SLOT(resetSlider()));

    connect(_contrastSlider, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));
    connect(_brightnessSlider, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));
    connect(_saturationSlider, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));
    connect(_gammaSlider, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));

    updateUI(1.0);
}

void VideoTunerDialog::valueChanged(const double &v)
{
    CSlider2 * slider = qobject_cast<CSlider2 *>(this->sender());
    if(slider == _contrastSlider) {
        MediaPlayer->video()->setContrast(v);
    }
    else if(slider == _brightnessSlider) {
        MediaPlayer->video()->setBrightness(v);
    }
#ifdef HUE
    else if(slider == _hueSlider) {
        MediaPlayer->video()->setHue(v);
    }
#endif
    else if(slider == _saturationSlider) {
        MediaPlayer->video()->setSaturation(v);
    }
    else if(slider == _gammaSlider) {
        MediaPlayer->video()->setGamma(v);
    }
}

void VideoTunerDialog::updateUI(const qreal &scaleRatio)
{
    _hMargin = 4 * scaleRatio;
    _sliderX = 120 * scaleRatio;

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setBold(true);
    font.setPixelSize(12 * scaleRatio);

#ifdef HUE
    _hueSlider->updateUI(scaleRatio);
    _hueLabel->setFont(font);
    _hueLabel->adjustSize();
#endif

    _contrastSlider->updateUI(scaleRatio);
    _brightnessSlider->updateUI(scaleRatio);
    _saturationSlider->updateUI(scaleRatio);
    _gammaSlider->updateUI(scaleRatio);

    _contrastLabel->setFont(font);
    _brightnessLabel->setFont(font);
    _saturationLabel->setFont(font);
    _gammaLabel->setFont(font);

    _contrastLabel->adjustSize();
    _brightnessLabel->adjustSize();
    _saturationLabel->adjustSize();
    _gammaLabel->adjustSize();

    _resetBtn->setFont(font);
    _resetBtn->resize(120 * scaleRatio, 32 * scaleRatio);

#ifdef HUE
    this->resize(460 * scaleRatio, 280 * scaleRatio);
#else
    this->resize(460 * scaleRatio, 240 * scaleRatio);
#endif
}

void VideoTunerDialog::resetSlider()
{
    _contrastSlider->setValue(1.0);
    _brightnessSlider->setValue(1.0);
#ifdef HUE
    _hueSlider->setValue(1.0);
#endif
    _saturationSlider->setValue(1.0);
    _gammaSlider->setValue(1.0);
}

void VideoTunerDialog::resizeEvent(QResizeEvent *)
{
    //Contrast
    _contrastSlider->move(_sliderX, _hMargin);
    _contrastLabel->move(_contrastSlider->x() - _contrastLabel->width(),
                         _contrastSlider->y() + (_contrastSlider->height() - _contrastLabel->height()) / 2);

    //Brightness
    _brightnessSlider->move(_sliderX, _contrastSlider->y() + _contrastSlider->height() + _hMargin);
    _brightnessLabel->move(_brightnessSlider->x() - _brightnessLabel->width(),
                           _brightnessSlider->y() + (_brightnessSlider->height() - _brightnessLabel->height()) / 2);

#ifdef HUE
    // Hue
    _hueSlider->move(_sliderX, _brightnessSlider->y() + _brightnessSlider->height() + _hMargin);
    _saturationSlider->move(_sliderX, _hueSlider->y() + _hueSlider->height() + _hMargin);
    _hueLabel->move(_hueSlider->x() - _hueLabel->width(),
                    _hueSlider->y() + (_hueSlider->height() - _hueLabel->height()) / 2);
#else
    _saturationSlider->move(_sliderX, _brightnessSlider->y() + _brightnessSlider->height() + _hMargin);
#endif
    _saturationLabel->move(_saturationSlider->x() - _saturationLabel->width(),
                           _saturationSlider->y() + (_saturationSlider->height() - _saturationLabel->height()) / 2);

    //Gamma
    _gammaSlider->move(_sliderX, _saturationSlider->y() + _saturationSlider->height() + _hMargin);
    _gammaLabel->move(_gammaSlider->x() - _gammaLabel->width(),
                      _gammaSlider->y() + (_gammaSlider->height() - _gammaLabel->height()) / 2);






    _resetBtn->move((this->width() - _resetBtn->width()) / 2,
                    _gammaSlider->y() + _gammaSlider->height() + _hMargin * 4);
}
