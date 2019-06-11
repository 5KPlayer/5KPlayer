#include "liveicomodewidget.h"
#include <QHBoxLayout>
#include <QScrollBar>
#include <QWheelEvent>
#include <QTimer>
#include "globalarg.h"
#include "playermainwidget.h"
LiveIcoModeWidget::LiveIcoModeWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    this->setAcceptDrops(true);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);

    QHBoxLayout *hlay = new QHBoxLayout(this);
    hlay->setContentsMargins(0,0,0,0);
    hlay->setSpacing(0);

    _singleTimer = new QTimer(this);
    _singleTimer->setInterval(200);
    _singleTimer->setSingleShot(true);

    connect(_singleTimer, SIGNAL(timeout()), SLOT(singleClicked()));

    _area = new TransEventScrollArea(this);
    hlay->addWidget(_area);
    this->setLayout(hlay);

    _area->verticalScrollBar()->setEnabled(false);
    _area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scrollW = new TransEventWidget(_area);
    _area->setWidget(_scrollW);

    _scaleRatio = Global->_screenRatio;
    _scrollW->setStyleSheet("background:transparent");
    _area->setStyleSheet(".TransEventScrollArea{border-style: solid; border-width: 2px; border-color: rgb(46,46,51); border-radius: 10px;}");
    //_scrollW->setStyleSheet(".TransEventWidget{border-style: solid; border-width: 2px; border-color: rgb(46,46,51); border-radius: 10px;}");
    //_area->setFrameShape(QFrame::NoFrame);
}

LiveIcoModeWidget::~LiveIcoModeWidget()
{
    qDeleteAll(_liveWlist);
    _liveWlist.clear();
}

void LiveIcoModeWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
}

void LiveIcoModeWidget::addLiveWidget(const QString &liveUrl, const QString &LiveImage, const bool &favetes)
{
    LiveIco* liveWidget = new LiveIco;
    connect(liveWidget,SIGNAL(updateLiveWidget(QString,bool)),SIGNAL(upDataLiveFav(QString,bool)));
    liveWidget->resize(120*_scaleRatio,120*_scaleRatio);
    liveWidget->setLiveData(liveUrl,LiveImage,favetes);
    liveWidget->setParent(_scrollW);
    _liveWlist.append(liveWidget);
}

void LiveIcoModeWidget::clearLiveWidget()
{
    qDeleteAll(_liveWlist);
    _liveWlist.clear();
    _scrollW->update();
}

void LiveIcoModeWidget::updateLiveWidget()
{
    if(_liveWlist.count() < 1)
        return;
    //子界面的 宽 高
    const int liveWidgetWidth   = _liveWlist.at(0)->width();
    const int liveWidgetHeight  = _liveWlist.at(0)->height();
    //滚动主界面的宽度
    const int w = _area->width() - 6*_scaleRatio;
    //每行放多少个
    int n = (w+10*_scaleRatio) / (liveWidgetWidth+15*_scaleRatio);
    if(n < 1) return;

    //横向间距
    int m = (w - liveWidgetWidth * n)/(n-1);
    //排列 子界面
    int row = 0;
    int crow = -1;
    for(int i=0; i<_liveWlist.count(); ++i)
    {
        row = i / n;
        if(row > crow && crow != -1)
            row++;
        const int column = i % n;
        int x = column *(m + liveWidgetWidth);
        int y = row * (10 + liveWidgetHeight);
        if(column == 0)
            x +=3*_scaleRatio;

        if(row == 0)
            y += 3*_scaleRatio;

        _liveWlist.at(i)->move(x, y);
        _liveWlist.at(i)->setVisible(true);
    }
    //防止点中最后一行
    if(row == crow)
        row++;


    //记住滚动位置
    int value = _area->verticalScrollBar()->value();
    _area->verticalScrollBar()->setValue(0);

    //重置滚动界面的 大小 及 坐标
    _scrollW->setGeometry(0, 0, w,
                          (row+1) * (10 + liveWidgetHeight));

    //还原滚动位置
    _area->verticalScrollBar()->setValue(value);
}

int LiveIcoModeWidget::getShowLiveCount()
{
    return 0;
}

void LiveIcoModeWidget::singleClicked()
{
    QWidget *w = _scrollW->childAt(_singleClickedPoint);
    if(w == NULL)
    {
        return;
    }
    LiveIco* chooseWidget;
    if(w->inherits("TransEventLabel"))
        chooseWidget = qobject_cast<LiveIco*>(w->parent());
    else
        chooseWidget = qobject_cast<LiveIco*>(w);
    if(_isReleased) {
        emit liveShow(chooseWidget->getLiveData());
    }
}

void LiveIcoModeWidget::resizeEvent(QResizeEvent *)
{
    for(int i=0;i<_liveWlist.count();i++) {
        _liveWlist.at(i)->resize(120*_scaleRatio,120*_scaleRatio);
    }
    updateLiveWidget();
}

void LiveIcoModeWidget::wheelEvent(QWheelEvent *e)
{
    const int deltaY = e->angleDelta().y() / 8;

//    if(e->angleDelta().y() == -120 && _liveWlist.size() >0){
//        const int subWidgetWidth = _liveWlist.at(0)->width();
//        //滚动主界面的宽度
//        const int w = _area->width() - 2;
//        //每行放多少个
//        int n = (w+10) / (subWidgetWidth+10);
//        widgetAddVideo(n);
//    }
    int value = _area->verticalScrollBar()->value() - deltaY;

    if(value < 0)
        value = 0;
    else if(value > _area->verticalScrollBar()->maximum())
        value = _area->verticalScrollBar()->maximum();

    _area->verticalScrollBar()->setValue(value);
}

void LiveIcoModeWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    _singleTimer->stop();//停止单击事件触发器
    const QPoint &p = QPoint(e->pos().x(), e->pos().y() + _area->verticalScrollBar()->value());

    QWidget *w = _scrollW->childAt(p);
    if(w == NULL) {
        e->ignore();
        return;
    }
    LiveIco* detailWidget;
    if(w->inherits("TransEventLabel"))
        detailWidget = qobject_cast<LiveIco*>(w->parent());
    else
        detailWidget = qobject_cast<LiveIco*>(w);
    emit livePlay(detailWidget->getLiveData());
}

void LiveIcoModeWidget::mousePressEvent(QMouseEvent *e)
{
    _singleTimer->start();
    _singleClickedPoint = QPoint(e->pos().x(), e->pos().y() + _area->verticalScrollBar()->value());
    _isReleased = false;
}

void LiveIcoModeWidget::mouseReleaseEvent(QMouseEvent *)
{
    _isReleased = true;
}
