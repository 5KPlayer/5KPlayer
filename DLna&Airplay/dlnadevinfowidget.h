#ifndef DLNADEVINFOWIDGET_H
#define DLNADEVINFOWIDGET_H

#include <QWidget>

#include "updateinterface.h"
#include "transeventwidget.h"
#include "transeventico.h"
#include "devstruct.h"

class QLabel;
class dlnaDevInfoWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit dlnaDevInfoWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setDevInfo(deviceInfo dev);
signals:
private:
    void upUI();
    void upInfo();
public slots:

protected:
    void resizeEvent(QResizeEvent *);

private:

    TransEventIco*  _ico;
    QLabel*         _devName;
    QLabel*         _devSupInfo;
    QLabel*         _line1;

    double      _scaleRatio;
    deviceInfo  _devInfo;
};

#endif // DLNADEVINFOWIDGET_H
