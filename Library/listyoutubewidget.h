#ifndef LISTYOUTUBEWIDGET_H
#define LISTYOUTUBEWIDGET_H

#include <QStyleOption>
#include <QPainter>
#include "dystructu.h"
#include "transeventwidget.h"
#include "transeventscrollarea.h"
#include "updateinterface.h"
/*
 *
 * youtube的list列表
 */
class QScrollBar;
class URLWidget;
class ListYoutubeWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit ListYoutubeWidget(QWidget *parent = 0);
    ~ListYoutubeWidget();
    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void    initYtbData(QList<MEDIAINFO> mediaList);        // 初始化数据
    void    initWidget(bool init,bool initScrool);          // 初始化界面，init=false，滚动条的位置不变， init=true，滚动条位置到0
    void    upSelStream(MEDIAINFO mediaInfo);               // 更新显示的媒体信息

signals:
    void showDetailSig(MEDIAINFO);                          // 显示详细信息
private:
    void updateWidget();                                    // 更新widget位置
    void updateWidData(int dataPos);                        // 更新widget的数据
    void clearWidget();

public slots:
    void scroolBarChange(int pos);                                  // 滚动条事件
    void addanalysisWeb(QString web);                               // 添加分析信息
    void analyWebFinsh(QString web, QList<MEDIAINFO> mediaList);    // 分析成功
    void analyWebFail(QString web);                                 // 分析失败
    void analyWebSubFinish(MEDIAINFO mediaInfo);                    // 分析字幕结果
    void delWebWidget(MEDIAINFO mediaInfo);                         // 删除Web媒体

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void wheelEvent(QWheelEvent *);

private:
    QList<URLWidget *>  _wlist;             //子控件
    URLWidget *         _chooseWidget;      //鼠标点中的界面
    TransEventWidget*   _pListWidget;       // listWidget主界面
    QScrollBar*         _pScrollBar;        // 滚动条
    double              _scaleRatio;

    QList<MEDIAINFO>    _widgetData;       //widget中的媒体数据
    bool                _init;
    int                 _pScrollMaxValue;
};

#endif // LISTYOUTUBEWIDGET_H
