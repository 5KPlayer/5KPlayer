#ifndef URLMEDIAWIDGET_H
#define URLMEDIAWIDGET_H

#include <QWidget>
#include "dylabel.h"
#include "transeventwidget.h"
#include "transevenlistwidget.h"
#include "updateinterface.h"
#include "dystructu.h"
#include "urlsubtitleswidget.h"
#include "urlprowidget.h"
/*!
 * \brief The UrlMediaWidget class
 * Youtube 中使用的详细媒体界面，其他包含了一个媒体中的所有能下载的格式、分辨率和字幕
 */
class UrlMediaWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit UrlMediaWidget(QWidget *parent = 0);

    void updateLanguage() ;
    void updateUI(const qreal &scaleRatio);

    void setSubAnalyze(bool state);             // 分析字幕显示的图片
signals:
    void doneMedia(MEDIAINFO,QStringList);      // 点击done发出的信号
    void clickGetSubtitle(MEDIAINFO);           // 点击获取字幕发出的信号
public slots:
    void UpDataSource(const MEDIAINFO &urlMedia,const QStringList checksub,const bool state);    // 更新详细面板信息
    void formatShowAll();                                   // 点击showAll按钮
    void upFormatWidget(const STREAMINFO &format);          // 选中其他流的事件
    void doneClick();                                       // 点击面板done的按钮
    void getSubClick();                                     // 点击分析字幕按钮
    void analyzeSubFinish(const MEDIAINFO &media);          // 字幕分析完成

private:
    void Init();
    void InitLanguage();
    void UpUI();

    void UpMedia(const MEDIAINFO &mediainfo);                   // 更新媒体信息，包含字幕信息
    void upFormatList();                                        // 更新format
    void upSubtitles(const QStringList &subList);               // 更新字幕

    QString matchAudioforVideo();                               // 根据当前的video的format信息匹配audio
    void getformatId();                                         // 根据媒体的formatId获取视频流，如果只有一个音频流，则获取音频流

protected:
    void resizeEvent(QResizeEvent *);
private:
    TransEvenListWidget* _formatList;       // format列表
    URLSubtitlesWidget * _subtitleWidget;   // 字幕
    QLabel             * _mediaThumbnail;   // 缩略图
    QLabel             * _title;
    QLabel             * _titleinfo;
    QLabel             * _url;
    QLabel             * _urlinfo;
    QLabel             * _note;
    DYLabel            * _showAllBtn;
    DYLabel            * _doneBtn;
    double               _scaleRatio;
    bool                 _showAllFlag;
    QString              _show;
    QString              _hide;

    UrlproWidget       *_proWidget;         // 解析字幕的标记
    DYLabel            *_showSubBtn;        // 解析字幕按钮

    MEDIAINFO            _curMedia;
    STREAMINFO           _curFormat;

    QList<STREAMINFO>    _videoformat;
    QList<STREAMINFO>    _audioformat;

};

#endif // URLMEDIAWIDGET_H
