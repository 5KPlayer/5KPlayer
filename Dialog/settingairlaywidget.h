#ifndef SETTINGAIRLAYWIDGET_H
#define SETTINGAIRLAYWIDGET_H

#include "updateinterface.h"
#include "textbutton.h"
#include "transeventlabel.h"
#include "svgbutton.h"
#include <QLineEdit>
#include <QMap>
class SettingAirlayWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SettingAirlayWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    int getAirPosForKey(QString str);
    QStringList getAirKeys();

private:
    void Init();
    void upUI();

private slots:
    void openRecordPaht();
    void selectRecordPath();
    void airOffClick();
    void airOnClick();
    void recordEditChange(const QString &editText);

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    TransEventLabel* _title;
    TransEventLabel* _titleLine;
    TransEventLabel* _titleLine2;

    TransEventLabel*    _sirplayService;
    TextButton *        _airplayOff;
    TextButton *        _airplayOn;

    TransEventLabel*    _status;
    TransEventLabel*    _statusInfo;
    TransEventLabel*    _computer;
    TransEventLabel*    _computerName;
    TransEventLabel*    _bonjourInfo;

    TransEventLabel*    _recordFolder;
    QLineEdit*          _recordEdit;
    SvgButton*          _recordSelect;
    SvgButton*          _recordOpen;

    double              _scaleRatio;
    QMap<QString,int>   _airPosMap;
};

#endif // SETTINGAIRLAYWIDGET_H
