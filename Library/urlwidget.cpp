#include "urlwidget.h"
#include "librarydata.h"
#include "playermainwidget.h"
#include "librarydata.h"
#include "youtubedown.h"
#include <QDebug>
#include <QApplication>
#include "globalarg.h"
#include <QTextCodec>
URLWidget::URLWidget(QWidget *parent) : TransEventWidget(parent)
{
    Init();
}

void URLWidget::updateLanguage()
{
    InitLanguage();
}

void URLWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void URLWidget::setDataSoure(const MEDIAINFO &data,bool isDoneBtn)
{
    _wdgData = data;
    if(data.title.isEmpty()) {
        setAnalyPanl();
    } else {
        setDataPanl();
        _audioFormat.clear();
        _vidioFormat.clear();
        for(int f=0;f<_wdgData.streamList.size();f++){
            if(_wdgData.streamList.at(f).streamType == 1)
                _audioFormat.append(_wdgData.streamList.at(f));
            if(_wdgData.streamList.at(f).streamType == 2)
                _vidioFormat.append(_wdgData.streamList.at(f));
        }
        LibData->sortFormat(&_vidioFormat);

        if(!isDoneBtn) {
            if(_vidioFormat.size() > 0)
                _curVideoFormat = _vidioFormat.at(0);
            _wdgData.formatID = matchAudioforVideo();
        } else {
            _curVideoFormat = YtbDown->getformatStream(_wdgData);
        }
        upDatadisplay();
    }
}

MEDIAINFO URLWidget::getDataSoure()
{
    return _wdgData;
}

void URLWidget::setIsSelect(bool isSel)
{
    _isSelect = isSel;
    if(_isSelect) {
        this->setStyleSheet("background-color: rgb(64, 64, 64);");
        _line->setStyleSheet("background-color: rgb(74, 74, 74);");
    } else {
        this->setStyleSheet("background-color: rgb(17, 17, 18);");
        _line->setStyleSheet("background-color: rgb(32, 30, 31);");
    }
}

void URLWidget::setDownState(bool state)
{
    if(state) {
        _downfinish = false;
        _detailed->hide();
        _down->hide();
        _downPro->show();
        _downProSize->show();
    } else {
        _detailed->show();
        _down->show();
        _downPro->hide();
        _downProSize->hide();
    }
}

void URLWidget::setDownProcess(double process)
{
    _downPro->setAngle(process);
}

void URLWidget::setMediaSubtitle(QStringList subList)
{
    _wdgData.subtitleList = subList;
}

void URLWidget::Init()
{
    _scaleRatio= 1.0;
    _curSelect = 0;
    _isSelect = false;
    _isDown   = false;
    _isAnaly  = false;
    _ico            = new  TransEventLabel(this);
    _title          = new  TransEventLabel(this);
    _name           = new  TransEventLabel(this);
    _resolution     = new  TransEventLabel(this);
    _resinfo        = new  TransEventLabel(this);
    _fileSize       = new  TransEventLabel(this);
    _size           = new  TransEventLabel(this);
    _downProSize    = new  TransEventLabel(this);
    _detailed       = new  SvgButton(this);
    _down           = new  SvgButton(this);
    _delBtn         = new  SvgButton(this);
    _downPro        = new  DownButton(this);
    _line           = new QLabel(this);
    _definition     = new QLabel(_ico);
    _reAnalyze      = new DYLabel(this,true);

    _reAnalyze->hide();
    InitLanguage();
    _title->setStyleSheet("color:rgb(161,164,178);");
    _name->setStyleSheet("color:rgb(161,164,178);");
    _definition->setStyleSheet("color:rgb(0,128,0);");
    _resolution->setStyleSheet("color:rgb(161,164,178);");
    _resinfo->setStyleSheet("color:rgb(161,164,178);");
    _fileSize->setStyleSheet("color:rgb(161,164,178);");
    _size->setStyleSheet("color:rgb(161,164,178);");
    _downProSize->setStyleSheet("color:rgb(161,164,178);");
    _title->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    _resolution->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    _fileSize->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    _downProSize->setAlignment(Qt::AlignCenter);
    _reAnalyze->setAlignment(Qt::AlignCenter);
    _ico->setStyleSheet("background-color: rgb(0, 0, 0);");
    _line->setStyleSheet("background-color: rgb(32, 30, 31);");

    _downPro->hide();
    _downProSize->hide();
    _detailed->setImagePrefixName("setting");
    _delBtn->setImagePrefixName("strike_out");
    _down->setImagePrefixName("Download");
    connect(_reAnalyze,SIGNAL(clicked()),SLOT(clickReAnaly()));
    connect(_delBtn,SIGNAL(clicked()),SLOT(delClick()));
    connect(_detailed,SIGNAL(clicked()),SLOT(detailedClick()));
    connect(_down,SIGNAL(clicked()),SLOT(downClick()));
    connect(_downPro,SIGNAL(clicked()),SLOT(clickstopDown()));
    connect(this,SIGNAL(downUrl(MEDIAINFO)),YtbDown,SLOT(clickDownbtn(MEDIAINFO)));
    connect(this,SIGNAL(stopDown(MEDIAINFO)),YtbDown,SLOT(proStop(MEDIAINFO)));
    connect(YtbDown,SIGNAL(sendDownProcess(QString,double)),SLOT(upProcessInfo(QString,double)));
}

void URLWidget::InitLanguage()
{
    if(_isAnaly) {
        _title->setText(Lge->getLangageValue("YTBVIDEO/urltitle"));
        if(_reAnalyze->isHidden()) {
            _resinfo->setText(Lge->getLangageValue("YTBVIDEO/analying"));
        } else {
            _resinfo->setText(Lge->getLangageValue("YTBVIDEO/analyfail"));
        }
    } else {
        _title->setText(Lge->getLangageValue("YTBVIDEO/title"));
    }
    _resolution->setText(Lge->getLangageValue("YTBVIDEO/resolution"));
    _fileSize->setText(Lge->getLangageValue("YTBVIDEO/filesize"));
    _down->setSvgText(Lge->getLangageValue("YTBVIDEO/down"));
    _down->setSvgTextCenter(true);
    _reAnalyze->setText(Lge->getLangageValue("YTBVIDEO/reanaly"));
    QFont font;
    font.setFamily(Global->getFontFamily());
    _down->setSvgTextFontSize(font,10);
    _down->setSvgTextColor(QColor(97,38,0),QColor(139,54,0));
    font.setPixelSize(12*_scaleRatio);
    _title->setFont(font);
    _name->setFont(font);
    _resolution->setFont(font);
    _resinfo->setFont(font);
    _fileSize->setFont(font);
    _size->setFont(font);
    _reAnalyze->setFont(font);
    _definition->setFont(font);
}

void URLWidget::upUI()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _title->setFont(font);
    _name->setFont(font);
    _resolution->setFont(font);
    _resinfo->setFont(font);
    _fileSize->setFont(font);
    _size->setFont(font);
    _reAnalyze->setFont(font);
    _definition->setFont(font);
    int distance = 5*_scaleRatio;
    int labelH   = 15*_scaleRatio;
    _ico->setGeometry(distance,(this->height()-80*_scaleRatio)/2,90*_scaleRatio,80*_scaleRatio);
    int defLength = _definition->fontMetrics().width(_definition->text());
    _definition->setGeometry(_ico->width()-defLength,_ico->height()-18*_scaleRatio,defLength,15*_scaleRatio);
    _detailed->setGeometry(_ico->x()+_ico->width()+distance,(this->height()-40*_scaleRatio)/2,40*_scaleRatio,40*_scaleRatio);
    _down->setGeometry(this->width()-130*_scaleRatio,(this->height()-20*_scaleRatio)/2,80*_scaleRatio,20*_scaleRatio);
    _downPro->setGeometry(this->width()-100*_scaleRatio,(this->height()-40*_scaleRatio)/2,40*_scaleRatio,40*_scaleRatio);
    _downProSize->setGeometry(this->width()-130*_scaleRatio,_downPro->y()+_downPro->height(),100*_scaleRatio,15*_scaleRatio);
    _reAnalyze->setGeometry(this->width()-130*_scaleRatio,(this->height()-20*_scaleRatio)/2,80*_scaleRatio,20*_scaleRatio);
    int labelXpos = _detailed->x()+_detailed->width()+10*_scaleRatio;
    int labelYposStart = this->height()/3;
    int labelYpos = (this->height()/3-labelH)/2;
    _title->setGeometry(labelXpos,labelYpos,80*_scaleRatio,labelH);
    _resolution->setGeometry(labelXpos,labelYposStart+labelYpos,80*_scaleRatio,labelH);
    _fileSize->setGeometry(labelXpos,labelYposStart*2+labelYpos,80*_scaleRatio,labelH);

    labelXpos = _title->x()+_title->width()+5*_scaleRatio;
    int w = _down->x() - labelXpos;

    _name->setGeometry(labelXpos,labelYpos,w,labelH);
    _resinfo->setGeometry(labelXpos,labelYposStart+labelYpos,110*_scaleRatio,labelH);
    _size->setGeometry(labelXpos,labelYposStart*2+labelYpos,65*_scaleRatio,labelH);
    _delBtn->setGeometry(this->width()-20*_scaleRatio,5*_scaleRatio,15*_scaleRatio,15*_scaleRatio);
    _line->setGeometry(0,this->height()-3*_scaleRatio,this->width()-distance,1*_scaleRatio);
    uptitleName();
}

void URLWidget::uptitleName()
{
    QString name =  _wdgData.title;
    name.append(".").append(_curVideoFormat.ext);
    if(_isAnaly)  {
        name = _wdgData.webUrl;
    }
    int LabWidth   = _name->width();
    int nameLength = _name->fontMetrics().width(name);
    if(LabWidth<nameLength) {
       QString showtext = "";
       int nameSize   = name.size();
       for(int i=0;i<nameSize;i++) {
           showtext = name.left(i).append("...");
           int showWidth = _name->fontMetrics().width(showtext);
           if(showWidth > LabWidth) {
               showtext = name.left(i-1).append("...");
               break;
           }
       }
       _name->setText(showtext);
    } else {
       _name->setText(name);
    }
    _name->setToolTip(name);
}

void URLWidget::upDatadisplay()
{
    QString resinfo = "";
    QString title;
    int64_t videofileSize = 0;
    int64_t audiofileSize = 0;
    QString definition = "";
    _StreamInfo curVideoStream = YtbDown->getformatStream(_wdgData);

    title = _wdgData.title;
    title.append(".").append(curVideoStream.ext.toUpper());

    bool isVideo = true;
    if(_wdgData.formatID.contains("+")) {
        for(int i=0;i<_vidioFormat.size();i++) {
            if(_vidioFormat.at(i).streamId == curVideoStream.streamId) {
                if(_vidioFormat.at(i).resolution.width()!=0) {
                    resinfo = QString::number(_vidioFormat.at(i).resolution.width()).append("x")\
                            .append(QString::number(_vidioFormat.at(i).resolution.height()));
                    definition = QString::number(_vidioFormat.at(i).resolution.height()).append("P");
                    if(_vidioFormat.at(i).bitRate > 30 )
                        definition.append(" " + QString::number(_vidioFormat.at(i).bitRate));
                } else {
                    resinfo = "N/A";
                }
            }
        }
        videofileSize = curVideoStream.fileSize;
        QString audioId = getaudioformat(_wdgData.formatID);
        for(int i=0;i<_audioFormat.size();i++) {
            if(_audioFormat.at(i).streamId == audioId) {
                audiofileSize = _audioFormat.at(i).fileSize;
                break;
            }
        }
        resinfo.append(" ").append("[").append(curVideoStream.ext.toUpper()).append("]");
    } else {
        isVideo = false;
        audiofileSize = curVideoStream.fileSize;
        if(curVideoStream.resolution.width() != 0) {
            resinfo = QString::number(curVideoStream.resolution.width()).append("x")\
                    .append(QString::number(curVideoStream.resolution.height())) \
                    .append(" [").append(curVideoStream.ext.toUpper()).append("]");
        } else {
            resinfo = QString("[").append(curVideoStream.ext.toUpper()).append("]");
        }
    }

    _definition->setText(definition);
    int defLength = _definition->fontMetrics().width(definition);
    _definition->setGeometry(_ico->width()-defLength,_ico->height()-18*_scaleRatio,defLength,15*_scaleRatio);
    _resinfo->setText(resinfo);
    QString fileSize = "N/A";
    if(isVideo) {
        if(videofileSize != 0) {
            fileSize = LibData->getfileSize(videofileSize+audiofileSize,0);
        }
    } else {
        if(audiofileSize > 0) {
            fileSize = LibData->getfileSize(audiofileSize,0);
        }
    }

    _size->setText(fileSize);
    uptitleName();
    QPixmap pixmap;
    if(pixmap.loadFromData(_wdgData.screenshot)) {
        pixmap = pixmap.scaled(90*_scaleRatio,90*_scaleRatio*pixmap.height()/pixmap.width());
    }
    _ico->setPixmap(pixmap); // 你在QLabel显示图片
}

void URLWidget::setAnalyPanl()
{
    _detailed->setVisible(false);
    _down->setVisible(false);
    _resolution->setVisible(false);
    _fileSize->setVisible(false);
    _size->setVisible(false);
    _definition->setVisible(false);
    _isAnaly = true;
    QPixmap pixmap;
    _ico->setPixmap(pixmap); // 清除QLabel显示图片
    _title->setText(Lge->getLangageValue("YTBVIDEO/urltitle"));

    if(_wdgData.isFavorites) {
        _reAnalyze->show();
        _resinfo->setText(Lge->getLangageValue("YTBVIDEO/analyfail"));
    } else {
        _reAnalyze->hide();
        _resinfo->setText(Lge->getLangageValue("YTBVIDEO/analying"));
        YtbDown->startAnalyPath(_wdgData.webUrl);
    }
    uptitleName();
}

void URLWidget::setDataPanl()
{
    _detailed->setVisible(true);
    _down->setVisible(true);
    _resolution->setVisible(true);
    _fileSize->setVisible(true);
    _size->setVisible(true);
    _definition->setVisible(true);
    _isAnaly = false;
    _reAnalyze->hide();
    _title->setText(Lge->getLangageValue("YTBVIDEO/title"));
}

QString URLWidget::matchAudioforVideo()
{
    QString formats = _curVideoFormat.streamId;
    if(_audioFormat.size() == 0)
        return formats;
    for(int i=0;i<_audioFormat.size();i++) {
        if(_audioFormat.at(i).streamId == _curVideoFormat.streamId) {
            return formats;
        }
    }
    QString audioformatId = "";
    QString audioformat = "";
    if(_curVideoFormat.ext == "mp4") {
        audioformat = "m4a";
    } else {
        audioformat = _curVideoFormat.ext;
    }
    for(int i=0;i<_audioFormat.size();++i) {
        if(_audioFormat.at(i).ext.contains(audioformat)) {
            audioformatId = _audioFormat.at(i).streamId;
            break;
        }
    }
    //_curMedia.curForamtId;
    if(!audioformatId.isEmpty()) {
        formats.append("+").append(audioformatId);
    } else {
        formats.append("+").append(_audioFormat.at(0).streamId);
    }
    return formats;
}

void URLWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}

void URLWidget::enterEvent(QEvent *)
{
    if(!_isSelect) {
        this->setStyleSheet("background-color: rgb(51, 51, 56);");
        _line->setStyleSheet("background-color: rgb(61, 61, 66);");
    }
}

void URLWidget::leaveEvent(QEvent *)
{
    if(!_isSelect) {
        this->setStyleSheet("background-color: rgb(17, 17, 18);");
        _line->setStyleSheet("background-color: rgb(32, 30, 31);");
    }
}

QString URLWidget::getvideoformat(const QString &formatids)
{
    QString reFormat = "";
    if(!formatids.contains("+")) {
        for(int i=0;i<_vidioFormat.size();i++) {
            if(_vidioFormat.at(i).streamId == formatids) {
                reFormat = formatids;
                break;
            }
        }
    } else {
        QString format = formatids;
        QStringList info = format.split("+");
        if(info.size()>=1)
            reFormat = info.at(0);
    }
    return reFormat;
}

QString URLWidget::getaudioformat(const QString &formatids)
{
    QString reFormat = "";
    if(!formatids.contains("+")) {
        for(int i=0;i<_audioFormat.size();i++) {
            if(_audioFormat.at(i).streamId == formatids) {
                reFormat = formatids;
                break;
            }
        }
    } else {
        QString format = formatids;
        QStringList info = format.split("+");
        if(info.size()>=2)
            reFormat = info.at(1);
    }
    return reFormat;
}

QString URLWidget::checkFormatIds(const QString &formats)
{
    if(_audioFormat.size() == 0)
        return formats;
    // 只有音频时
    for(int i=0;i<_audioFormat.size();i++) {
        if(_audioFormat.at(i).streamId == formats){
            return formats;
        }
    }
    // 检查视频和音频的format是否匹配
    QString audioformatId = "";
    QString audioformat = "";
    QString videoformat = getvideoformat(formats);
    QString videoExt = "";
    for(int i=0;i<_vidioFormat.size();++i) {
        if(_vidioFormat.at(i).streamId == videoformat) {
            videoExt = _vidioFormat.at(i).ext;
            break;
        }
    }
    if(videoExt == "mp4") {
        audioformat = "m4a";
    } else {
        audioformat = videoExt;
    }
    for(int i=0;i<_audioFormat.size();++i) {
        if(_audioFormat.at(i).ext.contains(audioformat)) {
            audioformatId = _audioFormat.at(i).streamId;
            break;
        }
    }
    //_curMedia.curForamtId;
    if(!audioformatId.isEmpty()) {
        videoformat.append("+").append(audioformatId);
    } else {
        videoformat.append("+").append(_audioFormat.at(0).streamId);
    }
    return videoformat;
}

void URLWidget::delClick()
{
    emit deleteClick(_wdgData);
}

void URLWidget::detailedClick()
{
    emit showDetail(_wdgData);
}

void URLWidget::downClick()
{
    // 如果下载的项目在分析字幕中，则不能进行下载，在线程中通过url来进行区别，分析和下载同时进行会混淆
    QList<MEDIAINFO> analySubList = YtbDown->getSubAnalyzeList();
    for(int i=0;i<analySubList.size();i++) {
        if(analySubList.at(i).webUrl == _wdgData.webUrl) {
            return;
        }
    }
    upProcessInfo(_wdgData.webUrl,0);
    _downfinish = false;
    _detailed->hide();
    _down->hide();
    _downPro->show();
    _downProSize->show();
    _wdgData.formatID = checkFormatIds(_wdgData.formatID);
    _wdgData.ext = _curVideoFormat.ext;
    emit downUrl(_wdgData);
}

void URLWidget::clickstopDown()
{
    emit stopDown(_wdgData);
    _detailed->show();
    _down->show();
    _downPro->hide();
    _downProSize->hide();
}

void URLWidget::clickReAnaly()
{
    YtbDown->startAnalyPath(_wdgData.webUrl);
    _resinfo->setText(Lge->getLangageValue("YTBVIDEO/analying"));
    _resinfo->setToolTip(Lge->getLangageValue("YTBVIDEO/analying"));
    _reAnalyze->hide();
}

void URLWidget::upProcessInfo(const QString &objectName, double process)
{
    if(objectName == _wdgData.webUrl) {
        _downPro->setAngle(process);
        if(process == 100) {
            _downfinish = true;
        } else if(process == 200) {
            _detailed->show();
            _down->show();
            _down->setEnabled(true);
            _downPro->hide();
            _downProSize->hide();
        }
        double cursize = 0.0f;
        QString videoId = getvideoformat(_wdgData.formatID);
        QString AudioId = getaudioformat(_wdgData.formatID);

        if(videoId.isEmpty())
            _downfinish = true;
        if(_downfinish) {
            for(int i=0;i<_audioFormat.size();++i) {
                if(_audioFormat.at(i).streamId == AudioId) {
                    cursize = _audioFormat.at(i).fileSize;
                    break;
                }
            }
        } else {
            for(int i=0;i<_vidioFormat.size();++i) {
                if(_vidioFormat.at(i).streamId == videoId) {
                    cursize = _vidioFormat.at(i).fileSize;
                    break;
                }
            }
        }
        QString info = "N/A";
        if(cursize>0) {
            if(_downfinish && process == 100) {
                info = LibData->getfileSize(0,0);
            } else {
                info = LibData->getfileSize(cursize*process/100.0,0);
            }
            info.append("/").append(LibData->getfileSize(cursize,0));
        }
        _downProSize->setText(info);
    }
}

