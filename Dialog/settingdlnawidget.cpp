#include "settingdlnawidget.h"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QLineEdit>
#include <QDebug>


#include "combobox.h"
#include "globalarg.h"
#include "dylabel.h"
#include "dycheckbox.h"
#include "textbutton.h"
#include "DLna/dlnamanager.h"
#include "playermain.h"

SettingDLnaWidget::SettingDLnaWidget(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    _title              = new QLabel("DLNA",this);
    _titleLine          = new QLabel(this);
    _titleLine2         = new QLabel(this);

    _dlnaSerDes         = new TransEventLabel(this);
    _dlnaService        = new TransEventLabel(this);
    _dlnaServOff        = new TextButton(Lge->getLangageValue("SetGay/off"),40,24,this);
    _dlnaServOn         = new TextButton(Lge->getLangageValue("SetGay/on"),40,24,this);
    _dlnaServOn->setPosition(TextButton::Right);


    _serverstateLabel   = new TransEventLabel(this);
    _serverstateText    = new TransEventLabel(this);

    _serverNameLabel    = new TransEventLabel(this);
    _serverNameEdit     = new QLineEdit(this);

    _dlnaDevDes         = new TransEventLabel(this);
    _dlnaDevice         = new TransEventLabel(this);
    _dlnaDevOff         = new TextButton(Lge->getLangageValue("SetGay/off"),40,24,this);
    _dlnaDevOn          = new TextButton(Lge->getLangageValue("SetGay/on"),40,24,this);
    _dlnaDevOn->setPosition(TextButton::Right);

    _devicestateLabel   = new TransEventLabel(this);
    _devicestateText    = new TransEventLabel(this);

    _deviceNameLabel    = new TransEventLabel(this);
    _deviceNameEdit     = new QLineEdit(this);

    _usrMaxPlay         = new DYCheckBox(this,Lge->getLangageValue("SetDlna/maxplay"));
    _dlnaUserIpLabel    = new TransEventLabel(this);
    _dlnaUserIpComboBox = new ComboBox(this);
    _dlnaAppBtn         = new DYLabel(this,true);

    _scaleRatio = 1.0;
    _title->setPalette(p1);
    _usrMaxPlay->setPalette(p1);
    _usrMaxPlay->setTextFontBlod(true);
    _usrMaxPlay->setChecked(Global->getDLnaUserMaxPlay());

    _dlnaSerDes->setPalette(p1);
    _dlnaSerDes->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _dlnaSerDes->setText(Lang("SetDlna/serdes"));
    _dlnaService->setPalette(p1);
    _dlnaService->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _dlnaService->setText(Lang("SetDlna/service"));
    _serverstateLabel->setPalette(p1);
    _serverstateLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _serverstateLabel->setText(Lang("SetDlna/serstatus"));
    _serverstateText->setPalette(p1);
    _serverstateText->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _serverNameLabel->setPalette(p1);
    _serverNameLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _serverNameLabel->setText(Lang("SetDlna/sername"));

    _dlnaDevDes->setPalette(p1);
    _dlnaDevDes->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _dlnaDevDes->setText(Lang("SetDlna/devdes"));
    _dlnaDevice->setPalette(p1);
    _dlnaDevice->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _dlnaDevice->setText(Lang("SetDlna/device"));
    _devicestateLabel->setPalette(p1);
    _devicestateLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _devicestateLabel->setText(Lang("SetDlna/devstatus"));
    _devicestateText->setPalette(p1);
    _devicestateText->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _deviceNameLabel->setPalette(p1);
    _deviceNameLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _deviceNameLabel->setText(Lang("SetDlna/devname"));
    _dlnaUserIpLabel->setPalette(p1);
    _dlnaUserIpLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _dlnaUserIpLabel->setText(Lang("SetDlna/conIp"));
    _dlnaAppBtn->setAlignment(Qt::AlignCenter);

    _dlnaAppBtn->setText(Lang("SetDlna/appbtn"));

    _serverNameEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _deviceNameEdit->setContextMenuPolicy(Qt::NoContextMenu);

    _titleLine->setStyleSheet("background-color: rgb(20, 20, 21);");
    _titleLine2->setStyleSheet("background-color: rgb(71, 71, 75);");

    if(Global->getDLnaService()) {
        _dlnaServOff->recover();
        _dlnaServOn->click();
        _serverstateText->setText(Lang("SetDlna/run"));
    } else {
        _dlnaServOff->click();
        _dlnaServOn->recover();
        _serverstateText->setText(Lang("SetDlna/stop"));
    }
    if(Global->getDLnaDevice()) {
        _dlnaDevOff->recover();
        _dlnaDevOn->click();
        _devicestateText->setText(Lang("SetDlna/run"));
    } else {
        _dlnaDevOff->click();
        _dlnaDevOn->recover();
        _devicestateText->setText(Lang("SetDlna/stop"));
    }

    _serverNameEdit->setText(Global->getDLnaServiceName());
    _serverNameEdit->setCursorPosition(0);
    _deviceNameEdit->setText(Global->getDLnaDeviceName());
    _deviceNameEdit->setCursorPosition(0);

    // 获取所有ip地址
    QStringList retv;
    foreach (QNetworkInterface nic, QNetworkInterface::allInterfaces())
    {
        if ((nic.flags() & QNetworkInterface::IsUp) &&
            (nic.flags() & QNetworkInterface::IsRunning) &&
            (nic.flags() & ~QNetworkInterface::IsLoopBack))
        {
            foreach(QNetworkAddressEntry entry, nic.addressEntries())
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol && !entry.ip().isLoopback())
                {
                    retv.push_back(entry.ip().toString());
                }
            }
        }
    }
    QString dlnaIp = "";
    foreach(QString ipItem, retv)
    {
        if(ipItem != "127.0.0.1")
            _dlnaUserIpComboBox->addItem(ipItem);
        if(!Global->getDLnaUserIp().isEmpty()
           && ipItem == Global->getDLnaUserIp()) {
            dlnaIp = Global->getDLnaUserIp();
        }
    }
    if(dlnaIp.isEmpty()) {
        _dlnaUserIpComboBox->setCurrentIndex(0);
    } else {
        _dlnaUserIpComboBox->setCurrentText(dlnaIp);
    }

    connect(_dlnaServOn,SIGNAL(clicked()),SLOT(dlnaServerOpen()));
    connect(_dlnaServOff,SIGNAL(clicked()),SLOT(dlnaServerStop()));
    connect(_dlnaDevOn,SIGNAL(clicked()),SLOT(dlnaDeviceOpen()));
    connect(_dlnaDevOff,SIGNAL(clicked()),SLOT(dlnaDeviceStop()));
    connect(_serverNameEdit,SIGNAL(textChanged(QString)),SLOT(dlnaServerNameChange(QString)));
    connect(_deviceNameEdit,SIGNAL(textChanged(QString)),SLOT(dlnaDeviceNameChange(QString)));
    connect(_dlnaUserIpComboBox,SIGNAL(currentIndexChanged(QString)),SLOT(dlnaUserIpChange(QString)));
    connect(_usrMaxPlay,SIGNAL(checkChange(bool)),SLOT(dlnaUserMaxPlay(bool)));
    connect(_dlnaAppBtn,SIGNAL(clicked()),SLOT(appleBtnClick()));
}

void SettingDLnaWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

int SettingDLnaWidget::getDlnaPosForKey(QString key)
{
    return _dlnaPosMap.value(key);
}

QStringList SettingDLnaWidget::getDlnaKeys()
{
    return _dlnaPosMap.keys();
}

void SettingDLnaWidget::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setBold(true);
    font.setPixelSize(14 * _scaleRatio);
    _title->setFont(font);

    font.setPixelSize(12 * _scaleRatio);

    _dlnaService->setFont(font);
    _serverstateLabel->setFont(font);
    _serverstateText->setFont(font);
    _serverNameLabel->setFont(font);
    _serverNameEdit->setFont(font);
    _dlnaDevice->setFont(font);
    _devicestateLabel->setFont(font);
    _devicestateText->setFont(font);
    _deviceNameLabel->setFont(font);
    _deviceNameEdit->setFont(font);
    _dlnaUserIpLabel->setFont(font);
    _dlnaUserIpComboBox->setComFont(font);
    _dlnaAppBtn->setFont(font);
    _usrMaxPlay->setFont(font);
    _dlnaSerDes->setFont(font);
    _dlnaDevDes->setFont(font);
    _dlnaSerDes->adjustSize();
    _dlnaDevDes->adjustSize();
    _dlnaDevDes->setWordWrap(true);


    _dlnaServOff->updateUI(_scaleRatio);
    _dlnaServOn->updateUI(_scaleRatio);
    _dlnaDevOff->updateUI(_scaleRatio);
    _dlnaDevOn->updateUI(_scaleRatio);
    _usrMaxPlay->updateUI(_scaleRatio);

    int height = 20*_scaleRatio;
    int hsplit = 35*_scaleRatio;
    double xpos = 4*_scaleRatio;

    int titleW  = _title->fontMetrics().width(_title->text());
    _title->setGeometry(xpos,       5*_scaleRatio,titleW,height);
    int lineW   = this->width()- (_title->x() + _title->width() + 28*_scaleRatio);
    _titleLine->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,15*_scaleRatio,lineW, 1*_scaleRatio);
    _titleLine2->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,16*_scaleRatio,lineW, 1*_scaleRatio);

    _dlnaService->setGeometry(xpos,40* _scaleRatio,210 * _scaleRatio,height);
    _dlnaServOff->move(218*_scaleRatio,_dlnaService->y());
    _dlnaServOn->move(258*_scaleRatio,_dlnaService->y());
    _dlnaSerDes->move(308*_scaleRatio,_dlnaService->y()+2* _scaleRatio);
    _dlnaPosMap.insert(Lang("SetMain/dlna") + ":" + LangNoColon("SetDlna/service"),40*_scaleRatio);

    _serverstateLabel->setGeometry(xpos,_dlnaService->y()+hsplit,210 * _scaleRatio,height);
    _serverstateText->setGeometry(xpos+218*_scaleRatio,_dlnaService->y()+hsplit,170 * _scaleRatio,height);
    _dlnaPosMap.insert(Lang("SetMain/dlna") + ":" + LangNoColon("SetDlna/serstatus"),_serverstateLabel->y());

    _serverNameLabel->setGeometry(xpos,_serverstateLabel->y()+hsplit,210 * _scaleRatio,height);
    _serverNameEdit->setGeometry(xpos+218*_scaleRatio,_serverstateLabel->y()+hsplit,250 * _scaleRatio,height);
    _dlnaPosMap.insert(Lang("SetMain/dlna") + ":" + LangNoColon("SetDlna/sername"),_serverNameLabel->y());

    _dlnaDevice->setGeometry(xpos,_serverNameLabel->y() + 40*_scaleRatio,210 * _scaleRatio,height);
    _dlnaDevOff->move(218*_scaleRatio,_dlnaDevice->y());
    _dlnaDevOn->move(258*_scaleRatio,_dlnaDevice->y());
    _dlnaDevDes->move(308*_scaleRatio,_dlnaDevice->y()-2* _scaleRatio);
    _dlnaPosMap.insert(Lang("SetMain/dlna") + ":" + LangNoColon("SetDlna/device"),_dlnaDevice->y());

    _devicestateLabel->setGeometry(xpos,_dlnaDevice->y()+hsplit,210 * _scaleRatio,height);
    _devicestateText->setGeometry(xpos+218*_scaleRatio,_dlnaDevice->y()+hsplit,170 * _scaleRatio,height);
    _dlnaPosMap.insert(Lang("SetMain/dlna") + ":" + LangNoColon("SetDlna/devstatus"),_devicestateLabel->y());

    _deviceNameLabel->setGeometry(xpos,_devicestateLabel->y()+hsplit,210 * _scaleRatio,height);
    _deviceNameEdit->setGeometry(xpos+218*_scaleRatio,_devicestateLabel->y()+hsplit,250 * _scaleRatio,height);
    _dlnaPosMap.insert(Lang("SetMain/dlna") + ":" + LangNoColon("SetDlna/devname"),_deviceNameLabel->y());

    _dlnaUserIpLabel->setGeometry(xpos, _deviceNameLabel->y()+ 40 * _scaleRatio,210 * _scaleRatio,height);
    _dlnaUserIpComboBox->setGeometry(xpos+218*_scaleRatio, _dlnaUserIpLabel->y(),170 * _scaleRatio, height);
    _dlnaAppBtn->setGeometry(_dlnaUserIpComboBox->x() + _dlnaUserIpComboBox->width() + 20 * _scaleRatio,
                          _dlnaUserIpLabel->y(),60 * _scaleRatio, height);
    _dlnaPosMap.insert(Lang("SetMain/dlna") + ":" + LangNoColon("SetDlna/conIp"),_dlnaUserIpLabel->y());

    _usrMaxPlay->setGeometry(130*_scaleRatio, _dlnaUserIpLabel->y()+ 40 * _scaleRatio,220* _scaleRatio,height);

    _dlnaUserIpComboBox->upStyleSheet(_scaleRatio);
    _serverNameEdit->setStyleSheet( Global->lineEditStyleSheet(4 * _scaleRatio));
    _deviceNameEdit->setStyleSheet( Global->lineEditStyleSheet(4 * _scaleRatio));
}

void SettingDLnaWidget::dlnaUserIpChange(QString ip)
{
    Global->setDLnaUserIp(ip);
}

void SettingDLnaWidget::dlnaServerOpen()
{
    _dlnaServOff->recover();
    _serverstateText->setText(Lang("SetDlna/run"));
    Global->setDLnaService(true);
    DLNA->runServer();
}

void SettingDLnaWidget::dlnaServerStop()
{
    _dlnaServOn->recover();
    _serverstateText->setText(Lang("SetDlna/stop"));
    Global->setDLnaService(false);
    DLNA->stopServer();
}

void SettingDLnaWidget::dlnaServerNameChange(QString name)
{
    Global->setDLnaServiceName(name);
}

void SettingDLnaWidget::dlnaDeviceOpen()
{
    _dlnaDevOff->recover();
    Global->setDLnaDevice(true);
    PMW->dlnaOpenDevice();
    _devicestateText->setText(Lang("SetDlna/run"));
}

void SettingDLnaWidget::dlnaDeviceStop()
{
    _dlnaDevOn->recover();
    Global->setDLnaDevice(false);
    PMW->dlnaStopDevice();
    _devicestateText->setText(Lang("SetDlna/stop"));
}

void SettingDLnaWidget::dlnaDeviceNameChange(QString name)
{
    Global->setDLnaDeviceName(name);
}

void SettingDLnaWidget::appleBtnClick()
{
    PMW->dlnaReInit();
}

void SettingDLnaWidget::dlnaUserMaxPlay(bool check)
{
    Global->setDLnaUserMaxPlay(check);
}

void SettingDLnaWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
