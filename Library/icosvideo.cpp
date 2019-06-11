#include "icosvideo.h"
#include <QPixmap>
#include <QMouseEvent>
#include "librarydata.h"
#include "globalarg.h"
#include <QDebug>
IcosVideo::IcosVideo(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void IcosVideo::setDataSoure(const MEDIAINFO &info)
{
    _videoInfo = info;

    upUI();
    QString nameinfo = _videoInfo.title;
    int nameLabWidth = _videoName->width();
    int nameinfoWidth = _videoName->fontMetrics().width(nameinfo);
    if(nameLabWidth < nameinfoWidth) {
        QString showname = "";
        for(int i=0;i<nameinfo.size();i++) {
            showname = nameinfo.left(i).append("...");
            int showWidth = _videoName->fontMetrics().width(showname);
            if(showWidth > nameLabWidth) {
                showname = nameinfo.left(i-1).append("...");
                break;
            }
        }
        _videoName->setText(showname);
    } else {
        _videoName->setText(nameinfo);
    }

    QString details = LibData->getfileSize(_videoInfo.filesize,1);
    details.append(" ").append(LibData->sectotime(_videoInfo.duration));
    _videoDetails->setText(details);
    _videoName->setToolTip(_videoInfo.title);
    _videoDetails->setToolTip(details);

}

MEDIAINFO IcosVideo::getDataSoure()
{
    return _videoInfo;
}

void IcosVideo::setSelect(bool select, bool ctrl_down)
{
    if (ctrl_down)
        _videoInfo.isSelect = select ? !_videoInfo.isSelect:select;
    else
        _videoInfo.isSelect = select;
    if(_videoInfo.isSelect) {
        _videoImage->setStyleSheet("QLabel{border:2px solid rgb(0, 102, 204);}");
        _isSelect = true;
    } else {
        _videoImage->setStyleSheet("QLabel{border:2px solid rgb(17, 17, 18);}");
        _isSelect = false;
    }
    this->repaint();
}

bool IcosVideo::isSelect()
{
    return _isSelect;
}

void IcosVideo::updateLanguage()
{
    upUI();
}

void IcosVideo::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void IcosVideo::setplayFlag(bool state)
{
    if (state == true) {
        _playFlag->show();
        _playMovie->start();
    } else {
        _playMovie->stop();
        _playFlag->hide();
    }
}

void IcosVideo::Init()
{
    _videoImage   = new TransEventLabel(this);
    _videoName    = new TransEventLabel(this);
    _videoDetails = new TransEventLabel(this);
    _playFlag     = new TransEventLabel(this);
    _favirtesbtn  = new TransEventIcoClk(this);

    _playMovie = new QMovie(":/res/playing.gif");
    _playFlag->setScaledContents(true);
    _playFlag->setMovie(_playMovie);
    _playFlag->hide();
    connect(_favirtesbtn,SIGNAL(clicked()),SLOT(favoritesClick()));

    _scaleRatio = 1.0;
    _isSelect   = false;
    _videoImage->setStyleSheet("QLabel{border:2px solid rgb(17, 17, 18);}");
    _videoName->setStyleSheet("Color:rgb(182,183,184)");
    _videoDetails->setStyleSheet("Color:rgb(92,94,102)");
    _videoName->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _videoDetails->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    connect(LibData,SIGNAL(upplaymedia(MEDIAINFO)),SLOT(upplayflag(MEDIAINFO)));
}

void IcosVideo::upUI()
{
    int videoWidth = 114 * _scaleRatio;
    int videoHeight= 76  * _scaleRatio;
    int displayH = 0;
    int displayW = 0;
    if(_videoInfo.filepath != "") {
        if(LibData->getVideoStream(_videoInfo).aspectRatio != "" &&
           LibData->getVideoStream(_videoInfo).aspectRatio != "0:0"){
            QSize aspSize = LibData->getaspectRatio(_videoInfo);
            displayH = videoWidth*aspSize.height()/aspSize.width();
            displayW = videoWidth;
            if(displayH>videoHeight) {
                displayH=videoHeight;
                displayW = videoHeight*aspSize.width()/aspSize.height();
            }
        } else {
            displayH = videoHeight;
            displayW = videoHeight;
        }
    }
    _videoImage->setGeometry((this->width()-displayW)/2,videoHeight-displayH,displayW,displayH);
    int Ystart = _videoImage->y() + _videoImage->height()+3*_scaleRatio;
    int Ypos   = (this->height() - Ystart)/2;
    _favirtesbtn->setGeometry(_videoImage->x()+_videoImage->width()-22*_scaleRatio,_videoImage->y()+_videoImage->height()-22*_scaleRatio,20*_scaleRatio,20*_scaleRatio);

    _playFlag->setGeometry(6*_scaleRatio,Ystart,15*_scaleRatio,15*_scaleRatio);
    _videoName->setGeometry((this->width()-videoHeight)/2, Ystart, videoHeight,15*_scaleRatio);

    _videoDetails->setGeometry((this->width()-videoWidth)/2,Ystart + Ypos, videoWidth,15*_scaleRatio);

    QPixmap pixmap,showpixmap;
    if(_videoInfo.screenshot.isEmpty()) {
        if(_videoInfo.mediaType == 1)
            pixmap.load(":/res/png/videoshot.png");
        else
            pixmap.load(":/res/png/musicshot.png");
    } else {
        pixmap.loadFromData(_videoInfo.screenshot);
    }

    showpixmap=pixmap.scaled(_videoImage->width(),_videoImage->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    _videoImage->setPixmap(showpixmap);
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(_videoName->height()*4/5);
    _videoName->setFont(font);
    _videoDetails->setFont(font);
    upfavoritesIco();
}

void IcosVideo::upfavoritesIco()
{
    QString path;
    if (_videoInfo.isFavorites)
        path = ":/res/svg/love_on.svg";
    else
        path = ":/res/svg/love_nor.svg";
    _favirtesbtn->load(path);
}

void IcosVideo::favoritesClick()
{
    _videoInfo.isFavorites = !_videoInfo.isFavorites;
    upfavoritesIco();
    LibData->upmedia(_videoInfo);
}

void IcosVideo::upplayflag(const MEDIAINFO &videoinfo)
{
    if(_videoInfo.filepath == videoinfo.filepath) {
        _playFlag->show();
        _playMovie->start();
    } else {
        _playMovie->stop();
        _playFlag->hide();
    }
}

void IcosVideo::resizeEvent(QResizeEvent *)
{
    upUI();
}

