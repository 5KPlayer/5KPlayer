#include "settingairlaywidget.h"
#include "globalarg.h"
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QDesktopServices>
#include <QtConcurrent/QtConcurrent>
#include "openfile.h"
#include "playermain.h"

SettingAirlayWidget::SettingAirlayWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    Init();
}

void SettingAirlayWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _recordSelect->updateUI(scaleRatio);
    _recordOpen->updateUI(scaleRatio);
    upUI();
}

int SettingAirlayWidget::getAirPosForKey(QString str)
{
    return _airPosMap.value(str);
}

QStringList SettingAirlayWidget::getAirKeys()
{
    return _airPosMap.keys();
}

void SettingAirlayWidget::Init()
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    _title      = new TransEventLabel(this);
    _title->setPalette(p1);
    _title->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    _title->setText(Lge->getLangageValue("SetMain/airplay"));
    _titleLine  = new TransEventLabel(this);
    _titleLine2 = new TransEventLabel(this);
    _titleLine->setStyleSheet("background-color: rgb(20, 20, 21);");
    _titleLine2->setStyleSheet("background-color: rgb(71, 71, 75);");

    _sirplayService = new TransEventLabel(this);

    _status         = new TransEventLabel(this);
    _statusInfo     = new TransEventLabel(this);
    _computer       = new TransEventLabel(this);
    _computerName   = new TransEventLabel(this);
    _bonjourInfo    = new TransEventLabel(this);

    _recordFolder   = new TransEventLabel(this);
    _recordEdit     = new QLineEdit(this);
    _recordSelect   = new SvgButton(this);
    _recordOpen     = new SvgButton(this);

    _recordSelect->initSize(20,20);
    _recordOpen->initSize(20,20);
    _airplayOff     = new TextButton(Lge->getLangageValue("SetGay/off"),40,24,this);
    _airplayOn      = new TextButton(Lge->getLangageValue("SetGay/on"),40,24,this);
    _airplayOn->setPosition(TextButton::Right);

    _recordEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _airplayOff->setDisabled(true);
    _airplayOn->setDisabled(true);

    connect(_airplayOff, SIGNAL(clicked()), SLOT(airOffClick()));
    connect(_airplayOn,  SIGNAL(clicked()), SLOT(airOnClick()));

    _computerName->setText(Global->localName());
    if(PMW->airplayStarted()) {
        _airplayOn->click();
        _statusInfo->setText(Lang("SetGay/run"));
    }
    else {
        _airplayOff->click();
        _statusInfo->setText(Lang("SetGay/stop"));
    }
    _bonjourInfo->setTextFormat(Qt::RichText);
    _bonjourInfo->setWordWrap(true);
    int state = Global->bonjourServiceState();
    if(state < 0) {
        _bonjourInfo->setText(Lang("SetGay/NoBonj") +
                              " <a style='color: rgb(53,128,167);' " +
                               QString("href=https://support.apple.com/kb/DL999>%1</a>")
                              .arg(Lang("SetGay/downNow")));
        _bonjourInfo->setOpenExternalLinks(true);
    } else if(state != 1) {
        _bonjourInfo->setText(Lang("SetGay/noRunBonj") +
                              " <a style='color: rgb(53,128,167);' " +
                              QString("href=services.msc>%1</a>")
                              .arg(Lang("SetMain/set")));
        connect(_bonjourInfo, &QLabel::linkActivated, [](QString){
            QtConcurrent::run([](){
                QProcess::execute("cmd.exe /c services.msc");
            });
        });
    } else {
        _airplayOff->setDisabled(false);
        _airplayOn->setDisabled(false);
    }

    _sirplayService->setStyleSheet("color:rgb(255,255,255)");
    _sirplayService->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _sirplayService->setText(Lang("SetGay/AirSev"));

    _status->setStyleSheet("color:rgb(255,255,255)");
    _status->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _status->setText(Lang("SetGay/status"));

    _statusInfo->setStyleSheet("color:rgb(255,255,255)");
    _statusInfo->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    _bonjourInfo->setStyleSheet("color:rgb(255,255,255)");
    _bonjourInfo->setAlignment(Qt::AlignLeft|Qt::AlignTop);

    _computer->setStyleSheet("color:rgb(255,255,255)");
    _computer->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _computer->setText(Lang("SetGay/computer"));

    _computerName->setStyleSheet("color:rgb(255,255,255)");
    _computerName->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    _recordFolder->setStyleSheet("color:rgb(255,255,255)");
    _recordFolder->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _recordFolder->setText(Lang("SetGay/MRDpath"));

    QString path = Global->mirrorRecordPath();
#ifdef Q_OS_WIN
    path.replace("/", "\\");
#endif
#ifdef Q_OS_MACX
    path.replace("\\", "/");
#endif
    _recordEdit->setText(path);

    _recordSelect->setImagePrefixName("browse");
    _recordOpen->setImagePrefixName("file_open");

    connect(_recordEdit,SIGNAL(textChanged(QString)),SLOT(recordEditChange(QString)));
    connect(_recordOpen,SIGNAL(clicked()),SLOT(openRecordPaht()));
    connect(_recordSelect,SIGNAL(clicked()),SLOT(selectRecordPath()));
    _scaleRatio = Global->_screenRatio;
    upUI();
}

void SettingAirlayWidget::upUI()
{
    _airPosMap.clear();
    double height = 20 * _scaleRatio;
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * _scaleRatio);
    font.setBold(true);
    _sirplayService->setFont(font);
    _status->setFont(font);
    _statusInfo->setFont(font);
    _bonjourInfo->setFont(font);
    _computer->setFont(font);
    _computerName->setFont(font);
    _recordFolder->setFont(font);
    _recordEdit->setFont(font);

    font.setPixelSize(14 * _scaleRatio);
    _title->setFont(font);

    double xPos = 4*_scaleRatio;

    int titleW  = _title->fontMetrics().width(_title->text());
    _title->setGeometry(xPos,       5*_scaleRatio,titleW,height);
    int lineW   = this->width()- (_title->x() + _title->width() + 28*_scaleRatio);
    _titleLine->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,15*_scaleRatio,lineW, 1*_scaleRatio);
    _titleLine2->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,16*_scaleRatio,lineW, 1*_scaleRatio);

    _sirplayService->setGeometry(xPos,40*_scaleRatio,210*_scaleRatio,height);
    _airplayOff->updateUI(_scaleRatio);
    _airplayOn->updateUI(_scaleRatio);
    _airplayOff->move(xPos + 218*_scaleRatio,_sirplayService->y());
    _airplayOn->move(xPos + 258*_scaleRatio,_sirplayService->y());
    _airPosMap.insert(Lang("SetMain/airplay") + ":" + LangNoColon("SetGay/AirSev"),40*_scaleRatio);

    _status->setGeometry(xPos,_sirplayService->y() + 35*_scaleRatio,210*_scaleRatio,height);
    _statusInfo->setGeometry(xPos + 218*_scaleRatio,_sirplayService->y() + 35*_scaleRatio,170*_scaleRatio,height);
    _airPosMap.insert(Lang("SetMain/airplay") + ":" + LangNoColon("SetGay/status"),75*_scaleRatio);

    _computer->setGeometry(xPos,_sirplayService->y() + 70*_scaleRatio,210*_scaleRatio,height);
    _computerName->setGeometry(xPos + 218*_scaleRatio,_sirplayService->y() + 70*_scaleRatio,170*_scaleRatio,height);
    _airPosMap.insert(Lang("SetMain/airplay") + ":" + LangNoColon("SetGay/computer"),110*_scaleRatio);

    _recordFolder->setGeometry(xPos,_sirplayService->y() + 115*_scaleRatio,210*_scaleRatio,height);
    _recordEdit->setGeometry(xPos + 218*_scaleRatio,_sirplayService->y() + 115*_scaleRatio,280*_scaleRatio,height);
    _recordEdit->setStyleSheet( Global->lineEditStyleSheet(4 * _scaleRatio));
    _recordSelect->move(_recordEdit->x()+_recordEdit->width()    + 8 * _scaleRatio,_sirplayService->y() + 115*_scaleRatio);
    _recordOpen->move(_recordSelect->x()+_recordSelect->width()  + 4 * _scaleRatio,_sirplayService->y() + 115*_scaleRatio);
    _airPosMap.insert(Lang("SetMain/airplay") + ":" + LangNoColon("SetGay/MRDpath"),145*_scaleRatio);

    double bonjXpos = (this->width() - 512*_scaleRatio)/2;
    _bonjourInfo->setGeometry(bonjXpos, _sirplayService->y() + 180*_scaleRatio,
                                  512*_scaleRatio, 88*_scaleRatio);
}

void SettingAirlayWidget::openRecordPaht()
{
    if(!_recordEdit->text().isEmpty()) {
        QString open = "file:///";
        open.append(QDir::toNativeSeparators(_recordEdit->text()));
        QDesktopServices::openUrl(QUrl(open, QUrl::TolerantMode));
    }
}

void SettingAirlayWidget::selectRecordPath()
{
    QString airPath = _recordEdit->text();
    const QString filePath = OpenFileDialog->getDir(Lang("Folder/record"), airPath);
    if(!filePath.isEmpty())
        _recordEdit->setText(filePath);
}

void SettingAirlayWidget::airOffClick()
{
    PMW->closeAirplay();
    _airplayOn->recover();
    _statusInfo->setText(Lang("SetGay/stop"));
}

void SettingAirlayWidget::airOnClick()
{
    if(PMW->startAirplay()) {
        _airplayOff->recover();
        _statusInfo->setText(Lang("SetGay/run"));
    }
}

void SettingAirlayWidget::recordEditChange(const QString &editText)
{
    Global->setMirrorRecordPath(editText);
}

void SettingAirlayWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
