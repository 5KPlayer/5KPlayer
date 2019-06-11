#ifndef SYNCTRACKWIDGET_H
#define SYNCTRACKWIDGET_H

#include "updateinterface.h"

class QLabel;
class DYLabel;
class CSlider2;

class SyncTrackWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SyncTrackWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private slots:
    void resetSlider();
    void valueChanged(double);

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    QLabel *_audioLabel;
    QLabel *_audioLabel1;
    QLabel *_audioLabel2;
    QLabel *_audioDetailLabel;
    CSlider2 *_audioSlider;

    QLabel *_subtitleLabel;
    QLabel *_subtitleLabel1;
    QLabel *_subtitleLabel2;
    QLabel *_subtitleDetailLabel;
    CSlider2 *_subtitleSlider;

    DYLabel *_resetBtn;

    int _margin;
    int _sliderX;
};

#endif // SYNCTRACKWIDGET_H
