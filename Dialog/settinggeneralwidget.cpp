#include "settinggeneralwidget.h"
#include "globalarg.h"
#include "playermainwidget.h"
#include "svgbutton.h"
#include "combobox.h"
#include "openfile.h"

#include <QLineEdit>
#include <QDebug>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

SettingGeneralWidget::SettingGeneralWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    Init();
}

void SettingGeneralWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _snapshotSelect->updateUI(_scaleRatio);
    _snapshotOpen->updateUI(_scaleRatio);
    _convertSelect->updateUI(_scaleRatio);
    _convertOpen->updateUI(_scaleRatio);
    upUI();
}

int SettingGeneralWidget::getGenPosForKey(QString str)
{
    return _genPosMap.value(str);
}

QStringList SettingGeneralWidget::getGenKeys()
{
    return _genPosMap.keys();
}

void SettingGeneralWidget::Init()
{
    _scaleRatio     = 1.0;
    _stayOnTop      = new TransEventLabel(this);
    _styOnTopBox    = new ComboBox(this);
    _closePlay      = new TransEventLabel(this);
    _closePlayBox   = new ComboBox(this);
    _openatLogin    = new TransEventLabel(this);
    _openatLoginBox = new ComboBox(this);
    _checkUpdates   = new TransEventLabel(this);
    _checkUpdatesBox= new ComboBox(this);
    _snapshotPath   = new TransEventLabel(this);
    _snapshotEdit   = new QLineEdit(this);
    _convertPath    = new TransEventLabel(this);
    _title          = new TransEventLabel(this);
    _titleLine      = new TransEventLabel(this);
    _titleLine2      = new TransEventLabel(this);

    _convertEdit    = new QLineEdit(this);
    _snapshotSelect = new SvgButton(this);
    _snapshotOpen   = new SvgButton(this);
    _convertSelect  = new SvgButton(this);
    _convertOpen    = new SvgButton(this);

    _snapshotEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _convertEdit->setContextMenuPolicy(Qt::NoContextMenu);
    QString sty = Lge->getLangageValue("SetGen/stayList");
    //<< "Always" << "While Playing" << "Never";
    _styOnTopBox->addItems(sty.split("|"));

    _title->setStyleSheet("color:rgb(255,255,255)");
    _title->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _title->setText(Lge->getLangageValue("SetMain/gen"));
    _titleLine->setStyleSheet("background-color: rgb(20, 20, 21);");
    _titleLine2->setStyleSheet("background-color: rgb(71, 71, 75);");

    sty = Lge->getLangageValue("SetGen/closeList");
    _closePlayBox->addItems(sty.split("|"));

    sty = Lge->getLangageValue("SetGen/openList");
    _openatLoginBox->addItems(sty.split("|"));

    sty = Lge->getLangageValue("SetGen/ckUpList");
    _checkUpdatesBox->addItems(sty.split("|"));


    _stayOnTop->setStyleSheet("color:rgb(255,255,255)");
    _stayOnTop->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _stayOnTop->setText(Lge->getLangageValue("SetGen/stay"));

    _closePlay->setStyleSheet("color:rgb(255,255,255)");
    _closePlay->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _closePlay->setText(Lge->getLangageValue("SetGen/close"));

    _openatLogin->setStyleSheet("color:rgb(255,255,255)");
    _openatLogin->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _openatLogin->setText(Lge->getLangageValue("SetGen/open"));

    _checkUpdates->setStyleSheet("color:rgb(255,255,255)");
    _checkUpdates->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _checkUpdates->setText(Lge->getLangageValue("SetGen/ckUp"));

    _snapshotPath->setStyleSheet("color:rgb(255,255,255)");
    _snapshotPath->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _snapshotPath->setText(Lge->getLangageValue("SetGen/snap"));

    _convertPath->setStyleSheet("color:rgb(255,255,255)");
    _convertPath->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _convertPath->setText(Lge->getLangageValue("SetGen/conrt"));

    _snapshotSelect->setImagePrefixName("browse");
    _snapshotOpen->setImagePrefixName("file_open");
    _convertSelect->setImagePrefixName("browse");
    _convertOpen->setImagePrefixName("file_open");

    _snapshotSelect->initSize(20,20);
    _snapshotOpen->initSize(20,20);
    _convertSelect->initSize(20,20);
    _convertOpen->initSize(20,20);

    _styOnTopBox->setCurrentIndex(Global->stayOnTop());
    _closePlayBox->setCurrentIndex(Global->closeType());
    //_openatLoginBox->setCurrentIndex(Global->openLogin());
    _openatLoginBox->setCurrentIndex(!Global->isOpenBootUp());
    _checkUpdatesBox->setCurrentIndex(Global->checkUpdateMode());

    QString path = Global->snapShotPath();
    path.replace("/", "\\");
    _snapshotEdit->setText(path);

    QString path2 = Global->convertPath();
    path2.replace("/", "\\");
    _convertEdit->setText(path2);

    _snapshotEdit->setCursorPosition(0);
    _convertEdit->setCursorPosition(0);
    connect(_styOnTopBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [=](int index){
        //Global->setStayOnTop(CGlobal::General_Stay(index));
        if(index == 0) {
            PMW->stayAlwaysOnTop();
        } else if(index == 1){
            PMW->stayPlayingOnTop();
        } else if(index == 2) {
            PMW->stayNeverOnTop();
        }
    });
    connect(_closePlayBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [=](int index){
        Global->setCloseType(CGlobal::General_Close(index));
    });
    connect(_openatLoginBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [=](int index){
        if(index == 0)
            Global->openBootUp();
        else if(index == 1)
            Global->closeBootUp();
        //Global->setOpenLogin(CGlobal::General_Login(index));
    });
    connect(_checkUpdatesBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [=](int index){
        Global->setCheckUpdateMode(CGlobal::General_Update(index));
    });

    connect(_snapshotSelect,SIGNAL(clicked()),SLOT(selectSnapshotPath()));
    connect(_snapshotOpen,  SIGNAL(clicked()),SLOT(openSnapshotPath()));
    connect(_snapshotEdit,  SIGNAL(textChanged(QString)),SLOT(snapPathChange(QString)));

    connect(_convertOpen,   SIGNAL(clicked()),SLOT(openConvertPath()));
    connect(_convertSelect, SIGNAL(clicked()),SLOT(selectConvertPath()));
    connect(_convertEdit,   SIGNAL(textChanged(QString)),SLOT(convertPathChange(QString)));

    upUI();
}

void SettingGeneralWidget::upUI()
{
    _genPosMap.clear();
    double height = 20*_scaleRatio;
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * _scaleRatio);
    font.setBold(true);
    _stayOnTop->setFont(font);
    _styOnTopBox->setFont(font);

    _closePlay->setFont(font);
    _closePlayBox->setFont(font);   

    _openatLogin->setFont(font);
    _openatLoginBox->setFont(font);

    _checkUpdates->setFont(font);
    _checkUpdatesBox->setFont(font);

    _snapshotPath->setFont(font);
    _snapshotEdit->setFont(font);

    _convertPath->setFont(font);
    _convertEdit->setFont(font);

    font.setPixelSize(14 * _scaleRatio);
    _title->setFont(font);

    double xPos = 4*_scaleRatio;

    int titleW  = _title->fontMetrics().width(_title->text());
    _title->setGeometry(xPos,       5*_scaleRatio,titleW,height);
    int lineW   = this->width()- (_title->x() + _title->width() + 28*_scaleRatio);
    _titleLine->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,15*_scaleRatio,lineW, 1*_scaleRatio);
    _titleLine2->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,16*_scaleRatio,lineW, 1*_scaleRatio);

    _stayOnTop->setGeometry(xPos,   40*_scaleRatio,210*_scaleRatio,height);
    _styOnTopBox->setGeometry(xPos + _stayOnTop->width()+8*_scaleRatio,40*_scaleRatio,220*_scaleRatio,height);
    _styOnTopBox->upStyleSheet(_scaleRatio);
    _genPosMap.insert(Lge->getLangageValue("SetMain/gen") + ":" + LangNoColon("SetGen/stay"),_stayOnTop->y());

    _closePlay->setGeometry(xPos,   75*_scaleRatio,210*_scaleRatio,height);
    _closePlayBox->setGeometry(_styOnTopBox->x(),       75*_scaleRatio,220*_scaleRatio,height);
    _closePlayBox->upStyleSheet(_scaleRatio);
    _genPosMap.insert(Lge->getLangageValue("SetMain/gen") + ":" + LangNoColon("SetGen/close"),75*_scaleRatio);

    _openatLogin->setGeometry(xPos, 110*_scaleRatio,210*_scaleRatio,height);
    _openatLoginBox->setGeometry(_styOnTopBox->x(),     110*_scaleRatio,220*_scaleRatio,height);
    _openatLoginBox->upStyleSheet(_scaleRatio);
    _genPosMap.insert(Lge->getLangageValue("SetMain/gen") + ":" + LangNoColon("SetGen/open"),110*_scaleRatio);

    _checkUpdates->setGeometry(xPos,145*_scaleRatio,210*_scaleRatio,height);
    _checkUpdatesBox->setGeometry(_styOnTopBox->x(),    145*_scaleRatio,220*_scaleRatio,height);
    _checkUpdatesBox->upStyleSheet(_scaleRatio);
    _genPosMap.insert(Lge->getLangageValue("SetMain/gen") + ":" + LangNoColon("SetGen/ckUp"),145*_scaleRatio);


    _snapshotPath->setGeometry(xPos,                    190*_scaleRatio,210*_scaleRatio,height);
    _snapshotEdit->setGeometry(xPos+218*_scaleRatio,    190*_scaleRatio,280*_scaleRatio,height);
    _snapshotEdit->setStyleSheet( Global->lineEditStyleSheet(4 * _scaleRatio));
    _genPosMap.insert(Lge->getLangageValue("SetMain/gen") + ":" + LangNoColon("SetGen/snap"),190*_scaleRatio);

    _snapshotSelect->move(xPos+506*_scaleRatio,  190*_scaleRatio);
    _snapshotOpen->move(xPos+530*_scaleRatio,    190*_scaleRatio);

    _convertPath->setGeometry(xPos,                     225*_scaleRatio,210*_scaleRatio,height);
    _convertEdit->setGeometry(xPos+218*_scaleRatio,     225*_scaleRatio,280*_scaleRatio,height);
    _convertEdit->setStyleSheet( Global->lineEditStyleSheet(4 * _scaleRatio));
    _genPosMap.insert(Lge->getLangageValue("SetMain/gen") + ":"
                      + Global->removeSetColon(Lge->getLangageValue("SetGen/conrt")),190*_scaleRatio);

    _convertSelect->move(xPos+506*_scaleRatio,   225*_scaleRatio);
    _convertOpen->move(xPos+530*_scaleRatio,     225*_scaleRatio);
}

void SettingGeneralWidget::openSnapshotPath()
{
    if(!_snapshotEdit->text().isEmpty()) {
        QString open = "file:///";
        open.append(QDir::toNativeSeparators(_snapshotEdit->text()));
        QDesktopServices::openUrl(QUrl(open, QUrl::TolerantMode));
    }
}

void SettingGeneralWidget::selectSnapshotPath()
{
    QString snapPath = _snapshotEdit->text();
    const QString filePath = OpenFileDialog->getDir(Lge->getLangageValue("Folder/snapshot"), snapPath);
    if(!filePath.isEmpty())
        _snapshotEdit->setText(filePath);
}

void SettingGeneralWidget::openConvertPath()
{
    if(!_convertEdit->text().isEmpty()) {
        QString open = "file:///";
        open.append(QDir::toNativeSeparators(_convertEdit->text()));
        QDesktopServices::openUrl(QUrl(open, QUrl::TolerantMode));
    }
}

void SettingGeneralWidget::selectConvertPath()
{
    QString convertPath = _convertEdit->text();
    const QString filePath = OpenFileDialog->getDir(Lge->getLangageValue("Folder/convert"), convertPath);
    if(!filePath.isEmpty())
        _convertEdit->setText(filePath);
}

void SettingGeneralWidget::snapPathChange(const QString &snapPath)
{
    Global->setSnapShotPath(snapPath);
    _snapshotEdit->setCursorPosition(0);
}

void SettingGeneralWidget::convertPathChange(const QString &convertPath)
{
    Global->setConvertPath(convertPath);
    _convertEdit->setCursorPosition(0);
}

void SettingGeneralWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
