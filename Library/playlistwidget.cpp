#include "playlistwidget.h"
#include "icosmodewidget.h"
#include "listmusicwidget.h"
#include "playlisthead.h"
#include <QDebug>
#include <QFileInfo>
#include <QtAlgorithms>
#include <QStandardPaths>
#include "librarydata.h"
#include "playermainwidget.h"
#include "configure.h"
#include "openfile.h"
#include "globalarg.h"
#include "listvideowidget.h"
PlayListWidget::PlayListWidget(QWidget *parent) : TransEventWidget(parent)
{
    this->setMouseTracking(true);
    setStyleSheet("background-color: rgb(17, 17, 18)");
    Init();
}

void PlayListWidget::updateLanguage()
{
    _listHead->updateLanguage();
    _listModeWidget->updateLanguage();
    _icoModeWidget->updateLanguage();
    _youbetu->updateLanguage();
}

void PlayListWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _listHead->updateUI(scaleRatio);
    _listModeWidget->updateUI(scaleRatio);
    _icoModeWidget->updateUI(scaleRatio);
    _musicWidget->updateUI(scaleRatio);
    _youbetu->updateUI(scaleRatio);
}

void PlayListWidget::Init()
{
    _mediaWidget = new TransEventWidget(this);
    _youbetu = new YoutubeWidget(this);
    _listHead = new PlayListHead(_mediaWidget);
    _listModeWidget = new ListVideoWidget(_mediaWidget);
    _icoModeWidget  = new IcosModeWidget(_mediaWidget);
    _musicWidget    = new ListMusicWidget(_mediaWidget);
    _youbetu->hide();

    connect(_listHead,SIGNAL(listModemenuclk(QAction*)),SLOT(listModeChange(QAction*)));
    connect(_listHead,SIGNAL(arragemenuclk(QAction*)),SLOT(arragemenuChange(QAction*)));
    connect(_listHead,SIGNAL(addClick()),SLOT(addItem()));
    connect(_listHead,SIGNAL(delClick()),SLOT(delItem()));

    connect(_listModeWidget,SIGNAL(delbtnEnable(bool)),_listHead,SLOT(updelbtn(bool)));
    connect(_musicWidget,SIGNAL(delbtnEnable(bool)),_listHead,SLOT(updelbtn(bool)));
    connect(_icoModeWidget,SIGNAL(delbtnEnable(bool)),_listHead,SLOT(updelbtn(bool)));

    connect(LibData,SIGNAL(upvideoList(bool,bool,QString)),SLOT(upmediaWidget(bool,bool,QString)));
    connect(LibData,SIGNAL(upresize()),SLOT(upresizeWidget()));
    connect(LibData,SIGNAL(clearShowItem()),SLOT(clearAllWidgetItem()));
    _prePath = "";
    _music = false;
    _scaleRatio = 1.0;
    _curListItem = "pMovies";

    showcurwidget(_curListItem);
}

void PlayListWidget::listModeChange(QAction *act)
{
    CGlobal::LibraryMode temp;
    if(act->data() == "icons"){
        temp = CGlobal::LibraryMode::IcoMode;
    } else {
        temp = CGlobal::LibraryMode::ListMode;
    }
    if(Global->getLibraryMode() == temp) return;
    Global->setLibraryMode(temp);
    showcurwidget(_curListItem);
}

void PlayListWidget::arragemenuChange(QAction *act)
{
    if(act->data() == "title") {
        LibData->setMusicSort(LibData->Title);
    } else if(act->data() == "artist") {
        LibData->setMusicSort(LibData->Artist);
    } else {
        LibData->InitMusic();
    }

    upmediaWidget(true,false,"");
}

void PlayListWidget::addItem()
{
    QString openPath;
    if(_prePath == "") {
        if(_curListItem == "pMusic") {
            openPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
        } else {
            openPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        }

    } else {
        openPath = _prePath;
    }

    QStringList fileList;
    if(_curListItem == "pMovies") {
        fileList = OpenFileDialog->getFileNames(Lge->getLangageValue("OpenFile/opvideo"),
                                                openPath,
                                                Lge->getLangageValue("OpenFile/videoFilt") + "(" +
                                                Config->getUserVideoFormat() + ")");
    } else if(_curListItem == "pMusic") {
        fileList = OpenFileDialog->getFileNames(Lge->getLangageValue("OpenFile/opmusic"),
                                                openPath,
                                                Lge->getLangageValue("OpenFile/musicFilt") + "(" +
                                                Config->getUserAudioFormat() + ")");
    } else {
        fileList = OpenFileDialog->getFileNames(Lge->getLangageValue("OpenFile/opmedia"),
                                                openPath,
                                                Lge->getLangageValue("OpenFile/allfile") + ";;" +
                                                Lge->getLangageValue("OpenFile/videoFilt") + "(" +
                                                Config->getUserVideoFormat() + ");;" +
                                                Lge->getLangageValue("OpenFile/musicFilt") + "(" +
                                                Config->getUserAudioFormat() + ");");
    }
    if (fileList.isEmpty())
        return;

    QString file = fileList.at(0);
    QFileInfo fileinfo(file);
    _prePath = fileinfo.path();
    clearAllWidgetItem();

    OpenFileDialog->addMediaDialog(fileList);
}

void PlayListWidget::delItem()
{
    bool isMove = false;

    int res = OpenFileDialog->mediaDelDialog();
    switch (res) {
    case 0:
        return;
        break;
    case 1:
        isMove = true;
        break;
    case 2 :
        isMove = false;
        break;
    }
    clearAllWidgetItem();
    if(_music) {
        _musicWidget->delSelWidget(isMove);
    } else {
        if(Global->getLibraryMode() == CGlobal::LibraryMode::ListMode) {
            _listModeWidget->delSelWidget(isMove);
        } else {
            _icoModeWidget->delSelWidget(isMove);
        }
    }
}

void PlayListWidget::upmediaWidget(bool bUp, bool bconvert, QString)
{
    Q_UNUSED(bconvert);
    //    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    //    QString str = time.toString("yyyy-MM-dd hh:mm:ss:zzz ddd"); //设置显示格式

    QList<MEDIAINFO> showList;
    showList = LibData->getItemShowList();

    if(!bUp || bconvert) {
        if(_showWidget == ListWidget) {
            _listModeWidget->upWidgetData(showList);
        } else if(_showWidget == MusicWidget) {
            _musicWidget->upWidgetData(showList);
        }
        return;
    }

    if(_showWidget == IcoWidget) {
        _icoModeWidget->clearWidget();
        _icoModeWidget->setShowItemName(_curListItem);
        int showNum = 0;
        if(showList.size()>LibData->getMaxCount())
            showNum = LibData->getMaxCount();
        else
            showNum = showList.size();
        for(int i=0;i<showNum;++i) {
            _icoModeWidget->addWidget(showList.at(i));
        }
        _icoModeWidget->updateSubWidget();
    }
    if(_showWidget == ListWidget) {
        _listModeWidget->clearWidget();
        _listModeWidget->setShowItemName(_curListItem);
        _listModeWidget->initData(showList);
    }
    if(_showWidget == MusicWidget) {
        _musicWidget->clearWidget();
        _musicWidget->setShowItemName(_curListItem);
        _musicWidget->initData(showList);
    }

}

void PlayListWidget::upresizeWidget()
{
    if (_showWidget == IcoWidget) {
        int maxCount = LibData->getMaxCount();
        int curCount = _icoModeWidget->getShowCount();
        if(maxCount>curCount) {
            _icoModeWidget->widgetAddVideo(maxCount-curCount);
        }
    }
}

void PlayListWidget::upMaxCount(bool up)
{
    int maxCount = 0;
    if(_showWidget == IcoWidget) {
         maxCount = ((_icoModeWidget->width()+10)/(120*_scaleRatio+10)+1) * ((_icoModeWidget->height()+10)/(120*_scaleRatio+10)+1);
    }
    LibData->setMaxCount(maxCount, up);
}

void PlayListWidget::upWidget()
{
    //LibData->upmediaPlay();
    upMaxCount(false);
    upmediaWidget(true,false,"");
}

void PlayListWidget::hideAllWidget()
{
    _listModeWidget->hide();
    _icoModeWidget->hide();
    _musicWidget->hide();
}

void PlayListWidget::clearAllWidgetItem()
{
    _listModeWidget->setShowItemName("");
    _icoModeWidget->setShowItemName("");
    _musicWidget->setShowItemName("");
}

void PlayListWidget::resizeEvent(QResizeEvent *)
{
    _youbetu->setGeometry(0,0,this->width(),this->height());
    _mediaWidget->setGeometry(0,0,this->width(),this->height());

    _listHead->setGeometry(0,0,this->width(),33*_scaleRatio);
    _listHead->repaint();
    _listModeWidget->setGeometry(0,_listHead->height(),this->width()-4*_scaleRatio,this->height()-_listHead->height()-4*_scaleRatio);
    _musicWidget->setGeometry(0,_listHead->height(),this->width()-4*_scaleRatio,this->height()-_listHead->height()-4*_scaleRatio);
    _icoModeWidget->setGeometry(0,_listHead->height(),this->width()-4*_scaleRatio,this->height()-_listHead->height()-4*_scaleRatio);
    upMaxCount(true);
}

void PlayListWidget::showcurwidget(const QString &itemName)
{
    _icoModeWidget->hide();
    _musicWidget->hide();
    _listModeWidget->hide();

    if((itemName == "FMovies") || (itemName == "FMusic"))
        _listHead->isfavorites(true);
    else
        _listHead->isfavorites(false);
    if((itemName == "FMovies") || (itemName == "FMusic")||(itemName == "yVideos") || (itemName == "yMusic") || (itemName == "pAirRecord"))
        _listHead->upaddbtn(false);
    else
        _listHead->upaddbtn(true);

    if((itemName == "FMusic")||(itemName == "pMusic") || (itemName == "yMusic"))
        _music = true;
    else
        _music = false;

    _listHead->menuSortEnable(_music);
    _listHead->clearSearchText();


    LibData->setselectitem(itemName);

    if(_curListItem == "YouTube") {
        _youbetu->show();
        _youbetu->raise();
        _mediaWidget->hide();
    } else {
        _youbetu->hide();
        _mediaWidget->show();
        hideAllWidget();
        if(LibData->isConstItem(_curListItem)) {
            _listHead->upMenuBtn(true);
            if(_music) {
                _musicWidget->clearSelWidget();
                _musicWidget->show();
                _musicWidget->setFocus();
                _musicWidget->raise();
                _showWidget = MusicWidget;
            } else {
                if(Global->getLibraryMode() == CGlobal::LibraryMode::IcoMode) {
                    _icoModeWidget->show();
                    _icoModeWidget->clearSelWidget();
                    _icoModeWidget->setFocus();
                    _icoModeWidget->raise();
                    _showWidget = IcoWidget;
                } else {
                    _listModeWidget->clearSelWidget();
                    _listModeWidget->show();
                    _listModeWidget->setFocus();
                    _listModeWidget->raise();
                    _showWidget = ListWidget;
                }
            }
        } else {
            _listHead->upMenuBtn(false);
            _listModeWidget->clearSelWidget();
            _listModeWidget->show();
            _listModeWidget->setFocus();
            _listModeWidget->raise();
            _showWidget = ListWidget;
        }
    }
    upWidget();
}

void PlayListWidget::itemClickSlt(const QString &itemName)
{
    if(itemName != _curListItem) {
        _curListItem = itemName;
        showcurwidget(_curListItem);
    }
}
