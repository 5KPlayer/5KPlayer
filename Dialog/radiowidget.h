#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include "updateinterface.h"
class QLabel;
class DYLabel;
class QLineEdit;
class LiveRecentPlay;
class LiveIcoModeWidget;

class RadioWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit RadioWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void upRecentWidget();
protected:
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void showClickedRadioAddr(const QString &addr);
    void playRadio();
    void playLiveUrl(const QString &liveUrl);
    void slotupDataUI();
    void upLiveFavirtes(const QString &liveUrl, bool fav);
    void showLiveUrl(const QString &url);

private:
    QLabel*             _radioLabel;
    QLineEdit*          _radioAddr;
    DYLabel*            _playBtn;
    LiveRecentPlay*     _recentWidget;
    LiveIcoModeWidget*  _liveIcoWidget;

    int _space;
    QStringList         _liveUrlList;   // 固定的liveUrl
    QStringList         _liveImaList;   // 固定的liveUrl对应的图片
    QStringList         _favliveList;   // 收藏的liveUrl
};

#endif // RADIOWIDGET_H
