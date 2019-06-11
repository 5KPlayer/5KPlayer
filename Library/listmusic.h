#ifndef LISTMUSIC_H
#define LISTMUSIC_H

#include <QWidget>
#include "updateinterface.h"
#include "transeventwidget.h"
#include "transeventlabel.h"
#include "transeventicoclk.h"
#include <QMovie>
#include "dylabel.h"
#include "dystructu.h"
class ListMusic : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit ListMusic(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setDataSoure(const MEDIAINFO &mediaInfo);
    MEDIAINFO getDataSoure();

    void setIsSelect(bool select,bool ctrldown);
    bool getIsSelect() {return _mediaInfo.isSelect;}

    void setplayFlag(bool state);

private slots:
    void favoritesClick();
    void upplayflag(const MEDIAINFO &mediainfo);
private:
    void Init();
    void upUI();
    void upfavoritesbtn();
    void upTitle();
protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private:
    TransEventLabel* _name;
    TransEventLabel* _info;
    TransEventLabel* _type;
    TransEventLabel* _bitrate;
    TransEventLabel* _filetime;
    TransEventLabel* _playFlag;
    TransEventIcoClk*_favorites;
    QMovie*  _playMovie;
    MEDIAINFO _mediaInfo;
    double _scaleRatio;

};

#endif // LISTMUSIC_H
