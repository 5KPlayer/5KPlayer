#ifndef LIVEICO_H
#define LIVEICO_H

#include <QWidget>
#include "transeventicoclk.h"
#include "transeventlabel.h"
#include "transeventico.h"
#include "updateinterface.h"

class LiveIco : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit LiveIco(QWidget *parent = 0);

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void setLiveData(const QString &liveUrl,const QString &liveImage, const bool &favires);
    QString getLiveData() const;

signals:
    void updateLiveWidget(const QString &urlPath, const bool &favires);

public slots:
    void favirtesClick();

private:
    void Init();

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    TransEventLabel*    _liveImage;
    TransEventIco*      _liveType;
    TransEventIcoClk*   _favirtesbtn;

    QString             _liveUrl;
    bool                _bFavires;
    double              _scaleRatio;
};

#endif // LIVEICO_H
