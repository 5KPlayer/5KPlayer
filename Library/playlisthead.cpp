#include "playlisthead.h"
#include "librarydata.h"
#include "globalarg.h"
#include <QDebug>
#include <QMouseEvent>
#define HEADHEIGHT  33
PlayListHead::PlayListHead(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    Init();
}

void PlayListHead::updateLanguage()
{
    InitLanguage();
}

void PlayListHead::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _addbtn->updateUI(_scaleRatio);
    _delbtn->updateUI(_scaleRatio);
    //_playMode->updateUI(_scaleRatio);
    _menu->updateUI(_scaleRatio);
    _searchbtn->updateUI(_scaleRatio);
    _searchdelbtn->updateUI(_scaleRatio);
    upUI();
}

void PlayListHead::isfavorites(bool state)
{
    _isFavorites = state;
    updelbtn(true);
}

void PlayListHead::Init()
{
    _scaleRatio = 1.0;
    _isFavorites = false;
    _addbtn     = new SvgButton(this);
    _delbtn     = new SvgButton(this);
    _menu       = new SvgButton(this);

    //_playMode   = new SvgButton(this);

    _line       = new TransEventLabel(this);

    _searchwidget = new QSvgWidget(this);
    _searchbtn    = new SvgButton(_searchwidget);
    _searchdelbtn = new SvgButton(_searchwidget);
    _searchEdit   = new QLineEdit(_searchwidget);

    _searchwidget->setAutoFillBackground(true);
    _searchwidget->load(QString(":/res/svg/search_box.svg"));

    _searchEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _searchEdit->setAlignment(Qt::AlignVCenter);
    //_playListMode = Global->getPlayMode();


    _searchbtn->initSize(13*_scaleRatio,14*_scaleRatio);
    _searchdelbtn->initSize(13*_scaleRatio,14*_scaleRatio);
    _addbtn->initSize(33*_scaleRatio,33*_scaleRatio);
    _delbtn->initSize(33*_scaleRatio,33*_scaleRatio);
    //_playMode->initSize(33*_scaleRatio,33*_scaleRatio);
    _menu->initSize(14*_scaleRatio,10*_scaleRatio);
    _line->setStyleSheet("background-color: rgb(29, 29, 30);");

    _searchbtn->setImagePrefixName("Search");
    _searchdelbtn->setImagePrefixName("seach_clos");
    _addbtn->setImagePrefixName("add");
    _delbtn->setImagePrefixName("delete");
    _menu->setImagePrefixName("luesuotuliebiao");

    //_searchwidget->setStyleSheet("border-image: url(:/res/search_box.png);");
    _searchEdit->setStyleSheet("border-image:url("");Color:rgb(107,107,107)");
    _searchbtn->setStyleSheet("border-image:url("")");
    _searchdelbtn->setStyleSheet("border-image:url("")");

    _searchEdit->setFrame(false);
    _searchdelbtn->setVisible(false);

    //upPlayModeBtn();
    _delbtn->setEnabled(false);
    _arrageGourp   = new QActionGroup(this);
    _listModeGourp = new QActionGroup(this);
    _listMode      = new QMenu();
    for(int i=0;i<5;++i) {
        _Action[i] = new QAction(_listMode);
        _Action[i]->setCheckable(true);
    }
    if(Global->getLibraryMode() == CGlobal::LibraryMode::IcoMode) {
        _Action[4]->setChecked(true);
    } else {
        _Action[3]->setChecked(true);
    }

    _Action[2]->setChecked(true);
    _Action[0]->setData("title");
    _Action[1]->setData("artist");
    _Action[2]->setData("date");
    _Action[3]->setData("list");
    _Action[4]->setData("icons");
    _listMode->addAction(_arrageGourp->addAction(_Action[0]));
    _listMode->addAction(_arrageGourp->addAction(_Action[1]));
    _listMode->addAction(_arrageGourp->addAction(_Action[2]));
    _listMode->addSeparator();
    _listMode->addAction(_listModeGourp->addAction(_Action[3]));
    _listMode->addAction(_listModeGourp->addAction(_Action[4]));
    _arrageGourp->setEnabled(false);

    _listMode->setStyleSheet(Global->menuStyleSheet());
    connect(_addbtn,SIGNAL(clicked()),SIGNAL(addClick()));
    connect(_delbtn,SIGNAL(clicked()),SIGNAL(delClick()));
    //connect(_playMode,SIGNAL(clicked()),SLOT(upplaymode()));
    connect(_searchEdit,SIGNAL(textEdited(QString)),SLOT(EditChange(QString)));
    connect(_searchdelbtn,SIGNAL(clicked()),SLOT(searchdelClick()));
    connect(_searchbtn,SIGNAL(clicked()),SLOT(searchClick()));
    connect(_menu,SIGNAL(clicked()),SLOT(menushow()));
    connect(_listModeGourp,SIGNAL(triggered(QAction*)),SIGNAL(listModemenuclk(QAction*)));
    connect(_arrageGourp,SIGNAL(triggered(QAction*)),SIGNAL(arragemenuclk(QAction*)));
    connect(this,SIGNAL(searchName(QString)),LibData,SLOT(searchChange(QString)));

    upUI();
    InitLanguage();
}

//void PlayListHead::upPlayModeBtn()
//{
//    if(_playListMode == 1) {
//        _playMode->setImagePrefixName("single");
//        _playMode->setshowDis();
//    } else if (_playListMode == 2) {
//        _playMode->setImagePrefixName("single");
//    } else if(_playListMode == 3){
//        _playMode->setImagePrefixName("Cycle");
//    } else {
//        _playMode->setImagePrefixName("random");
//    }
//}

void PlayListHead::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    _searchEdit->setFont(font);
    _listMode->setFont(font);
    _menu->setFont(font);
    _addbtn->setFont(font);
    _delbtn->setFont(font);

    _addbtn->move(HEADHEIGHT*_scaleRatio,0);
    _delbtn->move(_addbtn->x()+_addbtn->width(),0);
    //_playMode->move(_delbtn->x()+_delbtn->width(),0);
    _menu->move(this->width()-_menu->width()-10*_scaleRatio,(HEADHEIGHT-_menu->height())*_scaleRatio/2);

    _line->setGeometry(0,(HEADHEIGHT-1)*_scaleRatio,this->width(),1*_scaleRatio);
    _searchwidget->setGeometry(_menu->x()-_searchwidget->width()-30*_scaleRatio,(HEADHEIGHT-20)*_scaleRatio/2,220*_scaleRatio,20*_scaleRatio);
    _searchEdit->setGeometry(_searchbtn->width()+10*_scaleRatio,2*_scaleRatio,_searchwidget->width()-_searchbtn->width()*2-26*_scaleRatio,_searchwidget->height()-4*_scaleRatio);
    _searchbtn->move(6*_scaleRatio,(_searchwidget->height()-_searchbtn->height())/2);
    _searchdelbtn->move(_searchwidget->width()-_searchdelbtn->width()-10*_scaleRatio,(_searchwidget->height()-_searchdelbtn->height())/2);
}

void PlayListHead::InitLanguage()
{
    _Action[0]->setText(Lge->getLangageValue("LibHead/arrTitle"));
    _Action[1]->setText(Lge->getLangageValue("LibHead/arrArtist"));
    _Action[2]->setText(Lge->getLangageValue("LibHead/arrDate"));
    _Action[3]->setText(Lge->getLangageValue("LibHead/ViewList"));
    _Action[4]->setText(Lge->getLangageValue("LibHead/ViewIco"));
    _searchEdit->setPlaceholderText(Lge->getLangageValue("LibHead/search"));
    _searchbtn->setToolTip(Lge->getLangageValue("ToolTip/search"));
    _addbtn->setToolTip(Lge->getLangageValue("ToolTip/add"));
    _delbtn->setToolTip(Lge->getLangageValue("ToolTip/delete"));
    _menu->setToolTip(Lge->getLangageValue("ToolTip/menu"));
}

void PlayListHead::mouseMoveEvent(QMouseEvent *e)
{
    e->accept();
}

void PlayListHead::mouseDoubleClickEvent(QMouseEvent *e)
{
    e->accept();
}

void PlayListHead::menushow()
{
    _listMode->exec(QCursor::pos());
}

void PlayListHead::updelbtn(bool state)
{
    _delbtn->setEnabled(_isFavorites?false:state);
}

void PlayListHead::upMenuBtn(bool state)
{
    if(_menu->isVisible() != state) {
        _menu->setVisible(state);
    }
}

void PlayListHead::upaddbtn(bool state)
{
    _addbtn->setEnabled(state);
}

void PlayListHead::clearSearchText()
{
    _searchEdit->setText("");
    LibData->setSearchText("");
}

void PlayListHead::menuSortEnable(bool state)
{
    _arrageGourp->setEnabled(state);
    _listModeGourp->setEnabled(!state);
}

//void PlayListHead::upplaymode()
//{
//    if(_playListMode == 4)
//        _playListMode = 1;
//    else
//        _playListMode++;
//    Global->setPlayMode(_playListMode);
//    upPlayModeBtn();
//}

void PlayListHead::EditChange(QString)
{
    if(_searchEdit->text() == "")
        _searchdelbtn->setVisible(false);
    else {
        _searchdelbtn->setVisible(true);
    }

    emit searchName(_searchEdit->text());
}

void PlayListHead::searchdelClick()
{
    if(!_searchEdit->text().isEmpty()) {
        _searchEdit->setText("");
        _searchdelbtn->setVisible(false);
        emit searchName(_searchEdit->text());
    }
}

void PlayListHead::searchClick()
{
    if(!_searchEdit->text().isEmpty())
        emit searchName(_searchEdit->text());
}

void PlayListHead::resizeEvent(QResizeEvent *)
{
    upUI();
}
