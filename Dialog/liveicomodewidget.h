#ifndef LIVEICOMODEWIDGET_H
#define LIVEICOMODEWIDGET_H

#include <QWidget>
#include "transeventwidget.h"
#include "transeventscrollarea.h"
#include "updateinterface.h"
#include "liveico.h"
class QScrollArea;
class LiveIcoModeWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit LiveIcoModeWidget(QWidget *parent = 0);
    ~LiveIcoModeWidget();

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void addLiveWidget(const QString &liveUrl,const QString &LiveImage, const bool &favetes);
    void clearLiveWidget();
    void updateLiveWidget();
    int  getShowLiveCount();

signals:
    void upDataLiveFav(const QString &url, const bool &fav);
    void livePlay(const QString &url);
    void liveShow(const QString &url);

public slots:
    void singleClicked();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e); // 鼠标双击事件
    virtual void mousePressEvent(QMouseEvent *);   //鼠标按下
    virtual void mouseReleaseEvent(QMouseEvent *);

private:
    QList<LiveIco *>        _liveWlist;     //子控件列表
    TransEventScrollArea*   _area;          //滚动区域
    TransEventWidget*       _scrollW;       //滚动主界面

    QTimer *_singleTimer;       //区分 单击事件 还是 双击事件
    QPoint _singleClickedPoint;
    bool _isReleased;

    double                  _scaleRatio;
};

#endif // LIVEICOMODEWIDGET_H
