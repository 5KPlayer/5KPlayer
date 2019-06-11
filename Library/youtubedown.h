#ifndef YOUTUBEDOWN_H
#define YOUTUBEDOWN_H

#include <QObject>
#include <QProcess>
#include "singleton.h"
#include "dystructu.h"
#include "YtbTool.h"
/*!
    Youtube 的所有下载video和解析字幕的相关逻辑单例类
*/
#define YtbDown YoutubeDown::getInstance()

class YoutubeDown : public QObject,public Singleton<YoutubeDown>
{
    Q_OBJECT
    friend class  Singleton<YoutubeDown>;
    friend class QSharedPointer<YoutubeDown>;

public:
    QList<DOWNURL>      getDownList();              // 下载列表信息
    QList<URLMEDIADATA> getSubTitleList();          // 选中字幕列表信息
    QList<MEDIAINFO>    getSubAnalyzeList();        // 分析字幕列表信息
    void                upSubTitleList(const MEDIAINFO &media,const QStringList &subtitle);     // 更新选中字幕的信息

    void                deletemedia(const MEDIAINFO &media);            // 删除媒体
    void                startAnalyPath(QString path);                // 开始解析路径

    _StreamInfo getformatStream(const MEDIAINFO media);             // 根据媒体的formatId获取视频流，如果只有一个音频流，则获取音频流

signals:
    void sendDownProcess(QString objName,double pro);       // 下载进度消息
    void removeUrlWidget(MEDIAINFO);                        // 下载完成后，移除widgetItem的消息
    void sendanalyzeinfo(MEDIAINFO);                        // 字幕

    void analyFinsh(QString ,QList<MEDIAINFO>);             //分析成功
    void analyError(QString);                               //分析失败

public slots:
    void clickDownbtn(const MEDIAINFO &media);         // 点击下载
    void proStop(const MEDIAINFO &media);              // 取消下载
    void clickAnalyzeSub(const MEDIAINFO &media);      // 点击分析字幕

    void analypathFinish(QString url,QList<MEDIAINFO> mediaList);       // 分析结果
    void analySubFinish(const QString &url, const QStringList &subList);// 字幕结果

    void downProcessSlot(const QString &url,const double &process);     // 下载进度
    void downFinish(const QString &url,const QString &fileName);         // 下载完成
    void downFail(const QString &url);                                  // 下载失败

private:
    Q_DISABLE_COPY(YoutubeDown)
    explicit YoutubeDown(QObject *parent = 0);

    void addtoMediaList(QString path,QString item);  // 添加下载的文件到列表
    QString getWinFileName(QString title);          // 获取window环境下正确的文件名，在windows下。文件命名不能带有\/:*?"<>|


    QStringList getSelectSubList(QString url);      // 通过url获取选中的字幕信息
    DOWNURL     getDownMedia(QString url);          // 通过url获取下载媒体信息

    void            analyzeSubData(QString ObjName);
    QStringList analyzeSubtitles(QByteArray subByte); // 通过返回的byte信息获取字幕信息

private:
    QList<DOWNURL>          _downList;          // 下载列表
    QStringList             _analyMediaList;    // 媒体分析列表
    QList<MEDIAINFO>        _analySubList;      // 字幕分析列表
    QList<URLMEDIADATA>     _subtitleData;      // 字幕和选中的流信息列表,选中的流在libdata中的media的formatId也有保存，但不知道是否是初始化数据，如果是初始化数据，则根据设置的优先来选择选中流

    QString                 _ytbFilepath;       // ytb.exe路径
    YtbTool*                _ytbTool;
};

#endif // YOUTUBEDOWN_H
