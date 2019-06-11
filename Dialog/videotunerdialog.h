#ifndef VIDEOTUNERDIALOG_H
#define VIDEOTUNERDIALOG_H

#include "updateinterface.h"

class QLabel;
class DYLabel;
class CSlider2;

class VideoTunerDialog : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit VideoTunerDialog(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private slots:
    void resetSlider();
    void valueChanged(const double &);

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    QLabel *_contrastLabel;
    CSlider2 *_contrastSlider;

    QLabel *_brightnessLabel;
    CSlider2 *_brightnessSlider;

#ifdef HUE
    QLabel *_hueLabel;
    CSlider2 *_hueSlider;
#endif

    QLabel *_saturationLabel;
    CSlider2 *_saturationSlider;

    QLabel *_gammaLabel;
    CSlider2 *_gammaSlider;

    DYLabel *_resetBtn;

    int _sliderX;
    int _hMargin;
};

#endif // VIDEOTUNERDIALOG_H
