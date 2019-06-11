#include "listyoutubewidget.h"
#include "urlwidget.h"
#include "sqloperation.h"
#include "youtubedown.h"
#include "openfile.h"

#include <QHBoxLayout>
#include <QScrollBar>
#include <QResizeEvent>

#define WIDGETHEIGTH    100

ListYoutubeWidget::ListYoutubeWidget(QWidget *parent)
        : TransEventWidget(parent)
{
    _pListWidget        = new TransEventWidget(this);
    _pScrollBar         = new QScrollBar(Qt::Vertical, this);
    QHBoxLayout *hlay   = new QHBoxLayout(this);
    hlay->setContentsMargins(0,0,0,0);
    hlay->setSpacing(0);

    hlay->addWidget(_pListWidget);
    hlay->addWidget(_pScrollBar);
    this->setLayout(hlay);

    connect(_pScrollBar,SIGNAL(valueChanged(int)),SLOT(scroolBarChange(int)));
    _scaleRatio = 1.0;
    _init       = true;
}

ListYoutubeWidget::~ListYoutubeWidget()
{
    qDeleteAll(_wlist);
    _wlist.clear();
}

void ListYoutubeWidget::updateLanguage()
{
    for(URLWidget * urlWidget : _wlist)
    {
        urlWidget->updateLanguage();
    }
}

void ListYoutubeWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    for(int i=0;i<_wlist.size();++i) {
        _wlist.at(i)->updateUI(_scaleRatio);
        _wlist.at(i)->resize(this->width() - 4*_scaleRatio,WIDGETHEIGTH*_scaleRatio);
    }
    _pScrollBar->setStyleSheet(QString(// 设置垂直滚动条基本样式
                              "QScrollBar:vertical{width:%1px;padding-top:0px;padding-bottom:0px;"
                              "margin:0px,0px,0px,0px;background: rgb(22,22,23);}"
                              // 滚动条
                              "QScrollBar::handle:vertical{width:%1px;min-height:%2px;background:rgba(51,51,56,75%);}"
                              // 鼠标放到滚动条上
                              "QScrollBar::handle:vertical:hover{width:%1px;min-height:%2px;background:rgba(51,51,56,75%);}"
                              // 当滚动条滚动的时候，上面的部分和下面的部分
                              "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:rgba(0,0,0,10%);}"
                              // 设置上箭头
                              "QScrollBar::sub-line:vertical{height:0px;width:0px;subcontrol-position:top;}"
                              // 设置下箭头
                              "QScrollBar::add-line:vertical{height:0px;width:0px;subcontrol-position:bottom;}")
                       .arg(4  * _scaleRatio).arg(20  * _scaleRatio));
}

void ListYoutubeWidget::initYtbData(QList<MEDIAINFO> mediaList)
{
    _widgetData = mediaList;
    _pScrollBar->setVisible(false);
    _pScrollMaxValue= 0;
    initWidget(_init,true);
}

void ListYoutubeWidget::initWidget(bool init, bool initScrool)
{
    if(_widgetData.count() < 1) {
        clearWidget();
        return;
    }
    int ShowCount = this->height()/(WIDGETHEIGTH*_scaleRatio)+4;
    int max = (_widgetData.count()*(WIDGETHEIGTH*_scaleRatio) - this->height());
    if(max > 0) {
        _pScrollBar->setVisible(true);
        if(_pScrollMaxValue != max) {
            _pScrollBar->setRange(0,max); //  scrollbar的范围在_widgetShowCount的值发生变化时需要改变
            _pScrollMaxValue = max;
        }
    } else {
        _pScrollBar->setValue(0);
        _pScrollBar->setVisible(false);
    }

    if(init) {
        for(int i=0;i<ShowCount;i++) {
            URLWidget* urlWidget = new URLWidget(_pListWidget);
            urlWidget->resize(this->width()-4*_scaleRatio,WIDGETHEIGTH*_scaleRatio);
            connect(urlWidget,SIGNAL(showDetail(MEDIAINFO)),SIGNAL(showDetailSig(MEDIAINFO)));
            connect(urlWidget,SIGNAL(deleteClick(MEDIAINFO)),SLOT(delWebWidget(MEDIAINFO)));
            _wlist.append(urlWidget);
        }
    } else {
        if(ShowCount > _wlist.count()) {
            int addCount = ShowCount-_wlist.count();
            for(int i=0;i<addCount;i++) {
                URLWidget* urlWidget = new URLWidget(_pListWidget);
                urlWidget->resize(this->width()-4*_scaleRatio,WIDGETHEIGTH*_scaleRatio);
                connect(urlWidget,SIGNAL(showDetail(MEDIAINFO)),SIGNAL(showDetailSig(MEDIAINFO)));
                connect(urlWidget,SIGNAL(deleteClick(MEDIAINFO)),SLOT(delWebWidget(MEDIAINFO)));
                _wlist.append(urlWidget);
            }
        }
    }
    if(initScrool) {
        _pScrollBar->setValue(0);
        scroolBarChange(0);
    } else {
        scroolBarChange(_pScrollBar->value());
    }
    if(_init)
        _init = false;
}

void ListYoutubeWidget::upSelStream(MEDIAINFO mediaInfo)
{
    // 更新界面
    for(URLWidget * urlWidget : _wlist)
    {
        if(urlWidget->getDataSoure().webUrl == mediaInfo.webUrl) {
            urlWidget->setDataSoure(mediaInfo,true);
            break;
        }
    }
    // 更新数据
    for(int i=0;i<_widgetData.count();i++) {
        if(_widgetData.at(i).filepath == mediaInfo.filepath) {
            _widgetData.replace(i,mediaInfo);
            break;
        }
    }
}

void ListYoutubeWidget::updateWidget()
{
    if(_wlist.count() <= 0)
        return;
    int y = _wlist.at(0)->y();

    const int h = _wlist.at(0)->height();
    const int count = _wlist.length();

    for(int i=0; i<count;i++)
        _wlist.at(i)->move(0, y + i * h);
}

void ListYoutubeWidget::updateWidData(int dataPos)
{
    //设置每个子模块的数据
    QList<URLMEDIADATA> subList  = YtbDown->getSubTitleList();
    QList<DOWNURL>      downList = YtbDown->getDownList();
    for(URLWidget * urlWidget : _wlist)
    {
        if(urlWidget && dataPos < _widgetData.count())
        {
            int i = dataPos++;
            urlWidget->setDownState(false);
            // 这里看是否是第一次添加，如果是，则需要进行排序
            bool isFirst = false;
            for(int j=0;j<subList.size();j++) {
                if(subList.at(j).mediainfo.webUrl == _widgetData.at(i).webUrl) {
                    isFirst = true;
                    break;
                }
            }
            urlWidget->setDataSoure(_widgetData.at(i),isFirst);

            // 是否在下载列表中进行下载
            for(int j=0;j<downList.size();++j) {
                if(downList.at(j).downmedia.webUrl == _widgetData.at(i).webUrl) {
                    urlWidget->setDownState(true);
                    urlWidget->setDownProcess(downList.at(j).downProcess);
                    connect(YtbDown,SIGNAL(sendDownProcess(QString,double)),urlWidget,SLOT(upProcessInfo(QString,double)));
                    break;
                }
            }
            urlWidget->updateUI(_scaleRatio);
            urlWidget->setVisible(true);
        }
        else
        {
            urlWidget->setVisible(false);
        }
    }
}

void ListYoutubeWidget::clearWidget()
{
    for(int i=0;i<_wlist.count();i++) {
        _wlist.at(i)->setVisible(false);
    }
}

void ListYoutubeWidget::scroolBarChange(int pos)
{
    if(_wlist.count() < 1)
        return;
    const int h = _wlist.at(0)->height();

    int tmp_y = pos;
    int index = 0;

    if(pos > h)
    {
        //第一个的y值
        tmp_y = h + pos % h;

        //第一个的index
        index = (pos - h) / h;
    }

    //移动第一个的坐标
    _wlist.at(0)->move(0, -tmp_y);

    // 更新数据
    updateWidData(index);
    // 更新位置
    updateWidget();
}

void ListYoutubeWidget::addanalysisWeb(QString web)
{
    // 检查url是否存在
    /*
     * youtube的列表web是存放在filepath中，一个filepath对应有多个视频
     * 所以要删除所有filepath下的视频
     */
    bool isExits = false;
    MEDIAINFO media;
    media.webUrl = web;
    for(int i=0;i<_widgetData.count();i++) {
        if(_widgetData.at(i).filepath == web) {
            isExits = true;
            break;
        }
    }
    if(isExits) {
        // 弹出警告信息
        int res = OpenFileDialog->urlExitDialog();
        if(res == 1) {
            QList<MEDIAINFO> delList;
            delList.clear();
            for(int i=_widgetData.count()-1;i>=0;i--) {
                if(_widgetData.at(i).filepath == web) {
                    if(!_widgetData.at(i).title.isEmpty())
                        delList.append(_widgetData.at(i));
                    YtbDown->deletemedia(_widgetData.at(i));
                    _widgetData.removeAt(i);
                }
            }
            // 从数据库中删除
            if(delList.count() > 0)
                Sql->sqlDelUrlList(delList);
        } else {
            return;
        }
    }
    _widgetData.prepend(media);
    initWidget(false,true);
}

void ListYoutubeWidget::analyWebFinsh(QString web, QList<MEDIAINFO> mediaList)
{
    // 添加到数据库
    Sql->insertmediaListtosql(mediaList);

    for(int i=0;i<_widgetData.count();i++) {
        if(_widgetData.at(i).webUrl == web) {
            if(mediaList.count() > 1) {
                _widgetData.removeAt(i);
                for(int k=0;k<mediaList.count();k++) {
                    _widgetData.prepend(mediaList.at(k));
                }
            } else {
                _widgetData.replace(i,mediaList.at(0));
            }
            break;
        }
    }
    initWidget(false,false);
}

void ListYoutubeWidget::analyWebFail(QString web)
{
    for(int i=0;i<_widgetData.count();i++) {
        if(_widgetData.at(i).webUrl == web) {
            MEDIAINFO media;
            media.webUrl = web;
            media.isFavorites = true;
            _widgetData.replace(i,media);
            break;
        }
    }
    initWidget(false,false);
}

void ListYoutubeWidget::analyWebSubFinish(MEDIAINFO mediaInfo)
{
    for(int i=0;i<_widgetData.count();i++) {
        if(_widgetData.at(i).webUrl == mediaInfo.webUrl) {
            _widgetData.replace(i,mediaInfo);
            Sql->upUrlMediaSub(mediaInfo);
            break;
        }
    }
}

void ListYoutubeWidget::delWebWidget(MEDIAINFO mediaInfo)
{
    // 从数据库中删除
    if(!mediaInfo.title.isEmpty())
        Sql->sqlDelforUrlMedia(mediaInfo);

    YtbDown->deletemedia(mediaInfo);

    for(int i=0;i<_widgetData.count();i++) {
        if(_widgetData.at(i).webUrl == mediaInfo.webUrl) {
            _widgetData.removeAt(i);
            break;
        }
    }
    initWidget(false,false);
}

void ListYoutubeWidget::resizeEvent(QResizeEvent *e)
{
    initWidget(_init,false);
    for(int i=0;i<_wlist.count();i++){
        _wlist.at(i)->resize(this->width()-4*_scaleRatio,WIDGETHEIGTH*_scaleRatio);
    }
    e->accept();
}

void ListYoutubeWidget::wheelEvent(QWheelEvent *e)
{
    //下滚 负数-120  上滚 正数 120
    if(_pScrollBar->isVisible()) {
        const int delta_y = e->angleDelta().y() / 6;
        const int curValue = _pScrollBar->value();
        _pScrollBar->setValue(curValue - delta_y);
    }
}
