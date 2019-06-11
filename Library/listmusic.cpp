#include "listmusic.h"
#include "librarydata.h"
#include <QKeyEvent>
#include "globalarg.h"
ListMusic::ListMusic(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void ListMusic::updateLanguage()
{
    upUI();
}

void ListMusic::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void ListMusic::setDataSoure(const MEDIAINFO &mediaInfo)
{
    _mediaInfo = mediaInfo;
    _name->setText(_mediaInfo.title);
    QString codec = LibData->getMusicStream(_mediaInfo).codec;
    _type->setText(codec.toUpper());
    _info->setText(_mediaInfo.artist);
    _filetime->setText(LibData->sectotime(_mediaInfo.duration));
    _bitrate->setText(QString::number(LibData->getMusicStream(_mediaInfo).bitRate).append("kbps"));

}

MEDIAINFO ListMusic::getDataSoure()
{
    return _mediaInfo;
}

void ListMusic::setIsSelect(bool select, bool ctrldown)
{
    if(ctrldown)
        _mediaInfo.isSelect = !_mediaInfo.isSelect;
    else
        _mediaInfo.isSelect = select;
    if(!_mediaInfo.isSelect) {
        this->setStyleSheet("background-color: rgb(17, 17, 18);");
        _playFlag->setStyleSheet("background-color: rgb(17, 17, 18);");
    } else {
        this->setStyleSheet("background-color: rgb(31, 31, 31);");
        _playFlag->setStyleSheet("background-color: rgb(31, 31, 31);");
    }
}

void ListMusic::setplayFlag(bool state)
{
    if (state == true) {
        _playFlag->show();
        _playMovie->start();
    } else {
        _playMovie->stop();
        _playFlag->hide();
    }
}

void ListMusic::favoritesClick()
{
    _mediaInfo.isFavorites = !_mediaInfo.isFavorites;
    upfavoritesbtn();
    LibData->upmedia(_mediaInfo);
}

void ListMusic::Init()
{
    _name      = new TransEventLabel(this);
    _info      = new TransEventLabel(this);
    _type      = new TransEventLabel(this);
    _bitrate   = new TransEventLabel(this);
    _filetime  = new TransEventLabel(this);
    _playFlag  = new TransEventLabel(this);
    _favorites = new TransEventIcoClk(this);

    _playMovie = new QMovie(":/res/list_playing.gif");
    _playFlag->setScaledContents(true);
    _playFlag->setMovie(_playMovie);

    connect(_favorites,SIGNAL(clicked()),SLOT(favoritesClick()));
    _name->setStyleSheet("Color:rgb(182,183,184);");
    _info->setStyleSheet("Color:rgb(115,117,128);");
    _type->setStyleSheet("Color:rgb(115,117,128);");
    _bitrate->setStyleSheet("Color:rgb(115,117,128);");
    _filetime->setStyleSheet("Color:rgb(115,117,128);");
    _scaleRatio = 1.0;
    connect(LibData,SIGNAL(upplaymedia(MEDIAINFO)),SLOT(upplayflag(MEDIAINFO)));
}

void ListMusic::upUI()
{
    int musicheight = 30*_scaleRatio;
    int space   = 20*_scaleRatio;
    int palyWH  = 26*_scaleRatio;
    _playFlag->setGeometry(space-palyWH, (musicheight-palyWH)/2,palyWH,palyWH);
    _favorites->setGeometry(this->width()-22*_scaleRatio,(musicheight-15*_scaleRatio)/2,15*_scaleRatio,15*_scaleRatio);
    _filetime->setGeometry(_favorites->x()-60*_scaleRatio,0,50*_scaleRatio,musicheight);
    _bitrate->setGeometry(_filetime->x()-85*_scaleRatio,0,80*_scaleRatio,musicheight);
    _type->setGeometry(_bitrate->x()-90*_scaleRatio,0,83*_scaleRatio,musicheight);
    _info->setGeometry(_type->x()-65*_scaleRatio,0,60*_scaleRatio,musicheight);
    _name->setGeometry(space,0,_info->x()-space*2,musicheight);
    QFont font;
    font.setPixelSize(_name->height()*2/5);
    font.setFamily(Global->getFontFamily());
    _name->setFont(font);
    _info->setFont(font);
    _type->setFont(font);
    _bitrate->setFont(font);
    _filetime->setFont(font);
    upfavoritesbtn();
    upTitle();
}

void ListMusic::upfavoritesbtn()
{
    QString path;
    if (_mediaInfo.isFavorites)
        path = ":/res/svg/love_on.svg";
    else
        path = ":/res/svg/love_nor.svg";
    _favorites->load(path);
}

void ListMusic::upTitle()
{
    int LabWidth   = _name->width();
    int nameSize   = _mediaInfo.title.size();
    int nameLength = _name->fontMetrics().width(_mediaInfo.title);
    if(LabWidth<nameLength) {
       QString showtext = "";
       for(int i=0;i<nameSize;i++) {
           showtext = _mediaInfo.title.left(i).append("...");
           int showWidth = _name->fontMetrics().width(showtext);
           if(showWidth > LabWidth) {
               showtext = _mediaInfo.title.left(i-1).append("...");
               break;
           }
       }
       _name->setText(showtext);
    } else {
       _name->setText(_mediaInfo.title);
    }
}

void ListMusic::upplayflag(const MEDIAINFO &mediainfo)
{
    if((_mediaInfo.filepath == mediainfo.filepath)) {
        _playFlag->show();
        _playMovie->start();
    } else {
        _playMovie->stop();
        _playFlag->hide();
    }
}

void ListMusic::resizeEvent(QResizeEvent *)
{
    upUI();
}

void ListMusic::enterEvent(QEvent *)
{
    if(!_mediaInfo.isSelect) {
        this->setStyleSheet("background-color: rgb(51, 51, 56);");
        _playFlag->setStyleSheet("background-color: rgb(51, 51, 56);");
    }
}

void ListMusic::leaveEvent(QEvent *)
{
    if(!_mediaInfo.isSelect) {
        this->setStyleSheet("background-color: rgb(17, 17, 18);");
        _playFlag->setStyleSheet("background-color: rgb(17, 17, 18);");
    }
}

