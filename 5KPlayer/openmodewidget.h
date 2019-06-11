#ifndef OPENMODEWIDGET_H
#define OPENMODEWIDGET_H

#include <QWidget>
#include "transeventwidget.h"
#include "updateinterface.h"

class SvgButton;

class OpenModeWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit OpenModeWidget(QWidget *parent = 0);
    ~OpenModeWidget(){}

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void openAnimal(bool b);

Q_SIGNALS:
    void itemClicked(QString item);

private Q_SLOTS:
    void openVideoDialog();
    void openDvdDialog();
    void openMusicDialog();
    void openRadioDialog();

    void openYouTubeDialog();
    void openAirPlayDialog();

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    SvgButton *_videoBtn;
    SvgButton *_dvdBtn;
    SvgButton *_musicBtn;
    SvgButton *_radioBtn;
    SvgButton *_youtubeBtn;
    SvgButton *_airPlay;
};

#endif // OPENMODEWIDGET_H
