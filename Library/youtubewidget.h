#ifndef YOUTUBEWIDGET_H
#define YOUTUBEWIDGET_H

#include <QWidget>
#include "transeventwidget.h"
#include "transevenlistwidget.h"
#include "updateinterface.h"
#include "youtubehead.h"
#include "analyzewidget.h"
#include "urlmediawidget.h"
#include "dystructu.h"
#include "urlprowidget.h"
#include "urlwidget.h"
#include "listyoutubewidget.h"

class YoutubeWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit YoutubeWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
signals:
    void sendDownProcess(QString objName,double pro);
public slots:
    void upDetail(const MEDIAINFO &media);                                                             // 点击列表中的详细时的事件
    void detailDone(const MEDIAINFO &media, const QStringList &checkSubtitle);                         // 点击详细面板的done时的事件                               // 点击列表中的删除按钮时的事件

private:
    void Init();
    void upUI();

protected:
    void resizeEvent(QResizeEvent *);
private:
    YoutubeHead             *_head;             // Ytb面板的head
    AnalyzeWidget           *_analyzeWidget;    // Ytb面板的分析面板
    ListYoutubeWidget       *_urlListwidget;    // Ytb面板的Url媒体列表
    UrlMediaWidget          *_mediaWidget;      // Ytb面板的媒体详细面板
    double                   _scaleRatio;

};

#endif // YOUTUBEWIDGET_H
