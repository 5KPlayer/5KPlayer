#ifndef SETTINGDLNAWIDGET_H
#define SETTINGDLNAWIDGET_H

#include "updateinterface.h"
#include "transeventlabel.h"

#include <QMap>

class ComboBox;
class QLineEdit;
class TextButton;
class DYLabel;
class DYCheckBox;
class SettingDLnaWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SettingDLnaWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    int  getDlnaPosForKey(QString key);
    QStringList getDlnaKeys();

private:
    void upUI();
private slots:
    void dlnaUserIpChange(QString ip);
    void dlnaServerOpen();
    void dlnaServerStop();
    void dlnaServerNameChange(QString name);
    void dlnaDeviceOpen();
    void dlnaDeviceStop();
    void dlnaDeviceNameChange(QString name);
    void appleBtnClick();
    void dlnaUserMaxPlay(bool check);
protected:
    virtual void resizeEvent(QResizeEvent *);
public slots:
private:
    QLabel*     _title;
    QLabel*     _titleLine;
    QLabel*     _titleLine2;

    TransEventLabel*    _dlnaSerDes;
    TransEventLabel*    _dlnaService;
    TextButton *        _dlnaServOff;
    TextButton *        _dlnaServOn;

    TransEventLabel*    _serverstateLabel;
    TransEventLabel*    _serverstateText;

    TransEventLabel*    _serverNameLabel;
    QLineEdit*          _serverNameEdit;

    TransEventLabel*    _dlnaDevDes;
    TransEventLabel*    _dlnaDevice;
    TextButton *        _dlnaDevOff;
    TextButton *        _dlnaDevOn;

    TransEventLabel*    _devicestateLabel;
    TransEventLabel*    _devicestateText;

    QLabel*             _deviceNameLabel;
    QLineEdit*          _deviceNameEdit;

    DYCheckBox*         _usrMaxPlay;
    TransEventLabel*    _dlnaUserIpLabel;
    ComboBox*           _dlnaUserIpComboBox;
    DYLabel*            _dlnaAppBtn;

    double           _scaleRatio;
    QMap<QString,int>_dlnaPosMap;
};

#endif // SETTINGDLNAWIDGET_H
