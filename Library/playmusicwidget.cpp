#include "playmusicwidget.h"
#include "librarydata.h"
#include <QMouseEvent>
#include <QMimeData>
#include "openfile.h"
#include "cmenu.h"
#include "globalarg.h"

PlayMusicWidget::PlayMusicWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    this->setMouseTracking(true);
    this->setAcceptDrops(true);

    this->setStyleSheet(".PlayMusicWidget{background-color: rgb(14, 14, 15);}");
    Init();
}

void PlayMusicWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void PlayMusicWidget::setPlayMedia(const MEDIAINFO &media,bool isAirPlay)
{
    _titleData      = "";
    _imageData.clear();
    _imagePath = "";
    _mediaInfo = media;
    if(isAirPlay) {
        _airPlayFlag->setVisible(true);
        _favorites->setVisible(false);
    } else {
        _airPlayFlag->setVisible(false);
        _favorites->setVisible(true);
        upfavoritesbtn();
    }
}

void PlayMusicWidget::setPlayImage(const QByteArray &imageData)
{
    _imageData = imageData;
    _imagePath = "";
    if(!_airPlayFlag->isVisible()) {
        _airPlayFlag->setVisible(true);
        _favorites->setVisible(false);
    }
    upUI();
}

void PlayMusicWidget::setPlayTitle(const QString &titleName)
{
    _titleData = titleName;
    if(!_airPlayFlag->isVisible()) {
        _airPlayFlag->setVisible(true);
        _favorites->setVisible(false);
    }
    upUI();
}

void PlayMusicWidget::setLiveInfo(const QString &imagePath, const QString &title)
{
    _mediaInfo.title = "";
    _imagePath = imagePath;
    _imageData.clear();
    _titleData = title;
    _airPlayFlag->setVisible(false);
    _favorites->setVisible(false);
    upUI();
}

void PlayMusicWidget::upWidgtet()
{
    bool isExits = false;
    QList<MEDIAINFO> musicList = LibData->getmusicData();
    for(int i=0;i<musicList.size();++i) {
        if(musicList.at(i).filepath == _mediaInfo.filepath) {
            _mediaInfo = musicList.at(i);
            isExits = true;
            break;
        }
    }
    if(!isExits) {
         QList<MEDIAINFO> videoList = LibData->getmedaiData();
         for(int i=0;i<videoList.size();++i) {
             if(videoList.at(i).filepath == _mediaInfo.filepath) {
                 _mediaInfo = videoList.at(i);
                 break;
             }
         }
    }

    _musicTitle->setText(_mediaInfo.title);
    qDebug() << "xxxxxxxxx"<< _musicTitle->text();
    upfavoritesbtn();
    upUI();
}

void PlayMusicWidget::Init()
{


    _musiclog       = new TransEventLabel(this);
    _musicIco       = new TransEventLabel(_musiclog);

    _musicInfo      = new TransEventLabel(_musiclog);
    _airPlayFlag    = new TransEventIco(_musicInfo);
    _musicTitle     = new TransEventLabel(_musicInfo);
    _favorites      = new TransEventIcoClk(_musicInfo);

    _scaleRatio     = 1.0;
    _titleData      = "";
    _imageData.clear();

    _musicInfo->raise();
    _musicMovie     = new QMovie(":/res/music-bottom.gif");
    _musiclog->setScaledContents(true);
    _musiclog->setMovie(_musicMovie);

    _musicMovie->start();
    connect(_favorites,SIGNAL(clicked()),SLOT(favoritesClick()));

    _musicTitle->setAlignment(Qt::AlignCenter);
    _musicTitle->setStyleSheet("color:rgb(255,255,255)");
    _airPlayFlag->setVisible(false);
}

void PlayMusicWidget::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setBold(true);
    font.setPixelSize(20*_scaleRatio);
    _musicTitle->setFont(font);

    QString musictle = "";
    if(_titleData.isEmpty()) {
        musictle = _mediaInfo.title;
    } else {
        musictle = _titleData;
    }
    int titleWidth = _musicTitle->fontMetrics().width(musictle);

    if(titleWidth > this->width() - 100*_scaleRatio) {
        titleWidth = this->width() - 100*_scaleRatio;
        QString showtext = "";
        for(int i=0;i<musictle.size();i++) {
            showtext = musictle.left(i).append("...");
            int showWidth = _musicTitle->fontMetrics().width(showtext);
            if(showWidth > titleWidth) {
                showtext = musictle.left(i-1).append("...");
                break;
            }
        }

        _musicTitle->setText(showtext);
    } else {
        _musicTitle->setText(musictle);
    }
    if(_airPlayFlag->isVisible())
        _musicTitle->setGeometry((this->width()-titleWidth)/2,0,titleWidth,30*_scaleRatio);
    else
        _musicTitle->setGeometry((this->width()-titleWidth)/2,0,titleWidth,30*_scaleRatio);

    _musiclog->setGeometry((this->width()-320*_scaleRatio)/2,0,360*_scaleRatio,360*_scaleRatio);
    _musicInfo->setGeometry(0,320*_scaleRatio,this->width(),40*_scaleRatio);
    _airPlayFlag->setGeometry(10*_scaleRatio,5*_scaleRatio,25*_scaleRatio,25*_scaleRatio);

    _favorites->setGeometry(_musicTitle->x()+_musicTitle->width()+5*_scaleRatio,5*_scaleRatio,25*_scaleRatio,25*_scaleRatio);
    _airPlayFlag->load(QString(":/res/svg/airRecord-light.svg"));
    _musicIco->setGeometry(30*_scaleRatio,70*_scaleRatio,220*_scaleRatio,220*_scaleRatio);
    QPixmap pixmap,showpixmap;
    QByteArray imageByte;
    if(_imagePath.isEmpty()) {
        if(_imageData.isNull()) {
            imageByte = _mediaInfo.screenshot;
        } else {
            imageByte = _imageData;
        }

        if(imageByte.isNull()) {
            pixmap.load(QString(":/res/png/music.png"));// loadFromData(_mediaInfo.screenshot);
        } else {
            pixmap.loadFromData(imageByte);
        }
    } else {
        pixmap.load(_imagePath);
    }
    if(pixmap.width() == 0 || pixmap.height() ==0) {
        pixmap.load(QString(":/res/png/music.png"));
    }
    int pixWidth,pixHeith;
    if(pixmap.width() > pixmap.height()) {
        pixWidth = 220*_scaleRatio;
        pixHeith = 220*_scaleRatio * pixmap.height()/pixmap.width();
    } else if(pixmap.width() == pixmap.height()) {
        pixWidth = 220*_scaleRatio;
        pixHeith = 220*_scaleRatio;
    } else {
        pixWidth = 220*_scaleRatio*pixmap.width()/pixmap.height();
        pixHeith = 220*_scaleRatio;
    }
    showpixmap = pixmap.scaled(pixWidth, pixHeith, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    _musicIco->setPixmap(showpixmap);
    qDebug() << _musicTitle->text() << _musicInfo->geometry();
    _musicTitle->show();
    _musicInfo->raise();
    upfavoritesbtn();
}

void PlayMusicWidget::upfavoritesbtn()
{
    QString path;
    if (_mediaInfo.isFavorites)
        path = ":/res/svg/love_on.svg";
    else
        path = ":/res/svg/love_nor.svg";
    _favorites->load(path);
}

void PlayMusicWidget::favoritesClick()
{
    _mediaInfo.isFavorites = !_mediaInfo.isFavorites;
    upfavoritesbtn();
    LibData->upmedia(_mediaInfo,true);
}

void PlayMusicWidget::resizeEvent(QResizeEvent *e)
{
    upUI();
    e->accept();
}

void PlayMusicWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton) {
        Menu->showRightMenu(cursor().pos());
    }
    e->ignore();
}

void PlayMusicWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->urls().count() == 1)
        e->setDropAction(Qt::MoveAction);
    else
        e->setDropAction(Qt::IgnoreAction);
    e->accept();
}

void PlayMusicWidget::dropEvent(QDropEvent *e)
{
    OpenFileDialog->openFile(e->mimeData()->urls().at(0).toLocalFile());
    e->accept();
}
