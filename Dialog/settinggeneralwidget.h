#ifndef SETTINGGENERALWIDGET_H
#define SETTINGGENERALWIDGET_H

#include "updateinterface.h"
#include "transeventlabel.h"
#include <QMap>
class ComboBox;
class QLineEdit;
class SvgButton;

class SettingGeneralWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SettingGeneralWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);
    int  getGenPosForKey(QString str);
    QStringList getGenKeys();

private:
    void Init();
    void upUI();

private slots:
    void openSnapshotPath();
    void selectSnapshotPath();
    void openConvertPath();
    void selectConvertPath();
    void snapPathChange(const QString &snapPath);
    void convertPathChange(const QString &convertPath);
protected:
    virtual void resizeEvent(QResizeEvent *);
private:
    TransEventLabel* _title;
    TransEventLabel* _titleLine;
    TransEventLabel* _titleLine2;
    TransEventLabel* _stayOnTop;
    ComboBox*       _styOnTopBox;
    TransEventLabel* _closePlay;
    ComboBox*       _closePlayBox;
    TransEventLabel* _openatLogin;
    ComboBox*       _openatLoginBox;
    TransEventLabel* _checkUpdates;
    ComboBox*       _checkUpdatesBox;
    TransEventLabel* _snapshotPath;
    QLineEdit*       _snapshotEdit;
    TransEventLabel* _convertPath;
    QLineEdit*       _convertEdit;

    SvgButton*   _snapshotSelect;
    SvgButton*   _snapshotOpen;
    SvgButton*   _convertSelect;
    SvgButton*   _convertOpen;

    double           _scaleRatio;

    QMap<QString,int> _genPosMap;
};

#endif // SETTINGGENERALWIDGET_H
