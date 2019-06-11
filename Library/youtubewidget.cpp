#include "youtubewidget.h"
#include "sqloperation.h"
#include "librarydata.h"
#include "youtubedown.h"
#include <QDebug>
#include <QWheelEvent>
#include <QScrollBar>
YoutubeWidget::YoutubeWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void YoutubeWidget::updateLanguage()
{
    _head->updateLanguage();
    _analyzeWidget->updateLanguage();
    _mediaWidget->updateLanguage();
    _urlListwidget->updateLanguage();
}

void YoutubeWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _head->updateUI(scaleRatio);
    _mediaWidget->updateUI(scaleRatio);
    _analyzeWidget->updateUI(scaleRatio);
    _urlListwidget->updateUI(scaleRatio);
    upUI();
}

void YoutubeWidget::upDetail(const MEDIAINFO &media)
{
    bool isAnalySub = false;
    // 是否在分析字幕列表，在的话，显示分析字幕的图标
    QList<MEDIAINFO> analyzeList = YtbDown->getSubAnalyzeList();
    for(int i=0;i<analyzeList.size();++i) {
        if(analyzeList.at(i).webUrl == media.webUrl){
            qDebug("zaifenxilieb");
            _mediaWidget->setSubAnalyze(true);
            isAnalySub = true;
            break;
        }
    }
    QStringList checksub;
    checksub.clear();

    if(!isAnalySub) {
        // 是否有选中的字幕，
        QList<URLMEDIADATA>  subtitleData = YtbDown->getSubTitleList();
        for(int i=0;i<subtitleData.size();++i) {
            if(subtitleData.at(i).mediainfo.webUrl == media.webUrl) {
                checksub = subtitleData.at(i).checkSubtitle;
                break;
            }
        }
    }
    _mediaWidget->UpDataSource(media,checksub,isAnalySub);

    _urlListwidget->hide();
    _mediaWidget->show();
}

void YoutubeWidget::detailDone(const MEDIAINFO &media, const QStringList &checkSubtitle)
{
    _urlListwidget->upSelStream(media);
    YtbDown->upSubTitleList(media,checkSubtitle);
    _urlListwidget->show();
    _mediaWidget->hide();
}

void YoutubeWidget::Init()
{
    _scaleRatio = 1.0;
    _head           = new YoutubeHead(this);
    _analyzeWidget  = new AnalyzeWidget(this);
    _urlListwidget  = new ListYoutubeWidget(this);
    _mediaWidget    = new UrlMediaWidget(this);

    _mediaWidget->hide();
    _analyzeWidget->setStyleSheet("background-color: rgb(24, 24, 24)");

    connect(_urlListwidget, SIGNAL(showDetailSig(MEDIAINFO)),SLOT(upDetail(MEDIAINFO)));
    connect(_mediaWidget,SIGNAL(doneMedia(MEDIAINFO,QStringList)),SLOT(detailDone(MEDIAINFO,QStringList)));

    connect(_analyzeWidget, SIGNAL(addUrlAnalyze(QString)),                 _urlListwidget,SLOT(addanalysisWeb(QString)));                  // 分析路径
    connect(YtbDown,        SIGNAL(analyFinsh(QString,QList<MEDIAINFO>)),   _urlListwidget,SLOT(analyWebFinsh(QString,QList<MEDIAINFO>)));  // 分析成功
    connect(YtbDown,        SIGNAL(analyError(QString)),                    _urlListwidget,SLOT(analyWebFail(QString)));                    // 分析失败
    connect(YtbDown,        SIGNAL(sendanalyzeinfo(MEDIAINFO)),             _urlListwidget,SLOT(analyWebSubFinish(MEDIAINFO)));             // 分析字幕成功
    connect(YtbDown,        SIGNAL(removeUrlWidget(MEDIAINFO)),             _urlListwidget,SLOT(delWebWidget(MEDIAINFO)));                  // 下载完成删除

    // 初始化urlListWidget
    _urlListwidget->initYtbData(LibData->getUrlMedia());
}

void YoutubeWidget::upUI()
{
    _head->setGeometry(0,0,this->width(),100*_scaleRatio);
    _analyzeWidget->setGeometry(0,_head->height(),this->width(),66*_scaleRatio);
    int Ypos = _analyzeWidget->y()+_analyzeWidget->height();
    _mediaWidget->setGeometry(0,Ypos,this->width()-5*_scaleRatio,this->height()-Ypos - 4*_scaleRatio);
    _urlListwidget->setGeometry(0,Ypos,this->width()-5*_scaleRatio,this->height()-Ypos - 4*_scaleRatio);
}

void YoutubeWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}

