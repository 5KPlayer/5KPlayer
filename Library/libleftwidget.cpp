#include "libleftwidget.h"
#include <QDebug>
#include <QMouseEvent>
#include "openfile.h"
#include "globalarg.h"
#include "playermainwidget.h"
libLeftWidget::libLeftWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    this->setMouseTracking(true);
    Init();
}

void libLeftWidget::InitConstItem()
{
    _listwidget = new TransEvenListWidget(this);
    _listwidget->scrollToTop();
    _listwidget->setGeometry(this->rect());
    _fFavorites = new ListRoots(_listwidget);
    _fMovies    = new ListNodes(false, _listwidget);
    _fMusic     = new ListNodes(false,_listwidget);
    _yYouTube   = new ListRoots(_listwidget);
    _yVideos    = new ListNodes(false,_listwidget);
    _yMusic     = new ListNodes(false,_listwidget);
    _pPlayLists = new ListRoots(false,_listwidget);
    _pMovies    = new ListNodes(false,_listwidget);
    _pMusic     = new ListNodes(false,_listwidget);
    _pAirRecord = new ListNodes(false,_listwidget);
    _nListNew   = new ListNewNode(_listwidget);

    _yYouTube->setRootText("YouTube","rgb(232,63,63)");
    _fMovies->setNodeData(":/res/svg/movies-light.svg","FMovies",0);
    _fMusic->setNodeData(":/res/svg/music-light.svg","FMusic",0);
    _yVideos->setNodeData(":/res/svg/movies-light.svg","yVideos",0);
    _yMusic->setNodeData(":/res/svg/music-light.svg","yMusic",0);
    _pMovies->setNodeData(":/res/svg/movies-light.svg","pMovies",0);
    _pMusic->setNodeData(":/res/svg/music-light.svg","pMusic",0);
    _pAirRecord->setNodeData(":/res/svg/airRecord-light.svg","pAirRecord",0);

    InitLanguage();
    _listwidget->setStyleSheet("background-color: rgb(14, 14, 15)");
    _listwidget->resize(this->width(),this->height());
    for(int i=0;i<11;++i) {
        _WidgetItem[i] = new QListWidgetItem();
        _WidgetItem[i]->setSizeHint(QSize(_WidgetItem[i]->sizeHint().width(),35*_scaleRatio));
        _WidgetItem[i]->setText(Global->_constItemName[i]);
        _listwidget->addItem(_WidgetItem[i]);
    }
    _listwidget->setItemWidget(_WidgetItem[0],_fFavorites);
    _listwidget->setItemWidget(_WidgetItem[1],_fMovies);
    _listwidget->setItemWidget(_WidgetItem[2],_fMusic);
    _listwidget->setItemWidget(_WidgetItem[3],_yYouTube);
    _listwidget->setItemWidget(_WidgetItem[4],_yVideos);
    _listwidget->setItemWidget(_WidgetItem[5],_yMusic);
    _listwidget->setItemWidget(_WidgetItem[6],_pPlayLists);
    _listwidget->setItemWidget(_WidgetItem[7],_pMovies);
    _listwidget->setItemWidget(_WidgetItem[8],_pMusic);
    _listwidget->setItemWidget(_WidgetItem[9],_pAirRecord);
    _listwidget->setItemWidget(_WidgetItem[10],_nListNew);
    _WidgetItem[0]->setFlags(Qt::NoItemFlags);
    _WidgetItem[6]->setFlags(Qt::NoItemFlags);
    _WidgetItem[10]->setFlags(Qt::NoItemFlags);
    _listwidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //不显示滚动条
    _listwidget->setFrameShape(QListWidget::NoFrame);                //不显示边框
    _CurItem = _WidgetItem[7];

    ListNodes* node = (ListNodes*)_listwidget->itemWidget(_CurItem);
    node->setSelect(true);

    connect(_nListNew,SIGNAL(EditChanged(QString)),this,SLOT(AddWidgetItem(QString)));
    connect(_fFavorites,SIGNAL(clickShow()),this,SLOT(FavoritesClick()));
    connect(_yYouTube,SIGNAL(clickShow()),this,SLOT(YouTubeClick()));
    connect(_listwidget,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(itemClick(QListWidgetItem*)));
    connect(_nListNew,SIGNAL(setmouseig(bool)),_listwidget,SLOT(setmouse_ignore(bool)));
}

void libLeftWidget::InitItem()
{
    QList<QString> itemList = LibData->getitemNames();
    for(int i=0;i<itemList.size();++i) {
        AddWidgetItem(itemList.at(i));
    }
}

void libLeftWidget::updateLanguage()
{
    _nListNew->updateLanguage();
    _fFavorites->updateLanguage();
    _yYouTube->updateLanguage();
    _fMovies->updateLanguage();
    _fMusic->updateLanguage();
    _yVideos->updateLanguage();
    _yMusic->updateLanguage();
    _pMovies->updateLanguage();
    _pMusic->updateLanguage();
    _pAirRecord->updateLanguage();
    _fFavorites->updateLanguage();
    _yYouTube->updateLanguage();
    _pPlayLists->updateLanguage();
    _nListNew->updateLanguage();

    for(int i=0;i<_listwidget->count();++i) {
        _listwidget->item(i)->setSizeHint(QSize(_listwidget->item(i)->sizeHint().width(),35*_scaleRatio));
        if(i>=10 && i<_listwidget->count()-1) {
            ListNodes* node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(i));
            node->updateLanguage();
        }
    }

    _fMovies->setNodeText(Lge->getLangageValue("LibLeft/Movies"));
    _fMusic->setNodeText(Lge->getLangageValue("LibLeft/Music"));
    _yVideos->setNodeText(Lge->getLangageValue("LibLeft/Videos"));
    _yMusic->setNodeText(Lge->getLangageValue("LibLeft/Music"));
    _pMovies->setNodeText(Lge->getLangageValue("LibLeft/Movies"));
    _pMusic->setNodeText(Lge->getLangageValue("LibLeft/Music"));
    _pAirRecord->setNodeText(Lge->getLangageValue("LibLeft/AirRecord"));
    _fFavorites->setRootText(Lge->getLangageValue("LibLeft/Favorites"));
    _pPlayLists->setRootText(Lge->getLangageValue("LibLeft/PlayLists"),"rgb(14,14,15)");
}

void libLeftWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _fMovies->updateUI(scaleRatio);
    _fMusic->updateUI(scaleRatio);
    _yVideos->updateUI(scaleRatio);
    _yMusic->updateUI(scaleRatio);
    _pMovies->updateUI(scaleRatio);
    _pMusic->updateUI(scaleRatio);
    _pAirRecord->updateUI(scaleRatio);
    _fFavorites->updateUI(scaleRatio);
    _yYouTube->updateUI(scaleRatio);
    _pPlayLists->updateUI(scaleRatio);
    _nListNew->updateUI(scaleRatio);


    for(int i=0;i<_listwidget->count();++i) {
        _listwidget->item(i)->setSizeHint(QSize(_listwidget->item(i)->sizeHint().width(),35*_scaleRatio));
        if(i>=10 && i<_listwidget->count()-1) {
            ListNodes* node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(i));
            node->updateUI(scaleRatio);
        }
    }
}

void libLeftWidget::itemClick(QListWidgetItem *it)
{
    if(_listwidget->itemWidget(it)!=_yYouTube) {
        _PreItem = _CurItem;
        _CurItem = it;
        if(_PreItem) {
            ListNodes* norNode = (ListNodes*)_listwidget->itemWidget(_PreItem);
            norNode->setSelect(false);
        }

        ListNodes* node = (ListNodes*)_listwidget->itemWidget(_CurItem);
        node->setSelect(true);
        if(_listwidget->itemWidget(it) == _yVideos \
           || _listwidget->itemWidget(it) == _pMusic \
           || _listwidget->itemWidget(it) == _pAirRecord \
           || _listwidget->itemWidget(it) == _yMusic) {
            node->setNumRed(false);
            UpdataList(false,false,node->getNodeItem());
        }
    } else {
        _PreItem = _CurItem;
        ListNodes* norNode = (ListNodes*)_listwidget->itemWidget(_PreItem);
        if(_listwidget->itemWidget(_PreItem)!= _yYouTube)
            norNode->setSelect(false);
    }
    emit ListClick(it->text());
}

void libLeftWidget::FavoritesClick()
{
    if(_fMovies->isHidden()) {
        _listwidget->setItemHidden(_WidgetItem[1],false);
        _listwidget->setItemHidden(_WidgetItem[2],false);
        _fFavorites->setBtnNameShow(false);
    } else {
        _listwidget->setItemHidden(_WidgetItem[1],true);
        _listwidget->setItemHidden(_WidgetItem[2],true);
        _fFavorites->setBtnNameShow(true);
    }
}

void libLeftWidget::YouTubeClick()
{
    if(_yVideos->isHidden()) {
        _listwidget->setItemHidden(_WidgetItem[4],false);
        _listwidget->setItemHidden(_WidgetItem[5],false);
        _yYouTube->setBtnNameShow(false);
    } else {
        _listwidget->setItemHidden(_WidgetItem[4],true);
        _listwidget->setItemHidden(_WidgetItem[5],true);
        _yYouTube->setBtnNameShow(true);
    }
}

void libLeftWidget::AddWidgetItem(const QString &name)
{
    bool addflag = true;
    if(!_init)
        if(!LibData->addItem(name))
        {
            addflag = false;
        }
    if(addflag) {
        ListNodes* node = new ListNodes(true,this);
        node->setNodeData(":/res/svg/others_light.svg",name,0);
        node->setFontColor("rgb(255,170,51)");
        node->updateUI(_scaleRatio);
        QListWidgetItem* listItem = new QListWidgetItem();
        listItem->setText(name);
        listItem->setSizeHint(QSize(listItem->sizeHint().width(),35*_scaleRatio));
        _listwidget->insertItem(_nItemNum,listItem);
        _listwidget->setItemWidget(listItem,node);

        connect(node,SIGNAL(deleteNode(QString)),SLOT(DelWidgetItem(QString)));
        _nItemNum++;
    } else {
        OpenFileDialog->openTooltipDialog(Lge->getLangageValue("ItemExits/info"));
    }
}

void libLeftWidget::DelWidgetItem(const QString &name)
{
    LibData->deItem(name);
    int count = _listwidget->count();
    for(int i=0;i<count;++i) {
        QListWidgetItem * item = _listwidget->item(i);
        if(item->text() == name) {
            if(item->isSelected()) {
                QListWidgetItem *preitem = _listwidget->item(i-1);
                _CurItem = preitem;
                itemChange(_CurItem->text());
            }
            _listwidget->takeItem(i);
            _listwidget->removeItemWidget(item);
            _nItemNum--;
            delete item;
            break;
        }
    }
}

void libLeftWidget::UpdataList(bool bUp, bool bconvert,QString itemName)
{
    Q_UNUSED(bUp);
    QList<MEDIAINFO> videoList = LibData->getmedaiData();
    QList<MEDIAINFO> musicList = LibData->getmusicData();
    QList<MEDIAINFO> temp;
    temp.clear();
    ListNodes* node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(1)); //video favorites
    int favoritesNum = 0;
    for(int j=0;j<videoList.size();++j) {
        if(videoList.at(j).isFavorites)
            temp.append(videoList.at(j));
    }
    favoritesNum = LibData->movesMediaList(temp).size();
    node->UpdataNum(favoritesNum);

    temp.clear();
    node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(2)); //music favorites
    favoritesNum = 0;
    for(int j=0;j<musicList.size();++j) {
        if(musicList.at(j).isFavorites)
            temp.append(musicList.at(j));
    }
    favoritesNum = LibData->movesMediaList(temp).size();
    node->UpdataNum(favoritesNum);

    temp.clear();
    node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(4)); // yVideos
    temp = LibData->getMediaforItem("yVideos");

    if(checkUpList(node->getNodeItem(),itemName,bconvert)) {
        if(node->getNumRed()) {
            if(bconvert) {
                int count = node->getRedNum();
                if(count < temp.size())
                    node->UpdataNum(count+1);
            }
        } else if(LibData->getselectitem() == "yVideos") {
            node->UpdataNum(temp.size());
        } else {
            if(bconvert) {
                node->setNumRed(true);
                node->UpdataNum(1);
            } else {
                node->UpdataNum(temp.size());
            }
        }
    }

    node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(5)); // yMusic
    temp =  LibData->getMediaforItem("yMusic");
    if(checkUpList(node->getNodeItem(),itemName,bconvert)) {
        if(node->getNumRed()) {
            if(bconvert) {
                int count = node->getRedNum();
                if(count < temp.size())
                    node->UpdataNum(count+1);
            }
        } else if(LibData->getselectitem() == "yMusic") {
            node->UpdataNum(temp.size());
        } else {
            if(bconvert) {
                node->setNumRed(true);
                node->UpdataNum(1);
            } else {
                node->UpdataNum(temp.size());
            }
        }
    }

    node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(7)); // pMovies
    temp = LibData->getMediaforItem("pMovies");
    node->UpdataNum(temp.size());

    node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(8)); // pMusic
    temp =  LibData->getMediaforItem("pMusic");
    if(checkUpList(node->getNodeItem(),itemName,bconvert)) {
        if(node->getNumRed()) {
            if(bconvert) {
                int count = node->getRedNum();
                if(count < temp.size())
                    node->UpdataNum(count+1);
            }
        } else if(LibData->getselectitem() == "pMusic") {
            node->UpdataNum(temp.size());
        } else {
            if(bconvert) {
                node->setNumRed(true);
                node->UpdataNum(1);
            } else {
                node->UpdataNum(temp.size());
            }
        }
    }

    node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(9)); // pAirRecord
    temp =  LibData->getMediaforItem("pAirRecord");
    if(checkUpList(node->getNodeItem(),itemName,bconvert)) {
        if(node->getNumRed()) {
            if(bconvert) {
                int count = node->getRedNum();
                node->UpdataNum(count+1);
            }
        } else if(LibData->getselectitem() == "pAirRecord") {
            node->UpdataNum(temp.size());
        } else {
            if(bconvert && node->getRedNum() != temp.size()) {
                node->setNumRed(true);
                node->UpdataNum(1);
            } else {
                node->UpdataNum(temp.size());
            }
        }
    }

    for(int i=10;i<_nItemNum;++i) {
       ListNodes* node = (ListNodes*)_listwidget->itemWidget(_listwidget->item(i));
       int count = 0;
       for(int j=0;j<videoList.size();++j) {
           if(videoList.at(j).itemname == _listwidget->item(i)->text()) {
               count++;
           }
       }
       for(int j=0;j<musicList.size();++j) {
           if(musicList.at(j).itemname == _listwidget->item(i)->text()) {
               count++;
           }
       }
       node->UpdataNum(count);
    }
}

void libLeftWidget::itemChange(const QString &itemName)
{
    for(int i=0;i<_listwidget->count();++i) {
        QListWidgetItem* item = _listwidget->item(i);
        if(item->text() == itemName) {
            itemClick(item);
            break;
        }
    }
}

void libLeftWidget::Init()
{
    _init = true;
    _nItemNum = 10;
    _PreItem = NULL;
    _scaleRatio = 1.0;
    _curYtbNum = LibData->getMediaforItem("yVideos").size();
    InitConstItem();
    InitItem();
    _init = false;
    connect(LibData,SIGNAL(upvideoList(bool,bool,QString)),SLOT(UpdataList(bool,bool,QString)));
    UpdataList(false,false,"pMovies");
}

void libLeftWidget::InitLanguage()
{
    _fMovies->setNodeText(Lge->getLangageValue("LibLeft/Movies"));
    _fMusic->setNodeText(Lge->getLangageValue("LibLeft/Music"));
    _yVideos->setNodeText(Lge->getLangageValue("LibLeft/Videos"));
    _yMusic->setNodeText(Lge->getLangageValue("LibLeft/Music"));
    _pMovies->setNodeText(Lge->getLangageValue("LibLeft/Movies"));
    _pMusic->setNodeText(Lge->getLangageValue("LibLeft/Music"));
    _pAirRecord->setNodeText(Lge->getLangageValue("LibLeft/AirRecord"));

    _fFavorites->setRootText(Lge->getLangageValue("LibLeft/Favorites"));
    _pPlayLists->setRootText(Lge->getLangageValue("LibLeft/PlayLists"),"rgb(14,14,15)");
}

bool libLeftWidget::checkUpList(QString nodeName, QString itemName, bool bconvert)
{
    bool up = true;
    if(bconvert) {
        if(nodeName == itemName) {
            up = true;
        } else {
            up = false;
        }
    }
    return up;
}

void libLeftWidget::resizeEvent(QResizeEvent *)
{
    _listwidget->resize(this->width(),this->height());
    for(int i=0;i<_listwidget->count() ;++i) {
        _listwidget->itemWidget(_listwidget->item(i))->setFixedHeight(35*_scaleRatio);
    }
}

