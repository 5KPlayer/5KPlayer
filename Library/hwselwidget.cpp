#include "hwselwidget.h"
#include "dycheckbox.h"
#include "hardwarecheckthead.h"
#include "globalarg.h"

#include <QLabel>
#include <QProgressBar>
#include <QDebug>
#include <QTimer>
HwSelWidget::HwSelWidget(QWidget *parent, QString text)
    : TransEventWidget(parent)
{
    _widName = text;
    this->setStyleSheet("background-color: rgb(32, 32, 33);");
    _isEnable = false;
    init();
}

void HwSelWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _checkBox->updateUI(scaleRatio);
    upUI();
}

void HwSelWidget::setCheckBoxState(bool check)
{
    _checkBox->setChecked(check);
}

bool HwSelWidget::getCheckBoxState()
{
    return _checkBox->isChecked();
}

bool HwSelWidget::getIsSupport()
{
    return _isSupport;
}

void HwSelWidget::setIsSupport(bool isSupport)
{
    _isSupport = isSupport;
    _checkFinish->show();
    _checkFinish->raise();
    QPixmap map;
    if(isSupport) {
        if(_isEnable)
            map.load(":/res/png/hwRight");
        else
            map.load(":/res/png/hwRightE");
    } else {
        if(_isEnable)
            map.load(":/res/png/hwError");
        else
            map.load(":/res/png/hwErrorE");

    }

    _checkFinish->setPixmap(map);

    if(!isSupport)
        _checkBox->setCheckEnable(isSupport);
}

void HwSelWidget::setWidgetName(QString name)
{
    _widgetName = name;
}

QString HwSelWidget::getWidgetName()
{
    return _widgetName;
}

void HwSelWidget::setIsEnable(bool enable)
{
    _isEnable = enable;
    QPixmap map;
    if(_isSupport) {
        if(_isEnable)
            map.load(":/res/png/hwRight");
        else
            map.load(":/res/png/hwRightE");
    } else {
        if(_isEnable)
            map.load(":/res/png/hwError");
        else
            map.load(":/res/png/hwErrorE");

    }

    _checkFinish->setPixmap(map);
    if(Global->isHwSupport(_widgetName))
        _checkBox->setCheckEnable(enable);
}

void HwSelWidget::init()
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    _checkBox       = new DYCheckBox(this,_widName);
    _checkFinish    = new QLabel(this);
    _proBar         = new QProgressBar(this);
    _hwCheck        = new HardwareCheckThead;
    _proTimer       = new QTimer(this);

    _checkBox->setTextFontBlod(true);
    _proBar->setTextVisible(false);
    _proBar->hide();
    _checkFinish->setScaledContents(true);

    connect(_proTimer,SIGNAL(timeout()),SLOT(proTimerOut()));
    _proTimer->setInterval(100);

    connect(_checkBox,SIGNAL(checkChange(bool)),SLOT(clickCheckBox(bool)));
    connect(_hwCheck,SIGNAL(sendHwSupport(bool)),SLOT(checkHwSupporSlt(bool)));

    _scaleRatio = Global->_fontRatio;
}

void HwSelWidget::upUI()
{
    _checkBox->setGeometry(10*_scaleRatio,1*_scaleRatio,300*_scaleRatio,28*_scaleRatio);
    _proBar->setGeometry(10*_scaleRatio,this->height()-2*_scaleRatio,this->width()-20*_scaleRatio,2*_scaleRatio);

    _checkFinish->setGeometry(300*_scaleRatio,7*_scaleRatio,16*_scaleRatio,12*_scaleRatio);
}

void HwSelWidget::startCheckHw()
{
    _proBar->setValue(0);
    _proBar->show();
    _proTimer->start();
    _checkFinish->hide();
    if(_hwCheck->isRunning()) {
        _hwCheck->terminate();
    }
    _hwCheck->setCheckType(_widgetName);
    _hwCheck->start();
}

void HwSelWidget::clickCheckBox(bool check)
{
    emit sendCheckClick(check,_widgetName);
}

void HwSelWidget::checkHwSupporSlt(bool support)
{
    _proBar->setValue(100);
    _proTimer->stop();
    _proBar->hide();
    setIsSupport(support);
    Global->setHwSupport(_widgetName,_isSupport);
    emit sendHardwareSpt(_isSupport,_widgetName);
}

void HwSelWidget::proTimerOut()
{
    int proValue = _proBar->value();
    if(proValue < 50) {
        proValue += 10;
    } else if(proValue >= 50 && proValue < 90) {
        proValue += 5;
    } else if(proValue >= 90 && proValue < 100){
        proValue += 1;
    }
    _proBar->setValue(proValue);
}

void HwSelWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
