#ifndef ICOSVIDEO_H
#define ICOSVIDEO_H

#include <QWidget>
#include <QMovie>
#include "transeventlabel.h"
#include "transeventwidget.h"
#include "transeventicoclk.h"
#include "dylabel.h"
#include "dystructu.h"
#include "updateinterface.h"
class IcosVideo : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit IcosVideo(QWidget *parent = 0);

    void setDataSoure(const MEDIAINFO &info);
    MEDIAINFO getDataSoure();
    void setSelect(bool select = false,bool ctrl_down = false);
    bool isSelect();

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setplayFlag(bool state);
private:
    void Init();
    void upUI();
    void upfavoritesIco();
signals:
public slots:
    void favoritesClick();
    void upplayflag(const MEDIAINFO &videoinfo);

protected:
    void resizeEvent(QResizeEvent *);

public slots:
private:
    TransEventLabel*    _videoImage;
    TransEventLabel*    _videoName;
    TransEventLabel*    _videoDetails;
    TransEventLabel*    _playFlag;
    TransEventIcoClk*   _favirtesbtn;
    QMovie *            _playMovie;
    MEDIAINFO           _videoInfo;
    double              _scaleRatio;
    bool                _isSelect;
};

#endif // ICOSVIDEO_H
