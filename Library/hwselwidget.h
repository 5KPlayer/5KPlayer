#ifndef HWSELWIDGET_H
#define HWSELWIDGET_H

#include <QWidget>

#include "updateinterface.h"
#include "transeventwidget.h"

class DYCheckBox;
class QLabel;
class QProgressBar;
class HardwareCheckThead;
class HwSelWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit HwSelWidget(QWidget *parent = 0,QString text = "");

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void setCheckBoxState(bool check);
    bool getCheckBoxState();

    bool getIsSupport();
    void setIsSupport(bool isSupport);

    void setWidgetName(QString name);
    QString getWidgetName();

    void setIsEnable(bool enable);
    void startCheckHw();
private:
    void init();
    void upUI();
private slots:
    void clickCheckBox(bool check);
    void checkHwSupporSlt(bool support);
    void proTimerOut();
signals:
    void sendCheckClick(bool check,QString name);
    void sendHardwareSpt(bool support,QString name);
protected:
    void resizeEvent(QResizeEvent *);
private:
    DYCheckBox*     _checkBox;
    QLabel*         _checkFinish;
    QProgressBar*   _proBar;

    HardwareCheckThead* _hwCheck;
    QTimer*             _proTimer;

    QString     _widName;
    double      _scaleRatio;
    bool        _isSupport;
    bool        _isEnable;

    QString     _widgetName;
};

#endif // HWSELWIDGET_H
