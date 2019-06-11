#include "settingwidget.h"
#include "svgtextbutton.h"

#include "settinggeneralwidget.h"
#include "settingdownloadwidget.h"
#include "settingsubtitlewidget.h"
#include "settingairlaywidget.h"
#include "settingdlnawidget.h"
#include "hardwarewidget.h"
#include "transevenlistwidget.h"
#include "SetLeftNode.h"
#include "globalarg.h"
#include "svgbutton.h"
#include "searchingtool.h"


#include <QScrollBar>
#include <QListWidgetItem>
#include <QWheelEvent>
#include <QCompleter>
#include <QLabel>
#include <QDebug>
#include <QSvgWidget>
#include <QLineEdit>

#define W 104
#define H 64

SettingWidget::SettingWidget(QString initNodes, QWidget *parent) : UpdateInterfaceWidget(parent)
{
    this->setStyleSheet("background: rgb(14, 14, 15)");
    _headW = new QWidget(this);
    _headW->setStyleSheet("background-color: rgb(27,27,29)");
    _headTitle = new QLabel(Lge->getLangageValue("SetMain/title"),_headW);
    _headTitle->setStyleSheet("color:rgb(255,255,255)");
    _headTitle->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    _searchwidget = new QSvgWidget(_headW);
    _searchbtn    = new SvgButton(_searchwidget);
    _searchdelbtn = new SvgButton(_searchwidget);
    _searchEdit   = new QLineEdit(_searchwidget);
    _searTool     = new SearchingTool(_searchwidget);


    _searchwidget->setAutoFillBackground(true);
    _searchwidget->load(QString(":/res/svg/search_box.svg"));

    _searchbtn->initSize(13,14);
    _searchbtn->setImagePrefixName("Search");
    _searchbtn->setStyleSheet("border-image:url("")");

    _searchdelbtn->initSize(13,14);
    _searchdelbtn->setImagePrefixName("seach_clos");
    _searchdelbtn->setStyleSheet("border-image:url("")");

    _searchEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _searchEdit->setAlignment(Qt::AlignVCenter);
    _searchEdit->setStyleSheet("border-image:url("");Color:rgb(107,107,107)");

    _searchEdit->setFrame(false);
    _searchdelbtn->setVisible(false);

    _psetWidget     = new QWidget(this);
    _pScrollBar     = new QScrollBar(Qt::Vertical,this);

    _psetWidget->setStyleSheet("background:rgb(32,32,33)");

    _listwidget     = new TransEvenListWidget(this);
    _listwidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //不显示滚动条
    _listwidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //不显示滚动条
    _listwidget->setFrameShape(QListWidget::NoFrame);                //不显示边框
    _listwidget->setStyleSheet("background: rgb(14, 14, 15)");

    {
        _setGeneral     = new SetLeftNode(_listwidget);
        _setDownload    = new SetLeftNode(_listwidget);
        _setSubtitle    = new SetLeftNode(_listwidget);
        _setAirplay     = new SetLeftNode(_listwidget);
        _setHardware    = new SetLeftNode(_listwidget);
        _setDLnaware    = new SetLeftNode(_listwidget);

        _setDownload->setNodeData("Set_video_download",Lge->getLangageValue("SetMain/down"));
        _setGeneral->setNodeData("Set_general",Lge->getLangageValue("SetMain/gen"));
        _setSubtitle->setNodeData("Set_Subtitle",Lge->getLangageValue("SetMain/sub"));
        _setAirplay->setNodeData("Set_airplay",Lge->getLangageValue("SetMain/airplay"));
        _setHardware->setNodeData("Set_Headware",Lge->getLangageValue("SetMain/hard"));
        _setDLnaware->setNodeData("Set_dlna",Lge->getLangageValue("SetMain/dlna"));

        for(int i=0;i<6;++i) {
            _WidgetItem[i] = new QListWidgetItem();
            _WidgetItem[i]->setSizeHint(QSize(136*Global->_screenRatio ,35*Global->_screenRatio));
            _listwidget->addItem(_WidgetItem[i]);
        }
        _listwidget->setItemWidget(_WidgetItem[0],_setGeneral);
        _listwidget->setItemWidget(_WidgetItem[1],_setDownload);
        _listwidget->setItemWidget(_WidgetItem[2],_setSubtitle);
        _listwidget->setItemWidget(_WidgetItem[3],_setHardware);
        _listwidget->setItemWidget(_WidgetItem[4],_setDLnaware);
        _listwidget->setItemWidget(_WidgetItem[5],_setAirplay);

    }
    _generalW   = new SettingGeneralWidget(_psetWidget);
    _downloadW  = new SettingDownloadWidget(_psetWidget);
    _subtitleW  = new SettingSubtitleWidget(_psetWidget);
    _airPlayW   = new SettingAirlayWidget(_psetWidget);
    _dlnaW      = new SettingDLnaWidget(_psetWidget);
    _hardwareW  = new HardwareWidget(_psetWidget);

    _headW->raise();
    connect(_hardwareW,SIGNAL(upCheckState(bool)),SIGNAL(upCloseBtnEnable(bool)));
    connect(_searTool,SIGNAL(doClicked(QString)),SLOT(doClickedSlt(QString)));
    connect(_searchEdit,SIGNAL(textEdited(QString)),SLOT(searEditChange(QString)));
    connect(_searchdelbtn,SIGNAL(clicked()),SLOT(searDelClick()));
    connect(_searchbtn,SIGNAL(clicked()),SLOT(searBtnClicl()));
    connect(_listwidget,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(itemClick(QListWidgetItem*)));
    connect(_pScrollBar,SIGNAL(valueChanged(int)),SLOT(scrollValChange(int)));
    _screenRatio = Global->_screenRatio;
    updateUI(_screenRatio);

    _initNodes = initNodes;
}

void SettingWidget::scrollValChange(int val)
{
    _setGeneral->setSelect(false);
    _setDownload->setSelect(false);
    _setSubtitle->setSelect(false);
    _setAirplay->setSelect(false);
    _setHardware->setSelect(false);
    _setDLnaware->setSelect(false);
    if(val < _setPosMap.value("Downloader") - 15*_screenRatio) {
        _setGeneral->setSelect(true);
    } else if(val >= (_setPosMap.value("Downloader") - 15*_screenRatio) && val < _setPosMap.value("Subtitle") - 15*_screenRatio) {
        _setDownload->setSelect(true);
    } else if(val >= (_setPosMap.value("Subtitle") - 15*_screenRatio) && val < _setPosMap.value("Headware") - 15*_screenRatio) {
        _setSubtitle->setSelect(true);
    } else if(val >= (_setPosMap.value("Headware") - 15*_screenRatio) && val < _setPosMap.value("DLNA") - 15*_screenRatio) {
        _setHardware->setSelect(true);
    } else if(val >= (_setPosMap.value("DLNA") - 15*_screenRatio) && val < _setPosMap.value("AirPlay") - 15*_screenRatio) {
        _setDLnaware->setSelect(true);
    } else {
        _setAirplay->setSelect(true);
    }
    _psetWidget->move(_psetWidget->x(),(_headW->height()-val));
}

void SettingWidget::itemClick(QListWidgetItem *it)
{
    _setDownload->setSelect(false);
    _setSubtitle->setSelect(false);
    _setGeneral->setSelect(false);
    _setAirplay->setSelect(false);
    _setHardware->setSelect(false);
    _setDLnaware->setSelect(false);
    if(_listwidget->itemWidget(it) == _setGeneral) {
        _setGeneral->setSelect(true);
        _pScrollBar->setValue(0);
    } else if(_listwidget->itemWidget(it) == _setDownload) {
        _setDownload->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("Downloader"));
    } else if(_listwidget->itemWidget(it) == _setSubtitle) {
        _setSubtitle->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("Subtitle"));
    } else if(_listwidget->itemWidget(it) == _setAirplay) {
        _setAirplay->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("AirPlay"));
    } else if(_listwidget->itemWidget(it) == _setHardware) {
        _setHardware->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("Headware"));
    } else if(_listwidget->itemWidget(it) == _setDLnaware) {
        _setDLnaware->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("DLNA"));
    }
}

void SettingWidget::searEditChange(QString text)
{
    if(text.isEmpty()) {
        _searchdelbtn->setVisible(false);
        _searTool->setWidget(_searchEdit, _allKeys);
        return;
    } else {
        _searchdelbtn->setVisible(true);
    }

    QStringList matched;
    foreach (QString val, _allKeys) {
        if(val.contains(text, Qt::CaseInsensitive))
        {
            matched.append(val);
        }
    }
    _searTool->showUp(_searchEdit, matched);
}

void SettingWidget::doClickedSlt(const QString &str)
{
    _searchEdit->disconnect();
    _searchEdit->setText(str);
    if(str.isEmpty()) {
        _searchdelbtn->setVisible(false);
    } else {
        _searchdelbtn->setVisible(true);
    }

    int pos = getPosForKey(str);
    if(pos != 0)
        _pScrollBar->setValue(pos);

    connect(_searchEdit,SIGNAL(textEdited(QString)),SLOT(searEditChange(QString)));
}

void SettingWidget::searDelClick()
{
    _searchEdit->setText("");
    _searTool->setWidget(_searchEdit, _allKeys);
    _searchdelbtn->setVisible(false);
}

void SettingWidget::searBtnClicl()
{
    if(_searchEdit->text().isEmpty()) {
        return;
    }
    int pos = getPosForKey(_searchEdit->text());
    if(pos != 0)
        _pScrollBar->setValue(pos);

}

int SettingWidget::getPosForKey(QString key)
{
    QStringList edits = key.split(":");
    int pos = 0;

    if(edits.count() > 1) {
        if(edits.at(0) == Lge->getLangageValue("SetMain/gen")) {
            pos = _generalW->getGenPosForKey(key);
        } else if(edits.at(0) == Lge->getLangageValue("SetMain/down")) {
            pos = _setPosMap.value("Downloader");
            pos += _downloadW->getDownPosForKey(key);
        } else if(edits.at(0) == Lge->getLangageValue("SetMain/sub")) {
            pos = _setPosMap.value("Subtitle");
            pos +=_subtitleW->getSubPosForKey(key);
        } else if(edits.at(0) == Lge->getLangageValue("SetMain/airplay")) {
            pos = _setPosMap.value("AirPlay");
            pos += _airPlayW->getAirPosForKey(key);
        } else if(edits.at(0) == Lge->getLangageValue("SetMain/hard")) {
            pos = _setPosMap.value("Headware");
        } else if(edits.at(0) == Lge->getLangageValue("SetMain/dlna")) {
            pos = _setPosMap.value("DLNA");
            pos += _dlnaW->getDlnaPosForKey(key);
        }
    }
    return pos;
}

void SettingWidget::initSetPos(double scaleRatio)
{
    _allKeys.clear();
    _allKeys.append(_generalW->getGenKeys());
    _allKeys.append(_downloadW->getDownKeys());
    _allKeys.append(_subtitleW->getSubKeys());
    _allKeys.append(_dlnaW->getDlnaKeys());
    _allKeys.append(_airPlayW->getAirKeys());

    _searTool->setWidget(_searchEdit,_allKeys);

    _setPosMap.clear();
    _setPosMap.insert("General",0);
    _setPosMap.insert("Downloader",310*scaleRatio);
    _setPosMap.insert("Subtitle",510*scaleRatio);
    _setPosMap.insert("Headware",780*scaleRatio);
    _setPosMap.insert("DLNA",1090*scaleRatio);
    _setPosMap.insert("AirPlay",1430*scaleRatio);

}

void SettingWidget::upUI()
{
    const int w = this->width();
    const int h = this->height();

    _headW->setGeometry(0, 0, w, 30 * _screenRatio);
    _headTitle->setGeometry(10*_screenRatio,_headW->height()-24*_screenRatio,
                            100*_screenRatio,20*_screenRatio);

    _searchwidget->setGeometry(w-280*_screenRatio,_headW->height()-24*_screenRatio,260*_screenRatio,20*_screenRatio);
    _searchEdit->setGeometry(_searchbtn->width() + 10*_screenRatio,2*_screenRatio,
                             _searchwidget->width()-44*_screenRatio,
                             _searchwidget->height()-4*_screenRatio);

    _searchdelbtn->move(_searchwidget->width()-_searchdelbtn->width()-10*_screenRatio,(_searchwidget->height()-_searchdelbtn->height())/2);
    _searchbtn->move(10*_screenRatio,(_searchwidget->height()-_searchbtn->height())/2);


    int setH = this->height() - _headW->height() + (310+200+310+260+340)*_screenRatio;
    int setW = w-(156-16)*_screenRatio;
    _pScrollBar->setRange(0,(310+200+310+340+260)*_screenRatio);

    _listwidget->setGeometry(0,_headW->height(),156*_screenRatio,h - _headW->height());
    _psetWidget->setGeometry(0 + _listwidget->width(),_headW->height(),setW,setH);
    _pScrollBar->setGeometry(w - 8*_screenRatio,_headW->height(),4*_screenRatio,h - _headW->height());

    _generalW->setGeometry(54*_screenRatio,  0,     620*_screenRatio, 310*_screenRatio);
    _downloadW->setGeometry(54*_screenRatio, 310*_screenRatio,   620*_screenRatio, 200*_screenRatio);
    _subtitleW->setGeometry(54*_screenRatio, 510*_screenRatio,   620*_screenRatio, 270*_screenRatio);
    _hardwareW->setGeometry(54*_screenRatio, 780*_screenRatio,   620*_screenRatio, 310*_screenRatio);
    _dlnaW->setGeometry(54*_screenRatio,     1090*_screenRatio,  620*_screenRatio, 340*_screenRatio);
    _airPlayW->setGeometry(54*_screenRatio,  1430*_screenRatio,   620*_screenRatio, 260*_screenRatio);



    if(_initNodes == "Downloader") {
        _setDownload->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("Downloader"));
    } else if(_initNodes == "Subtitle") {
        _setSubtitle->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("Subtitle"));
    } else if(_initNodes == "AirPlay") {
        _setAirplay->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("AirPlay"));
    } else if(_initNodes == "Headware") {
        _setHardware->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("Headware"));
    } else if(_initNodes == "DLNA") {
        _setDLnaware->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("DLNA"));
    } else {
        _setGeneral->setSelect(true);
        _pScrollBar->setValue(_setPosMap.value("General"));
    }
    _psetWidget->move(_psetWidget->x(),(_headW->height()-_pScrollBar->value()));
}

void SettingWidget::updateUI(const qreal &scaleRatio)
{
    _screenRatio = scaleRatio;
    for(int i=0;i<6;++i) {
        _WidgetItem[i]->setSizeHint(QSize(136*_screenRatio ,35*_screenRatio));
    }
    _generalW->updateUI(scaleRatio);
    _downloadW->updateUI(scaleRatio);
    _subtitleW->updateUI(scaleRatio);
    _airPlayW->updateUI(scaleRatio);
    _dlnaW->updateUI(scaleRatio);
    _hardwareW->updateUI(scaleRatio);
    _setGeneral->updateUI(scaleRatio);
    _setDownload->updateUI(scaleRatio);
    _setSubtitle->updateUI(scaleRatio);
    _setAirplay->updateUI(scaleRatio);
    _setHardware->updateUI(scaleRatio);
    _setDLnaware->updateUI(scaleRatio);
    _searTool->updateUI(scaleRatio);
    _searchbtn->updateUI(scaleRatio);
    initSetPos(scaleRatio);

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);
    _searchEdit->setFont(font);

    font.setBold(true);
    font.setPixelSize(16 * scaleRatio);
    _headTitle->setFont(font);
    _pScrollBar->setStyleSheet(QString(Global->scrollBarSty)
                       .arg(4  * scaleRatio).arg(20  * scaleRatio));
    upUI();
}

void SettingWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}

void SettingWidget::wheelEvent(QWheelEvent *e)
{
    const int deltaY = e->angleDelta().y() / 8;

    int value = _pScrollBar->value() - deltaY;

    if(value < 0)
        value = 0;
    else if(value > _pScrollBar->maximum())
        value = _pScrollBar->maximum();

    _pScrollBar->setValue(value);
}
