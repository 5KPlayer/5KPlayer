#include "radiocheckwidget.h"
#include <QLabel>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

RadioCheckWidget::RadioCheckWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    _leave = true;
    _curPage = 0;
    _lrSpace = 12;
    _tbSpace = 8;
    _labelWidth = 120;

    _animalGroup = new QParallelAnimationGroup(this);

    _radioHttpList << "http://bbcmedia.ic.llnwd.net/stream/bbcmedia_radio5live_mf_p?s=1428556760&e=1428571160&h=ca7f382f56fdebc5f3ea3c23e5e62a3a"
                   << "http://1661.live.streamtheworld.com:80/WJFKAMAAC_SC?TGT=iTunes&DIST=CBS&SRC=CBS"
                   << "http://stream.us.gslb.liquidcompass.net/KDUSAMAAC?ats=1"
                   << "http://streamingads.hotmixradio.fm/hotmixradio-80-64.aac"
                   << "http://streaming.radionomy.com/ABC-Lounge"
                   << "http://ice.stream.frequence3.net/frequence3-128.mp3"
                   << "http://streaming.radionomy.com/atomik_radio"
                   << "http://online-kissfm.tavrmedia.ua/KissFM"
                   << "http://streaming.radionomy.com/The-Buffalo"
                   << "http://streaming206.radionomy.com:8888/The-Music-Radio"
                   << "http://streaming.radionomy.com/BestRadio"
                   << "http://streaming.radionomy.com/heart-fm";

    QStringList radioPictures;
    radioPictures << ":/res/RadioPreset/bbc.jpg"         << ":/res/RadioPreset/cbssports.jpg"
                  << ":/res/RadioPreset/nbcsports.png"   << ":/res/RadioPreset/hotmix80.png"
                  << ":/res/RadioPreset/abclounge.jpeg"  << ":/res/RadioPreset/frequence.jpg"
                  << ":/res/RadioPreset/atomikradio.jpg" << ":/res/RadioPreset/kissfm.png"
                  << ":/res/RadioPreset/buffalo.jpg"     << ":/res/RadioPreset/musicradio.jpg"
                  << ":/res/RadioPreset/bestradio.jpg"   << ":/res/RadioPreset/heartfm.jpg";

    for(int i=0; i<_radioHttpList.count(); ++i)
    {
        QLabel *label = new QLabel(this);
        //label->setAlignment(Qt::AlignCenter);
        label->setGeometry(0, _tbSpace, _labelWidth, _labelWidth);
        label->setScaledContents(true);
        label->setPixmap(QPixmap(radioPictures.at(i)));

         QPropertyAnimation *animation = new QPropertyAnimation(label, "pos");
         animation->setDuration(300);

         _labelList << label;
         _animalList << animation;
         _animalGroup->addAnimation(animation);
    }

    const int &count = _labelList.count();
    _totalPage = count / 4;
    if(count % 4 != 0)
        _totalPage ++;

    this->resize(_labelWidth*4 + 5*_lrSpace, _labelWidth + _tbSpace*2);
    this->setStyleSheet(".RadioCheckWidget{border-style: solid; border-width: 2px; border-color: rgb(46,46,51); border-radius: 10px;}");
}

RadioCheckWidget::~RadioCheckWidget()
{
    qDeleteAll(_labelList);
    _labelList.clear();

    qDeleteAll(_animalList);
    _animalList.clear();
}

void RadioCheckWidget::recount()
{
    //当前
    moveAll(_curPage, 0);

    //左边
    int leftPage = _curPage - 1;
    if(leftPage < 0)
        leftPage = _totalPage-1;
    moveAll(leftPage, -(this->width()));

    //右边
    int rightPage = _curPage + 1;
    if(rightPage == _totalPage)
        rightPage = 0;
    moveAll(rightPage, this->width());

    //
    const int &count = _labelList.count();
    for(int i=0;i<count;++i) {
        int curPage = i / 4;
        if(curPage == _curPage || curPage == leftPage || curPage == rightPage)
            _labelList.at(i)->setVisible(true);
        else
            _labelList.at(i)->setVisible(false);
    }

    _animalGroup->start();
}

void RadioCheckWidget::updateUI(const qreal &scaleRatio)
{
    _lrSpace = 12 * scaleRatio;
    _tbSpace = 8  * scaleRatio;
    _labelWidth = 120 * scaleRatio;

    foreach (QLabel *l, _labelList) {
        l->resize(_labelWidth, _labelWidth);
        l->move(l->x() * scaleRatio, _tbSpace);
    }
    this->resize(_labelWidth*4 + 5*_lrSpace, _labelWidth + _tbSpace*2);
    this->setStyleSheet(QString(".RadioCheckWidget{border-style: solid; border-width: 2px; "
                        "border-color: rgb(46,46,51); border-radius: %1px;}").arg(_lrSpace));
    recount();
}

void RadioCheckWidget::mousePressEvent(QMouseEvent *e)
{
    _leave = false;
    _tmpX = _pressedX = e->pos().x();
}

void RadioCheckWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(_leave)
        return;

    if(!this->rect().contains(e->pos()))
    {
        _leave = true;
        release(e->pos().x());
        return;
    }
    const int &t = e->pos().x() - _tmpX;
    _tmpX = e->pos().x();

    foreach (QLabel *l, _labelList) {
        l->move(l->x() + t, _tbSpace);
    }
}

void RadioCheckWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(_leave)
        return;

    release(e->pos().x());
}

void RadioCheckWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}

void RadioCheckWidget::release(const int &curX)
{
    bool needUpdate = false;
    const int &t = curX - _pressedX;
    if(t > 0) //向右滑
    {
        needUpdate = true;
        _curPage--;
        if(_curPage < 0)
            _curPage = _totalPage-1;
    }
    else if(t < 0)//向左滑
    {
        needUpdate = true;
        _curPage++;
        if(_curPage == _totalPage)
            _curPage = 0;
    }
    else //点击
    {
        QWidget * w = this->childAt(curX, _labelWidth / 2);
        QLabel *label = qobject_cast<QLabel *>(w);
        if(label)
        {
            const int index = _labelList.indexOf(label);
            if(index != -1)
            {
                curClickedInfo(_radioHttpList.at(index));
            }
        }
    }
    if(needUpdate)
        recount();
}

void RadioCheckWidget::moveAll(const int &page, const int &t)
{
    int column;
    const int &count = _labelList.count();
    for(int i=0; i<4; ++i)
    {
        column = page * 4 + i;
        if(column >= count)
            break;

        _animalList.at(column)->setStartValue(_labelList.at(column)->pos());
        _animalList.at(column)->setEndValue(QPoint(t + _labelWidth*i + _lrSpace*(i+1), _tbSpace));
    }
}
