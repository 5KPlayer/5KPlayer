#include "liverecentplay.h"
#include "liveurl.h"
#include "sqloperation.h"
#include "globalarg.h"
#include <QLabel>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
LiveRecentPlay::LiveRecentPlay(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    Init();
}

LiveRecentPlay::~LiveRecentPlay()
{

}

void LiveRecentPlay::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
}

void LiveRecentPlay::setRecentPathList(const QStringList &recentList)
{
    _recentPath = recentList;
}

void LiveRecentPlay::addRecent(const QString &LivePaht)
{
    // 先查找是否存在
    bool LivePathExits = false;
    int LivePathPos = -1;
    for(int i=0;i<_recentPath.size();++i) {
        if(_recentPath.at(i) == LivePaht) {
            LivePathExits = true;
            LivePathPos = i;
            break;
        }
    }
    // 不存在，直接移除最后一个，存在则移除存在的，然后将新path的插入到列表前，
    if(!LivePathExits) {
        _recentPath.prepend(LivePaht);
        if(_recentPath.size()>5) {
            Sql->removeLiveUrl(_recentPath.at(5));
            _recentPath.removeAt(5);
        }
        Sql->addLiveurlToSql(LivePaht,1);
    } else {
        Sql->removeLiveUrl(_recentPath.at(LivePathPos));
        _recentPath.removeAt(LivePathPos);
        _recentPath.prepend(LivePaht);
        Sql->addLiveurlToSql(LivePaht,1);
    }
    upUI();
    emit upWidgetUI();
}

void LiveRecentPlay::recentShowBtnClick()
{
    _recentShow = !_recentShow;
    if(_recentShow) {
        _showRecentBtn->setPixmap(QPixmap(":/res/png/live_B.png"));
    } else {
        _showRecentBtn->setPixmap(QPixmap(":/res/png/live_R.png"));
    }
    Global->setLiveRecentShow(_recentShow);
    upUI();
    emit upWidgetUI();
}

void LiveRecentPlay::recentDelPath(const QString &LivePaht)
{
    for(int i=0;i<_recentPath.size();++i) {
        if(_recentPath.at(i) == LivePaht) {
            _recentPath.removeAt(i);
            break;
        }
    }
    Sql->removeLiveUrl(LivePaht);
    upUI();
    if(_recentPath.size()>0)
        emit upWidgetUI();
}

void LiveRecentPlay::Init()
{
    _scaleRatio = Global->_screenRatio;
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    _title          = new DYLabel(this);
    _showRecentBtn  = new DYLabel(this);

    _title->setPalette(p1);

    _title->raise();
    _title->setText(Lge->getLangageValue("Live/recent"));
//    _showRecentBtn->setAlignment(Qt::AlignCenter);
//    _showRecentBtn->setStyleSheet("color:rgb(255,255,255)");
    _showRecentBtn->setScaledContents(true);
    for(int i=0;i<5;++i) {
        _recentList[i] = new LiveUrl(this);
        connect(_recentList[i],SIGNAL(delLiveUrl(QString)),SLOT(recentDelPath(QString)));
        connect(_recentList[i],SIGNAL(playLiveUrl(QString)),SIGNAL(sendLivePlay(QString)));
    }
    this->setStyleSheet(".LiveRecentPlay{border-style: solid; border-width: 2px; border-color: rgb(46,46,51); border-radius: 10px;}");
    Sql->initLiveUrl(&_recentPath,1);
    _recentShow = Global->liveRecentShow();

    int HeightCount = 1;
    if(_recentShow) {
        _showRecentBtn->setPixmap(QPixmap(":/res/png/live_B.png"));
        HeightCount+=_recentPath.size();
    } else {
        _showRecentBtn->setPixmap(QPixmap(":/res/png/live_R.png"));
    }
    this->resize(this->width(),20*_scaleRatio*HeightCount);
    connect(_title,SIGNAL(clicked()),SLOT(recentShowBtnClick()));
    connect(_showRecentBtn,SIGNAL(clicked()),SLOT(recentShowBtnClick()));
}

void LiveRecentPlay::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    //_showRecentBtn->setFont(font);
    _title->setFont(font);

    double _space = 4 * _scaleRatio;
    _showRecentBtn->setGeometry(6*_scaleRatio,4*_scaleRatio,12*_scaleRatio,12*_scaleRatio);
    _title->setGeometry(20*_scaleRatio,2*_scaleRatio,150*_scaleRatio,16*_scaleRatio);
    int showRecentCount = _recentPath.size();
    double height = _showRecentBtn->height()+_space;
    for(int i=0;i<5;++i) {
        _recentList[i]->setGeometry(_space,18*_scaleRatio*(i+1),this->width(),18*_scaleRatio);
        if(i<showRecentCount && _recentShow && showRecentCount!=0) {
            _recentList[i]->setVisible(true);
            _recentList[i]->setUrlPath(_recentPath[i]);
            height += 18*_scaleRatio;
            _recentList[i]->updateUI(_scaleRatio);
        } else {
            _recentList[i]->setVisible(false);
        }
    }
    this->resize(this->width(),height+4*_scaleRatio);
}

void LiveRecentPlay::resizeEvent(QResizeEvent *)
{
    upUI();
}

void LiveRecentPlay::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}
