#ifndef HARDWAREWIDGET_H
#define HARDWAREWIDGET_H


#include "updateinterface.h"
#include "dystructu.h"

#include <QLabel>
#include <QProcess>
#include <dycheckbox.h>
#include <QMap>
class DYLabel;
class ComboBox;
class QMovie;
class QFrame;
class HwSelWidget;
class HardwareWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit HardwareWidget(QWidget *parent = 0);
    ~HardwareWidget();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    int  getHardPosForKey(QString key);
    QStringList getHardKeys();
private:
    void Init();
    void UpUI();

    void initHwCodec();
    void initHwWidgetEnable(bool enable);

protected:
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void clickCheck(bool check,QString name);
    void hardwareTheadSlt(bool support,QString name);
    void clickAllow(bool check);
    void startScanHw();

private:
    QLabel* _title;
    QLabel* _titleLine;
    QLabel* _titleLine2;

    HwSelWidget*  _dxva2Widget;
    HwSelWidget*  _qsvWidget;
    HwSelWidget*  _cudaWidget;

    DYCheckBox* _useHardCodec;
    DYLabel*    _hwScanBtn;

    QMap<QString,int> _hardPosMap;
    int     _useHwIndex;
    int     _scanCount;
    bool    _initHw;

    double  _scaleRatio;
};

#endif // HARDWAREWIDGET_H
