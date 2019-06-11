#ifndef GGWIDGET_H
#define GGWIDGET_H

#include <QWidget>
#include <QDialog>
#include <qabstractanimation.h>
#include "updateinterface.h"
#include "dystructu.h"

class QLabel;
class DYLabel;
class DYCheckBox;
class SvgButton;
class QTimer;
class GGLabel;
class QPropertyAnimation;

class GGWidget : public QDialog,public UpdateInterface
{
    Q_OBJECT
public:
    explicit GGWidget(QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void clearWidget();
    void startShow(int number,QString adType);

private:
    void init();
    void upUI();
    void initGGWidget(AdvertStruct ggInfo);

    bool loadFileByAdNumber(int adNumber);
    AdvertStruct readJsonFile(QByteArray json,QString loadFolder);
    void adWidgetclose();
    bool haveConvert();  //判断video Covert是否安装过
    bool haveiPhoneManager();   //判断iPhoneManager是否已经安装
    bool haveDVDRipper();       //判断DVD Ripper是否安装过
    bool haveFullScreen();      //判断是否是全屏

signals:
    void clickWinKey(QString adType);

private slots:
    void animalFinish();
    void animalstateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State oldState);

    void showTimeOut();             // 显示时间完
    void closeTimeOut();            // 点击url后
    void clickCloseBtn();           // 点击关闭按钮
    void clickNoShow(bool check);
    void openUrl();
private:
    GGLabel*            _ico;
    GGLabel*            _texts[10];
    GGLabel*            _hosts[10];       // 热点先定义10个
    SvgButton*          _closeBtn;
    DYCheckBox*         _noshow;
    QPropertyAnimation* _showAnimal; //显示动画
    QTimer*             _widgeClostTimer;
    QTimer*             _widgeShowTimer;

    QWidget*        _adWidget;
    bool            _adShowState;       // true为显示,false为没显示
    AdvertStruct    _advertInfo;
    int             _curAdNumber;
    QString         _curAdType;
    QString         _showAdType;
    double          _scaleRatio;
    bool            _googleSap;     // 是否推送google统计
};

#endif // GGWIDGET_H
