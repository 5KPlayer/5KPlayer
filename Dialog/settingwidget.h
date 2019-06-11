#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include "updateinterface.h"
#include <QMap>

class TransEvenListWidget;
class SetLeftNode;
class SvgTextButton;
class QScrollBar;
class QListWidgetItem;
class SettingGeneralWidget;
class SettingDownloadWidget;
class SettingSubtitleWidget;
class SettingAirlayWidget;
class SettingDLnaWidget;
class HardwareWidget;
class QSvgWidget;
class SvgButton;
class QLineEdit;
class QLabel;
class SearchingTool;
class SettingWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SettingWidget(QString initNodes = "General",QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private slots:
    void scrollValChange(int val);
    void itemClick(QListWidgetItem *it);
    void searEditChange(QString text);
    void doClickedSlt(const QString& str);
    void searDelClick();
    void searBtnClicl();
private:
    int  getPosForKey(QString key);
    void initSetPos(double scaleRatio);
    void upUI();
signals:
    void upCloseBtnEnable(bool enable);
protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void wheelEvent(QWheelEvent *);

private:
    QWidget*    _headW;
    QLabel*     _headTitle;
    QSvgWidget* _searchwidget;
    SvgButton*  _searchbtn;
    SvgButton*  _searchdelbtn;
    QLineEdit*  _searchEdit;
    SearchingTool*  _searTool;

    QString     _initNodes;

    QWidget*    _psetWidget;   // s主界面
    QScrollBar* _pScrollBar;    // 滚动条

    TransEvenListWidget* _listwidget;
    QListWidgetItem* _WidgetItem[11];
    SetLeftNode* _setGeneral;
    SetLeftNode* _setDownload;
    SetLeftNode* _setSubtitle;
    SetLeftNode* _setAirplay;
    SetLeftNode* _setHardware;
    SetLeftNode* _setDLnaware;

    SvgTextButton *_generalBtn;
    SvgTextButton *_downloadBtn;
    SvgTextButton *_subtitleBtn;

    SettingGeneralWidget *  _generalW;
    SettingDownloadWidget * _downloadW;
    SettingSubtitleWidget * _subtitleW;
    SettingAirlayWidget *   _airPlayW;
    SettingDLnaWidget*      _dlnaW;
    HardwareWidget*         _hardwareW;

    SvgTextButton *_curPressed;

    double         _screenRatio;

    QMap<QString,int> _setPosMap;
    QStringList       _allKeys;
};

#endif // SETTINGWIDGET_H
