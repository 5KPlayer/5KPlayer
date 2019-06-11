#include "listsvideo.h"
#include <QDesktopServices>
#include <QApplication>
#include <QUrl>
#include <QDir>
#include <QMovie>
#include <QDebug>
#include <windows.h>
#include <ShellAPI.h>
#include <QTextCodec>
#include "librarydata.h"
#include "openfile.h"
#include "transeventlabel.h"
#include "transeventico.h"
#include "transeventicoclk.h"
#include "transeventlabelbtn.h"
#include "globalarg.h"
#include "dylabel.h"
#include "ffmpegconvert.h"
#include "adlogic.h"

ListsVideo::ListsVideo(QWidget *parent) : TransEventWidget(parent)
{
    _ListMode = false;
    this->setMouseTracking(true);
    Init();
}

ListsVideo::ListsVideo(bool isListMode, QWidget *parent) : TransEventWidget(parent)
{
    _ListMode = isListMode;
    Init();
}

void ListsVideo::setDataSoure(const MEDIAINFO &data)
{
    if(!_ListMode && data.mediaType == 1) {
        _convertBar->hide();
        _convertbtn->show();
    } else {
        _convertbtn->hide();
    }
    _mediaInfo = data;
    if(data.mediaType == 1)
        _streamInfo = LibData->getVideoStream(data);
    else
        _streamInfo = LibData->getMusicStream(data);

    // 设置convert标记
    bool bConvert = false;
    QList<CONVERTMEDIA> conlist = FFmpeg->getConvertList();
    for(int i=0;i<conlist.size();++i) {
        if(QString::compare(conlist.at(i).media.filepath , _mediaInfo.filepath,Qt::CaseInsensitive) == 0) {
            setConvertProValue(conlist.at(i).proValue);
            bConvert = true;
            break;
        }
    }
    if(!bConvert)
        setConvertProValue(-1);


    QFileInfo fileinfo(_mediaInfo.filepath);
//    if(LibData->checkMediaisAirPlay(_mediaInfo)) {
//        _airPlaySup->hide();
//    } else {
//        _airPlaySup->show();
//    }
    _airNoSupfixName->setText(fileinfo.suffix().toLower());
//    upAirPlayCon(LibData->b_isConCPT);
    upUI();
}

void ListsVideo::setIsSelect(bool select, bool ctrldown)
{
    if(ctrldown)
        _mediaInfo.isSelect = !_mediaInfo.isSelect;
    else
        _mediaInfo.isSelect = select;
    if(!_mediaInfo.isSelect) {
        this->setStyleSheet("background-color: rgb(14, 14, 15);");
        _playFlag->setStyleSheet("background-color: rgb(14, 14, 15);");
    } else {
        this->setStyleSheet("background-color: rgb(31, 31, 31);");
        _playFlag->setStyleSheet("background-color: rgb(31, 31, 31);");
    }
}

MEDIAINFO ListsVideo::getDataSoure()
{
    return _mediaInfo;
}

void ListsVideo::updateLanguage()
{
    upVideoInfo();
    InitLanguage();
    upUI();
}

void ListsVideo::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void ListsVideo::setplayFlag(bool state)
{
    if (state == true) {
        _playFlag->show();
        _playMovie->start();
    } else {
        _playMovie->stop();
        _playFlag->hide();
    }
}

bool ListsVideo::getSupAirPlay()
{
    return _airNoSup->isHidden();
}

void ListsVideo::setConvertProValue(double proValur)
{
    if(proValur == -1) {
        if(_ListMode) {
            disconnect(FFmpeg,SIGNAL(sendConvertProcess(QString,double)),this,SLOT(upProcess(QString,double)));
        }
        if(!_convertBar->isHidden()) {
            _convertBar->hide();
            _convertbtn->show();
        }
    } else {
        connect(FFmpeg,SIGNAL(sendConvertProcess(QString,double)),this,SLOT(upProcess(QString,double)));
        if(_convertBar->isHidden()) {
            _convertBar->show();
        }
        double value = proValur/_mediaInfo.duration*100;
        if(_convertBar->value()<value) {
            _convertBar->setValue(value);
        }
    }
}

void ListsVideo::Init()
{
    _videoIco   = new TransEventLabel(this);
    _titleIco   = new TransEventIco(this);
    _titleName  = new TransEventLabel(this);
    _airPlaySup = new TransEventLabel(this);
    _details    = new TransEventLabel(this);
    _path       = new TransEventLabel(this);
    _playFlag   = new TransEventLabel(this);
    _Line       = new TransEventLabel(this);
    _icoLine    = new TransEventLabel(this);
    _icoLine2   = new TransEventLabel(this);
    _favoritesbtn = new TransEventIcoClk(_videoIco);
    _openpathbtn  = new TransEventIcoClk(this);
    _convertbtn   = new DYLabel(this);
    _convertBar   = new QProgressBar(this);

    _airNoSup           = new TransEventLabel(this);
    _airNoSupTitle      = new TransEventLabel(_airNoSup);
    _airNoSupConvert    = new TransEventLabel(_airNoSup);
    _airNoSupIco        = new TransEventLabel(_airNoSup);
    _airNoSupfixName    = new TransEventLabel(_airNoSup);
    _airSupcodec        = new TransEventLabel(_airNoSup);
    _airSupfixName      = new TransEventLabel(_airNoSup);
    _airSupcodecIco     = new TransEventLabel(_airNoSup);
    _airSupfixNameIco   = new TransEventLabel(_airNoSup);
    _videoConvert       = new TransEventLabelBtn(_airNoSup,"border-image: url(:/res/png/video_converter_nor.png);");

    _airSupfixName->setText("MP4");
    _airSupcodec->setText("H264");

    _airNoSupIco->setPixmap(QPixmap(":/res/png/airplay_white.png"));
    _airSupcodecIco->setPixmap(QPixmap(":/res/png/right.png"));
    _airSupfixNameIco->setPixmap(QPixmap(":/res/png/right.png"));

    _airNoSupfixName->setStyleSheet("border-image: url(:/res/png/airplay_sender_geshi.png);color:rgb(255,0,0)");
    _airNoSup->setStyleSheet("background-color: rgb(71, 76, 83);");
    _airNoSupTitle->setStyleSheet("color: rgb(253, 151, 41);");
    _airNoSupConvert->setStyleSheet("color: rgb(255, 255, 255);");
    _airSupfixName->setStyleSheet("color: rgb(255, 255, 255);");
    _airSupcodec->setStyleSheet("color: rgb(255, 255, 255);");
    _videoConvert->setEnterStyle("border-image: url(:/res/png/video_converter_dis.png);");

    _airNoSupTitle->setAlignment(Qt::AlignCenter);
    _airNoSupConvert->setAlignment(Qt::AlignCenter);
    _airNoSupfixName->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    _videoConvert->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    _airSupfixName->setAlignment(Qt::AlignCenter);
    _airSupcodec->setAlignment(Qt::AlignCenter);

    _openpathbtn->setmouseHand(true);
    _playMovie = new QMovie(":/res/list_playing.gif");
    _playFlag->setScaledContents(true);
    _playFlag->setMovie(_playMovie);

    _titleName->setStyleSheet("color:rgb(182,183,184)");
    _details->setStyleSheet("color:rgb(115,117,128)");
    _path->setStyleSheet("color:rgb(115,117,128)");
    _Line->setStyleSheet("background-color: rgb(30, 30, 31);");
    _icoLine->setStyleSheet("background-color: rgb(31, 31, 35);");
    _icoLine2->setStyleSheet("background-color: rgb(46, 46, 51);");
    _convertbtn->setStyleSheet("border-image: url(:/res/png/convter_to.png);color:rgb(255,255,255)");

    _airPlaySup->setStyleSheet("border-image: url(:/res/png/not_supperted.png);color:rgb(145,60,3)");

    _convertbtn->setAlignment(Qt::AlignCenter);
    _convertbtn->setMouseHead(true);
    _convertBar->setTextVisible(false);
    _videoIco->setAlignment(Qt::AlignCenter);
    _airPlaySup->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    _titleName->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _details->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _path->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    if(!_ListMode) {
        this->setStyleSheet("background-color: rgb(39, 39, 44);");
        _favoritesbtn->hide();
        _playFlag->hide();
        _convertbtn->show();
        _icoLine->show();
        _icoLine2->show();
        _Line->hide();
    } else {
        _icoLine->hide();
        _icoLine2->hide();
        _Line->show();
        _convertbtn->hide();
    }

    _airNoSup->raise();
    _airNoSup->hide();
    _airPlaySup->hide();
    _convertBar->hide();
    _Menu = new QMenu();
    _MP3  = new QAction();
    _AAC  = new QAction();
    _MP4  = new QAction();
    _Menu->addAction(_MP3);
    _Menu->addAction(_AAC);
    _Menu->addSeparator();
    _Menu->addAction(_MP4);

    _Menu->setStyleSheet(Global->menuStyleSheet());
    connect(_videoConvert,SIGNAL(clicked(bool)),SLOT(airplayConvert(bool)));
    connect(_favoritesbtn,SIGNAL(clicked()),SLOT(favoritesClick()));
    connect(_openpathbtn,SIGNAL(clicked()),SLOT(openpathClick()));
    connect(_convertbtn,SIGNAL(clicked()),SLOT(convertClick()));
    connect(_MP4,SIGNAL(triggered(bool)),SLOT(ActionMP4Click(bool)));
    connect(_MP3,SIGNAL(triggered(bool)),SLOT(ActionMP3Click(bool)));
    connect(_AAC,SIGNAL(triggered(bool)),SLOT(ActionAACClick(bool)));
    connect(_Menu,SIGNAL(aboutToHide()),SLOT(converthide()));
    connect(LibData,SIGNAL(upplaymedia(MEDIAINFO)),SLOT(upplayflag(MEDIAINFO)));

    _titleIco->load(QString(":/res/svg/movies-light.svg"));
    _openpathbtn->load(QString(":/res/svg/others_light.svg"));
    _scaleRatio = 1.0;
    InitLanguage();
}

void ListsVideo::upVideoInfo()
{
    QString detai = Lge->getLangageValue("ListVideo/details");
    QString where = Lge->getLangageValue("ListVideo/where");
    if(_mediaInfo.filepath.isEmpty())
        return;
    int LabWidth   = this->width()- _titleName->x()-20*_scaleRatio;
    int nameSize   = _mediaInfo.title.size();
    int nameLength = _titleName->fontMetrics().width(_mediaInfo.title);
    if(LabWidth<nameLength) {
       QString showtext = "";
       for(int i=0;i<nameSize;i++) {
           showtext = _mediaInfo.title.left(i).append("...");
           int showWidth = _titleName->fontMetrics().width(showtext);
           if(showWidth > LabWidth) {
               showtext = _mediaInfo.title.left(i-1).append("...");
               break;
           }
       }
       _titleName->setText(showtext);
    } else {
       _titleName->setText(_mediaInfo.title);
    }
    _titleName->setToolTip(_mediaInfo.title);
    QString detailsTip = QString(_streamInfo.codec).append(",");
    if(_mediaInfo.mediaType == 1) {
        detailsTip.append(QString::number(_streamInfo.resolution.width())).append("x")\
                .append(QString::number(_streamInfo.resolution.height())).append(",");
    }
    detailsTip.append(LibData->sectotime(_mediaInfo.duration));

    QString videoDetails = QString(_streamInfo.codec).append(",");
    QFileInfo fileinfo(_mediaInfo.filepath);
    QString details = detai + " " + videoDetails.toUpper();
    QString detailsTemp = details;
    QString detailsshow = details;
    if(_mediaInfo.mediaType == 1)
        detailsshow = details.append(QString::number(_streamInfo.resolution.width())).append("x")\
                                     .append(QString::number(_streamInfo.resolution.height())).append(",");
    if(_details->fontMetrics().width(detailsshow) > _details->width()) {
        _details->setText(detailsTemp.append("..."));
    } else {
        detailsTemp = detailsshow;
        details = detailsshow.append(LibData->sectotime(_mediaInfo.duration));
        if(_details->fontMetrics().width(details) > _details->width()) {
            _details->setText(detailsTemp.append("..."));
        } else {
            _details->setText(details);
        }
    }
    _details->setToolTip(detailsTip);
    QString path = where + " " + QDir::toNativeSeparators(fileinfo.absolutePath());
    int pathMaxW   = this->width()- _path->x()-20*_scaleRatio;
    int pathSize   = path.size();
    int pathLength = _path->fontMetrics().width(path);
    if(pathMaxW<pathLength) {
        QString showtext = "";
        for(int i=0;i<pathSize;i++) {
            showtext = path.left(i).append("...");
            int showWidth = _path->fontMetrics().width(showtext);
            if(showWidth > LabWidth) {
                showtext = path.left(i-1).append("...");
                break;
            }
        }
       _path->setText(showtext);
       int pathW = _path->fontMetrics().width(showtext);
       _path->resize(pathW,20*_scaleRatio);
    } else {
        _path->resize(_path->fontMetrics().width(path),20*_scaleRatio);
        _path->setText(path);
    }

    _openpathbtn->setGeometry(_path->x()+_path->width(),_path->y(),20*_scaleRatio,20*_scaleRatio);
    upFavoritesbtn();
}

void ListsVideo::upFavoritesbtn()
{
    QString path;
    if (_mediaInfo.isFavorites)
        path = ":/res/svg/love_on.svg";
    else
        path = ":/res/svg/love_nor.svg";
    _favoritesbtn->load(path);
}

void ListsVideo::upUI()
{
    int titleH = 20*_scaleRatio;
    int airplay= 18*_scaleRatio;
    int playWH = 26*_scaleRatio;

    int deatilsH = 16*_scaleRatio;

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setWeight(57);
    font.setLetterSpacing(QFont::AbsoluteSpacing,0.5);
    font.setPixelSize(14*_scaleRatio);
    _titleName->setFont(font);
    font.setPixelSize(12*_scaleRatio);
    _details->setFont(font);
    _path->setFont(font);
    _convertbtn->setFont(font);
    _Menu->setFont(font);
    font.setPixelSize(9*_scaleRatio);
    _airPlaySup->setFont(font);

    _playFlag->setGeometry((35*_scaleRatio-playWH)/2,(100*_scaleRatio-playWH)/2,playWH,playWH);

    int icoWidth = 94*_scaleRatio;
    int icoHieght= 80*_scaleRatio;
    if(_ListMode)
        _videoIco->setGeometry(35*_scaleRatio,(this->height()-icoHieght)/2,icoWidth,icoHieght);
    else
        _videoIco->setGeometry(2,(this->height()-icoHieght)/2,icoWidth,icoHieght);
    _favoritesbtn->setGeometry(_videoIco->width()-titleH, _videoIco->height()-titleH, titleH, titleH);
    _titleIco->setGeometry(_videoIco->x()+_videoIco->width()+30*_scaleRatio ,_videoIco->y(), titleH, titleH);
    _titleName->setGeometry(_titleIco->x()+_titleIco->width()+6*_scaleRatio, _titleIco->y(), (this->width()-_titleIco->x()-_titleIco->width()), titleH);
    _icoLine->setGeometry(_titleIco->x()-10*_scaleRatio,_titleName->y()+_titleName->height()+3*_scaleRatio,this->width() - _titleIco->x(),1*_scaleRatio);
    _icoLine2->setGeometry(_icoLine->x(),_icoLine->y()+1*_scaleRatio,this->width() - _titleIco->x(),1*_scaleRatio);
    int pathW = _path->fontMetrics().width(_path->text())+3*_scaleRatio;

    int detailsWidth;
    if(this->width() > 580*_scaleRatio) {
        detailsWidth = (580-100)*_scaleRatio - _titleIco->x();
    } else {
        detailsWidth = this->width() - _titleIco->x() - 100*_scaleRatio;
    }
    if(_airPlaySup->isHidden()) {
        _details->setGeometry(_titleIco->x(), (this->height()-deatilsH)/2, detailsWidth, deatilsH);
        _convertbtn->setGeometry(_details->x()+_details->width(), _details->y(),    100*_scaleRatio, titleH);
        _convertBar->setGeometry(_convertbtn->x(),_convertbtn->y(),_convertbtn->width(),_convertbtn->height());
        int Ystart = this->height()-_details->y();
        _path->setGeometry(_titleIco->x(),_details->y()+(Ystart-deatilsH)/2, pathW, deatilsH);
        _openpathbtn->setGeometry(_path->x()+_path->width(),_path->y(),        titleH,        titleH);
    } else {
        int Ystart = _icoLine2->y()+_icoLine2->height()+3*_scaleRatio;
        int Ypos = (this->height()-Ystart)/3;
        int airplayWidth = _airPlaySup->fontMetrics().width(_airPlaySup->text())+10*_scaleRatio;
        _airPlaySup->setGeometry(_titleIco->x()+_titleIco->width()/2,Ystart+(Ypos-_airPlaySup->height())/2,airplayWidth,airplay);

        int Ydetails = _airPlaySup->y()+ _airPlaySup->height() + 3*_scaleRatio;
        _details->setGeometry(_titleIco->x(), Ydetails, detailsWidth, deatilsH);
        _convertbtn->setGeometry(_details->x()+_details->width(), _details->y()+(_details->height()-titleH)/2, 100*_scaleRatio, titleH);
        _convertBar->setGeometry(_convertbtn->x(),_convertbtn->y(),_convertbtn->width(),_convertbtn->height());

        int Ypath = _details->y()+ _details->height() + 3 *_scaleRatio;
        _path->setGeometry(_titleIco->x(), Ypath, pathW, deatilsH);
        _openpathbtn->setGeometry(_path->x()+_path->width(),_path->y(),  titleH,        titleH);
    }

    QFont airfont;
    airfont.setFamily(Global->getFontFamily());
    _airNoSup->setGeometry(this->width()-220*_scaleRatio,0,220*_scaleRatio,this->height());
    airfont.setPixelSize(14*_scaleRatio);
    _airNoSupTitle->setFont(airfont);
    _airNoSupTitle->setGeometry(0,0,220*_scaleRatio,20*_scaleRatio);
    airfont.setPixelSize(9*_scaleRatio);
    _airNoSupConvert->setFont(airfont);
    _airSupfixName->setFont(airfont);
    _airSupcodec->setFont(airfont);

    airfont.setPixelSize(16*_scaleRatio*4/5);
    _videoConvert->setFont(airfont);
    _airNoSupConvert->setGeometry(80*_scaleRatio,21*_scaleRatio,115*_scaleRatio,15*_scaleRatio);
    _airSupfixName->setGeometry(110*_scaleRatio,37*_scaleRatio,30*_scaleRatio,15*_scaleRatio);
    _airSupcodec->setGeometry(110*_scaleRatio,53*_scaleRatio,30*_scaleRatio,15*_scaleRatio);
    _airNoSupIco->setGeometry(45*_scaleRatio,30*_scaleRatio,30*_scaleRatio,20*_scaleRatio);
    _airNoSupfixName->setGeometry(32*_scaleRatio,53*_scaleRatio,56*_scaleRatio,14*_scaleRatio);
    _airSupcodecIco->setGeometry(142*_scaleRatio,40*_scaleRatio,10*_scaleRatio,8*_scaleRatio);
    _airSupfixNameIco->setGeometry(142*_scaleRatio,56*_scaleRatio,10*_scaleRatio,8*_scaleRatio);
    _videoConvert->setGeometry(75*_scaleRatio,80*_scaleRatio,120*_scaleRatio,16*_scaleRatio);

    _Line->setGeometry(10*_scaleRatio,this->height()-2*_scaleRatio,this->width()-30*_scaleRatio,1*_scaleRatio);
    if(_mediaInfo.filepath != "") {
        int videoWidth ;
        int videoheight;
        STREAMINFO videoStream = LibData->getVideoStream(_mediaInfo);

        if(videoStream.resolution.height() == 0) {
            videoWidth = icoHieght>icoWidth?icoWidth:icoHieght;
            videoheight = videoWidth;
        } else {
            if(videoStream.resolution.height()>videoStream.resolution.width()) {
                videoheight = icoHieght;
                videoWidth  = videoheight*videoStream.resolution.width()/videoStream.resolution.height();
            } else {
                videoWidth  = icoWidth;
                videoheight = icoWidth*videoStream.resolution.height()/videoStream.resolution.width();
            }
        }

        _videoIco->setStyleSheet("background-color: rgb(0, 0, 0)");
        QPixmap pixmap,showpixmap;
        if(_mediaInfo.screenshot.isEmpty()) {
            if(_mediaInfo.mediaType == 1)
                pixmap.load(":/res/png/videoshot.png");
            else
                pixmap.load(":/res/png/musicshot.png");
            _videoIco->setPixmap(pixmap);
        } else {
            pixmap.loadFromData(_mediaInfo.screenshot);
            showpixmap=pixmap.scaled(videoWidth, videoheight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            _videoIco->setPixmap(showpixmap);
        }
    }
    _convertBar->setStyleSheet(QString("QProgressBar {border: 1px solid grey;border-radius: %1px;background-color: rgb(39,39,44);}"
                                       "QProgressBar::chunk {background-color: rgb(114, 114, 116);}").arg(5*_scaleRatio));
    upFavoritesbtn();
    upVideoInfo();
}

void ListsVideo::InitLanguage()
{
    _airNoSupTitle->setText(Lge->getLangageValue("ListVideo/airNST"));
    _airNoSupConvert->setText(Lge->getLangageValue("ListVideo/airNSC"));
    _videoConvert->setText(Lge->getLangageValue("ListVideo/videoCov"));
    _airPlaySup->setText(Lge->getLangageValue("ListVideo/airNST"));
    _convertbtn->setText(Lge->getLangageValue("ListVideo/convert"));
    _MP3->setText(Lge->getLangageValue("ListVideo/MenuMp3"));
    _AAC->setText(Lge->getLangageValue("ListVideo/MenuAac"));
    _MP4->setText(Lge->getLangageValue("ListVideo/MenuMp4"));
}

bool ListsVideo::fileNotExist(QString filepath)
{
    bool isExist = true;
    if (!LibData->checkMediaExits(filepath)){
        QString filelog = "\"" + _mediaInfo.filepath + "\" ";
        filelog.append(Lge->getLangageValue("RecentFile/info"));
        OpenFileDialog->openTooltipDialog(filelog);
        isExist = false;
    }
    return isExist;
}

#include <QSettings>
void ListsVideo::ActionMP4Click(bool)
{
    if(!fileNotExist(_mediaInfo.filepath))
        return;

    //如果安装了就打开软件 否则跳转链接
    const QString key("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    QSettings set(key, QSettings::NativeFormat);
    foreach (QString key, set.childGroups()) {
        if(key.startsWith("WinX HD Video Converter Deluxe")) {
            set.beginGroup(key);
            if(set.contains("InstallLocation"))
            {
                QString path = set.value("InstallLocation").toString();
                path.append("WinX_HD_Video_Converter_Deluxe.exe");
                QStringList args;
                args << "-add" << _mediaInfo.filepath << "018000001";
                QProcess::startDetached(path, args);
            }
            set.endGroup();
            return;
        }
    }
    QString strValue("convert");
    if(LibData->getselectitem() == "yVideos" || LibData->getselectitem() == "yMusic") {
        strValue = "convert_youtube";
    }
    Global->openLanguageUrl(strValue);
}

void ListsVideo::ActionMP3Click(bool)
{
    if(!fileNotExist(_mediaInfo.filepath))
        return;
    _convertBar->show();
    _convertBar->setValue(0);
    _convertbtn->hide();

    FFmpeg->ffmpegConvertClick(_mediaInfo,".mp3");
    if(_ListMode) {
        connect(FFmpeg,SIGNAL(sendConvertProcess(QString,double)),this,SLOT(upProcess(QString,double)));
    }
}

void ListsVideo::ActionAACClick(bool)
{
    if(!fileNotExist(_mediaInfo.filepath))
        return;

    _convertBar->show();
    _convertBar->setValue(0);
    _convertbtn->hide();

    FFmpeg->ffmpegConvertClick(_mediaInfo,".aac");
    if(_ListMode) {
        connect(FFmpeg,SIGNAL(sendConvertProcess(QString,double)),this,SLOT(upProcess(QString,double)));
    }
}

void ListsVideo::favoritesClick()
{
    _mediaInfo.isFavorites = !_mediaInfo.isFavorites;
    LibData->upmedia(_mediaInfo);
    upFavoritesbtn();
}

void ListsVideo::convertClick()
{
    _Menu->popup(QCursor::pos());
}
#include <windows.h>
#include <shlobj.h>
void ListsVideo::openpathClick()
{
    ITEMIDLIST *pidl = ILCreateFromPathW(_mediaInfo.filepath.toStdWString().c_str());
    if(pidl) {
        SHOpenFolderAndSelectItems(pidl,0,0,0);
        ILFree(pidl);
    }
}

void ListsVideo::converthide()
{
    if(_ListMode)
        _convertbtn->hide();
}

void ListsVideo::upplayflag(const MEDIAINFO &mediainfo)
{
    if((_mediaInfo.filepath == mediainfo.filepath) && _ListMode) {
        _playFlag->show();
        _playMovie->start();
    } else {
        _playMovie->stop();
        _playFlag->hide();
    }
}

void ListsVideo::airplayConvert(bool)
{
    Global->openLanguageUrl("convert");
}

void ListsVideo::upProcess(QString objName, double time)
{
    if(objName == _mediaInfo.filepath) {
        if(time == -1) {
            if(_ListMode) {
                disconnect(FFmpeg,SIGNAL(sendConvertProcess(QString,double)),this,SLOT(upProcess(QString,double)));
            }
            if(!_convertBar->isHidden()) {
                _convertBar->hide();
                _convertbtn->show();
            }
        } else {
            if(_convertBar->isHidden()) {
                _convertBar->show();
            }
            double value = time/_mediaInfo.duration*100;
            if(_convertBar->value()<value) {
                _convertBar->setValue(value);
            }
            if(time>_mediaInfo.duration)
                _convertBar->setValue(100);
        }
    }
}

void ListsVideo::upAirPlayCon(bool con)
{
    if(con && !_airPlaySup->isHidden()) {
        _airNoSup->show();
    } else {
        _airNoSup->hide();
    }
}

void ListsVideo::resizeEvent(QResizeEvent *)
{
    upUI();
}

void ListsVideo::enterEvent(QEvent *)
{
    if(_ListMode && _mediaInfo.mediaType == 1)
        _convertbtn->show();

    if(!_mediaInfo.isSelect) {
        this->setStyleSheet("background-color: rgb(51, 51, 56);");
        _playFlag->setStyleSheet("background-color: rgb(51, 51, 56);");
    }
}

void ListsVideo::leaveEvent(QEvent *)
{
    if((_Menu->isHidden()) &&_ListMode)
        _convertbtn->hide();
    if((!_mediaInfo.isSelect) && (_ListMode)) {
        this->setStyleSheet("background-color: rgb(14, 14, 15);");
        _playFlag->setStyleSheet("background-color: rgb(14, 14, 15);");
    }
}
