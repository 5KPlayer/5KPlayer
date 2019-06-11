#include "urlmediawidget.h"
#include "urlformatwidget.h"
#include "librarydata.h"
#include "youtubedown.h"
#include "globalarg.h"
#include <QApplication>
#include <QCheckBox>
#include <QtDebug>
UrlMediaWidget::UrlMediaWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void UrlMediaWidget::updateLanguage()
{
    InitLanguage();
}

void UrlMediaWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _proWidget->updateUI(scaleRatio);
    _subtitleWidget->updateUI(scaleRatio);
    UpUI();
    for(int i=0;i<_formatList->count();++i) {
        _formatList->item(i)->setSizeHint(QSize(_formatList->width(),30*_scaleRatio));
        URLFormatWidget* urlFormat = (URLFormatWidget*) _formatList->itemWidget(_formatList->item(i));
        urlFormat->resize(_formatList->width(),30*_scaleRatio);
        urlFormat->updateUI(_scaleRatio);
    }

}

void UrlMediaWidget::setSubAnalyze(bool state)
{
    if(state) {
        _proWidget->show();
        _showSubBtn->hide();
    } else {
        _proWidget->hide();
        _showSubBtn->show();
    }
}

void UrlMediaWidget::UpDataSource(const MEDIAINFO &urlMedia, const QStringList checksub, const bool state)
{
    setSubAnalyze(state);
    _curMedia   = urlMedia;
    if(urlMedia.subtitleList.size() > 0) {
        _subtitleWidget->setSubTitle(urlMedia.subtitleList,checksub);
        _showSubBtn->hide();
    } else {
        _subtitleWidget->clearSubTitle();
    }
    UpUI();
    UpMedia(_curMedia);
    this->show();
}

void UrlMediaWidget::formatShowAll()
{
    _showAllFlag = !_showAllFlag;
    if(_showAllFlag) {
        _showAllBtn->setText(_hide);
    } else {
        _showAllBtn->setText(_show);
    }
    upFormatList();
}

void UrlMediaWidget::upFormatWidget(const STREAMINFO &format)
{
    _curFormat   = format;
    QString note = QString::number(format.resolution.height()).append("p");
    if(format.bitRate > 30 )
        note.append(" " + QString::number(format.bitRate));
    _note->setText(note);
    int noteWidth = _note->fontMetrics().width(_note->text());
    _note->setGeometry(_mediaThumbnail->width()-noteWidth,_mediaThumbnail->height()-15*_scaleRatio,noteWidth,15*_scaleRatio);
    for(int i=0;i<_formatList->count();++i) {
        URLFormatWidget* urlFormat = (URLFormatWidget*)_formatList->itemWidget(_formatList->item(i));
        if(urlFormat->getDataSouce().streamId != format.streamId) {
            urlFormat->setCheckBox(false);
        }
    }
}

void UrlMediaWidget::doneClick()
{
    _curMedia.formatID = matchAudioforVideo();
    emit doneMedia(_curMedia,_subtitleWidget->getCheckSubTitle());
}

void UrlMediaWidget::getSubClick()
{
    setSubAnalyze(true);
    emit clickGetSubtitle(_curMedia);
}

void UrlMediaWidget::analyzeSubFinish(const MEDIAINFO &media)
{
    if(_curMedia.webUrl == media.webUrl) {
        setSubAnalyze(false);
        _curMedia = media;
        upSubtitles(_curMedia.subtitleList);
    }
}

void UrlMediaWidget::Init()
{
    _scaleRatio        = 1.0;
    _showAllFlag       = false;
    _formatList        = new TransEvenListWidget(this);
    _mediaThumbnail    = new QLabel(this);
    _title             = new QLabel(this);
    _url               = new QLabel(this);
    _titleinfo         = new QLabel(this);
    _urlinfo           = new QLabel(this);
    _note              = new QLabel(_mediaThumbnail);
    _showAllBtn        = new DYLabel(this,true);
    _showSubBtn        = new DYLabel(this,true);
    _doneBtn           = new DYLabel(this,true);
    _subtitleWidget    = new URLSubtitlesWidget(this);
    _proWidget         = new UrlproWidget(this);

    //_mediaThumbnail->setScaledContents(true);
    //_proWidget->hide();
    _title->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _title->setStyleSheet("Color:rgb(182,183,184)");
    _titleinfo->setStyleSheet("Color:rgb(182,183,184)");
    _url->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _url->setStyleSheet("Color:rgb(182,183,184)");
    _urlinfo->setStyleSheet("Color:rgb(182,183,184)");
    _note->setStyleSheet("Color:rgb(0,128,0)");

    _formatList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _formatList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _formatList->scrollBarWidgets(Qt::AlignAbsolute);

    _showAllBtn->setAlignment(Qt::AlignCenter);

    _showSubBtn->setAlignment(Qt::AlignCenter);
    _showSubBtn->raise();

    _doneBtn->setAlignment(Qt::AlignCenter);

    connect(_showAllBtn,SIGNAL(clicked()),SLOT(formatShowAll()));
    connect(_showSubBtn,SIGNAL(clicked()),SLOT(getSubClick()));
    connect(_doneBtn,SIGNAL(clicked()),SLOT(doneClick()));

    connect(this,SIGNAL(clickGetSubtitle(MEDIAINFO)),YtbDown,SLOT(clickAnalyzeSub(MEDIAINFO)));
    connect(YtbDown,SIGNAL(sendanalyzeinfo(MEDIAINFO)),SLOT(analyzeSubFinish(MEDIAINFO)));
}

void UrlMediaWidget::InitLanguage()
{
    _subtitleWidget->updateLanguage();
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _titleinfo->setFont(font);
    _urlinfo->setFont(font);
    _note->setFont(font);
    font.setBold(true);
    _title->setFont(font);
    _url->setFont(font);

    _show  = Lge->getLangageValue("YTBDetils/show");
    _hide  = Lge->getLangageValue("YTBDetils/hide");
    _showAllBtn->setText(_show);
    _showSubBtn->setText(Lge->getLangageValue("YTBDetils/sub"));
    _doneBtn->setText(Lge->getLangageValue("YTBDetils/done"));
    _title->setText(Lge->getLangageValue("YTBDetils/title"));
    _url->setText(Lge->getLangageValue("YTBDetils/url"));
    for(int i=0;i<_formatList->count();++i) {
        URLFormatWidget* urlFormat = (URLFormatWidget*)_formatList->itemWidget(_formatList->item(i));
        urlFormat->updateLanguage();
    }
}

void UrlMediaWidget::UpUI()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _titleinfo->setFont(font);
    _urlinfo->setFont(font);
    _note->setFont(font);
    font.setBold(true);
    _title->setFont(font);
    _url->setFont(font);

    _formatList->setGeometry(40*_scaleRatio,100*_scaleRatio,this->width()-60*_scaleRatio,this->height()-220*_scaleRatio);
    _subtitleWidget->setGeometry(40*_scaleRatio,_formatList->y()+_formatList->height()+5*_scaleRatio,this->width()-60*_scaleRatio,50*_scaleRatio);
    _mediaThumbnail->setGeometry(40*_scaleRatio,15*_scaleRatio,120*_scaleRatio,80*_scaleRatio);
    _title->setGeometry(180*_scaleRatio,30*_scaleRatio,60*_scaleRatio,15*_scaleRatio);
    _titleinfo->setGeometry(242*_scaleRatio,30*_scaleRatio,this->width()-260*_scaleRatio,15*_scaleRatio);
    _url->setGeometry(180*_scaleRatio,60*_scaleRatio,60*_scaleRatio,15*_scaleRatio);
    _urlinfo->setGeometry(242*_scaleRatio,60*_scaleRatio,this->width()-260*_scaleRatio,15*_scaleRatio);
    int noteWidth = _note->fontMetrics().width(_note->text());
    _note->setGeometry(_mediaThumbnail->width()-noteWidth,_mediaThumbnail->height()-15*_scaleRatio,noteWidth,15*_scaleRatio);
    int proWidgetYpos = _formatList->y()+_formatList->height()+5*_scaleRatio+(57*_scaleRatio)/2;
    _proWidget->setGeometry((this->width()-57*_scaleRatio)/2,proWidgetYpos,57*_scaleRatio,57*_scaleRatio);

    _showAllBtn->setGeometry(this->width()-120*_scaleRatio,78*_scaleRatio,100*_scaleRatio,20*_scaleRatio);
    _showSubBtn->setGeometry(_formatList->x(),_formatList->y()+_formatList->height()+5*_scaleRatio,100*_scaleRatio,20*_scaleRatio);
    _doneBtn->setGeometry(this->width()-80*_scaleRatio,_subtitleWidget->y()+_subtitleWidget->height()+5*_scaleRatio,60*_scaleRatio,20*_scaleRatio);
}

void UrlMediaWidget::UpMedia(const MEDIAINFO &mediainfo)
{
    getformatId();
    QString title = mediainfo.title;
    title.append(".").append(_curFormat.ext);

    int LabWidth   = _titleinfo->width();
    int nameLength = _titleinfo->fontMetrics().width(title);
    if(LabWidth < nameLength) {
        QString showtext = "";
        int nameSize   = title.size();
        for(int i=0;i<nameSize;i++) {
            showtext = title.left(i).append("...");
            int showWidth = _titleinfo->fontMetrics().width(showtext);
            if(showWidth > LabWidth) {
                showtext = title.left(i-1).append("...");
                break;
            }
        }
        _titleinfo->setText(showtext);
    } else {
        _titleinfo->setText(title);
    }
    _titleinfo->setToolTip(title);

    _urlinfo->setText(mediainfo.webUrl);
    _videoformat.clear();
    _audioformat.clear();
    for(int i=0;i<mediainfo.streamList.size();++i) {
        if(mediainfo.streamList.at(i).streamType  == 1) {
            _audioformat.append(mediainfo.streamList.at(i));
        } else {
            _videoformat.append(mediainfo.streamList.at(i));
        }
    }
    LibData->sortFormat(&_videoformat);
    if(_videoformat.size() > 0) {
        QString note = QString::number(_curFormat.resolution.height()).append("p");
        if(_curFormat.bitRate  > 30 )
            note.append(" " + QString::number(_curFormat.bitRate));
        _note->setText(note);
        int noteWidth = _note->fontMetrics().width(_note->text());
        _note->setGeometry(_mediaThumbnail->width()-noteWidth,_mediaThumbnail->height()-15*_scaleRatio,noteWidth,15*_scaleRatio);
        QPixmap pixmap;
        if(pixmap.loadFromData(mediainfo.screenshot)) {
            pixmap = pixmap.scaled(_mediaThumbnail->width(),_mediaThumbnail->width()*pixmap.height()/pixmap.width());
        }
        _mediaThumbnail->setPixmap(pixmap); // 你在QLabel显示图片
        upFormatList();
    }
}

void UrlMediaWidget::upFormatList()
{
    _formatList->clear();
    if(_showAllFlag) {
        for(int i=0; i<_videoformat.size(); ++i) {
            QListWidgetItem * item = new QListWidgetItem (_formatList);
            URLFormatWidget* format = new URLFormatWidget();

            format->resize(_formatList->width(),30*_scaleRatio);
            format->updateUI(_scaleRatio);
            format->setDataSouce(_videoformat.at(i));
            if(_videoformat.at(i).streamId == _curFormat.streamId)
                format->setCheckBox(true);
            item->setSizeHint(QSize(_formatList->width(),30*_scaleRatio));
            connect(format,SIGNAL(upFormat(STREAMINFO)),SLOT(upFormatWidget(STREAMINFO)));
            _formatList->addItem(item);
            _formatList->setItemWidget(item,format);
        }
        for(int i=0; i<_audioformat.size(); ++i) {
            if(_audioformat.at(i).ext == "m4a") {
                QListWidgetItem * item = new QListWidgetItem (_formatList);
                URLFormatWidget* format = new URLFormatWidget();

                format->resize(_formatList->width(),30*_scaleRatio);
                format->updateUI(_scaleRatio);
                format->setDataSouce(_audioformat.at(i));
                if(_audioformat.at(i).streamId == _curFormat.streamId)
                    format->setCheckBox(true);
                item->setSizeHint(QSize(_formatList->width(),30*_scaleRatio));
                connect(format,SIGNAL(upFormat(STREAMINFO)),SLOT(upFormatWidget(STREAMINFO)));
                _formatList->addItem(item);
                _formatList->setItemWidget(item,format);
            }
        }
    } else {
        int audioCount = 0;
        for(int i=0; i<_audioformat.size(); ++i) {
            if(_audioformat.at(i).ext == "m4a") {
                audioCount++;
            }
        }
        if(audioCount>2) {
            audioCount = 2;
        }
        for(int i=0; i<(4-audioCount); ++i) {
            if(i>=_videoformat.size())
                break;
            QListWidgetItem * item = new QListWidgetItem (_formatList);
            URLFormatWidget* format = new URLFormatWidget();

            format->resize(_formatList->width(),30*_scaleRatio);
            format->updateUI(_scaleRatio);
            format->setDataSouce(_videoformat.at(i));
            if(_videoformat.at(i).streamId == _curFormat.streamId) {
                format->setCheckBox(true);
            }
            item->setSizeHint(QSize(_formatList->width(),30*_scaleRatio));
            connect(format,SIGNAL(upFormat(STREAMINFO)),SLOT(upFormatWidget(STREAMINFO)));
            _formatList->addItem(item);
            _formatList->setItemWidget(item,format);
        }
        int AddCount = 0;
        for(int i=0;i<_audioformat.size();i++) {
            if(_audioformat.at(i).ext == "m4a") {
                AddCount++;
                if(AddCount > audioCount) {
                    break;
                }
                QListWidgetItem * item = new QListWidgetItem (_formatList);
                URLFormatWidget* format = new URLFormatWidget();

                format->resize(_formatList->width(),30*_scaleRatio);
                format->updateUI(_scaleRatio);
                format->setDataSouce(_audioformat.at(i));
                if(_audioformat.at(i).streamId == _curFormat.streamId)
                    format->setCheckBox(true);
                item->setSizeHint(QSize(_formatList->width(),30*_scaleRatio));
                connect(format,SIGNAL(upFormat(STREAMINFO)),SLOT(upFormatWidget(STREAMINFO)));
                _formatList->addItem(item);
                _formatList->setItemWidget(item,format);
            }
        }
    }
    if(_videoformat.size()+_audioformat.size()<=4)
        _showAllBtn->setVisible(false);
    else
        _showAllBtn->setVisible(true);
}

QString UrlMediaWidget::matchAudioforVideo()
{
    QString formats = _curFormat.streamId;
    if(_audioformat.size() == 0)
        return formats;
    for(int i=0;i<_audioformat.size();i++) {
        if(_audioformat.at(i).streamId == _curFormat.streamId) {
            return formats;
        }
    }
    QString audioformatId = "";
    QString audioformat = "";
    if(_curFormat.ext == "mp4") {
        audioformat = "m4a";
    } else {
        audioformat = _curFormat.ext;
    }
    for(int i=0;i<_audioformat.size();++i) {
        if(_audioformat.at(i).ext.contains(audioformat)) {
            audioformatId = _audioformat.at(i).streamId;
            break;
        }
    }
    //_curMedia.curForamtId;
    if(!audioformatId.isEmpty()) {
        formats.append("+").append(audioformatId);
    } else {
        formats.append("+").append(_audioformat.at(0).streamId);
    }
    return formats;
}

void UrlMediaWidget::getformatId()
{
    QString formats = _curMedia.formatID;
    QString selectStream;
    if(formats.contains("+")) {
        QStringList formatIds = formats.split("+");
        selectStream = formatIds.at(0);
    } else {
        selectStream = formats;
    }
    for(int i=0;i<_curMedia.streamList.size();i++) {
        if(_curMedia.streamList.at(i).streamId == selectStream) {
            _curFormat = _curMedia.streamList.at(i);
            break;
        }
    }
}

void UrlMediaWidget::upSubtitles(const QStringList &subList)
{
    bool subup = false;
    QStringList checkList;
    checkList.clear();
    if(subList.size() > 0) {
        subup = true;
        _subtitleWidget->setSubTitle(subList,checkList);
    }

    if(!subup) {
        _subtitleWidget->clearSubTitle();
        _showSubBtn->show();
        _showSubBtn->raise();
    } else {
        _showSubBtn->hide();
    }
}

void UrlMediaWidget::resizeEvent(QResizeEvent *)
{
    UpUI();
    UpMedia(_curMedia);
}
