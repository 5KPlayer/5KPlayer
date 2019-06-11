#include "dlnalistwidget.h"

#include <QHBoxLayout>
#include <QScrollBar>
#include <QWheelEvent>

#include "controlmanager.h"
#include "dlnadevinfowidget.h"
#include "globalarg.h"
#include <QDebug>
dlnaListWidget::dlnaListWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);

    _pListWidget        = new TransEventWidget(this);
    _pScrollBar         = new QScrollBar(Qt::Vertical, this);
    QHBoxLayout *hlay   = new QHBoxLayout(this);
    hlay->setContentsMargins(0,0,0,0);
    hlay->setSpacing(0);

    hlay->addWidget(_pListWidget);
    hlay->addWidget(_pScrollBar);
    this->setLayout(hlay);

    connect(_pScrollBar,SIGNAL(valueChanged(int)),SLOT(scroolBarChange(int)));

    _scaleRatio     = 1.0;
}

void dlnaListWidget::updateLanguage()
{
    for(int i=0;i<_devList.count();i++) {
        _devList.at(i)->updateLanguage();
    }
}

void dlnaListWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    for(int i=0;i<_devList.count();i++) {
        _devList.at(i)->resize(this->width() - 4*_scaleRatio,48*_scaleRatio);
        _devList.at(i)->updateUI(_scaleRatio);
    }
    upDevicePos();
    _pScrollBar->setStyleSheet(QString(Global->scrollBarSty)
                               .arg(4  * _scaleRatio).arg(20  * _scaleRatio));
}

void dlnaListWidget::scroolBarChange(int pos)
{
    if(_devList.count() < 1)
        return;
    const int h = _devList.at(0)->height();

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
    _devList.at(0)->move(0, -tmp_y);

    //设置每个子模块的数据
    QList<deviceInfo> tempList = CONTROL->getDeviceList();
    for(dlnaDevInfoWidget * dev : _devList)
    {
        if(index < tempList.count())
        {
            int i = index++;
            if(tempList.at(i).deviceType != -1) {
                dev->setDevInfo(tempList.at(i));
                dev->updateUI(_scaleRatio);
                dev->setVisible(true);
            }
        }
        else
        {
            dev->setVisible(false);
        }
    }

    upDevicePos();
}

void dlnaListWidget::upWidget(bool isAdd)
{
    for(int i=0;i<_devList.count();i++) {
        _devList.at(i)->setVisible(false);
    }
    _devList.clear();
    QList<deviceInfo> tempList = CONTROL->getDeviceList();
    for(int i=0;i<tempList.count();i++) {
        dlnaDevInfoWidget* dlnawidget = new dlnaDevInfoWidget(_pListWidget);
        dlnawidget->setDevInfo(tempList.at(i));
        dlnawidget->resize(this->width()-4*_scaleRatio,48*_scaleRatio);
        dlnawidget->setVisible(true);
        dlnawidget->updateUI(_scaleRatio);
        _devList.append(dlnawidget);
    }

    int max = (_devList.count()*(48*_scaleRatio) - this->height());
    if(max > 0) {
        _pScrollBar->setVisible(true);
        _pScrollBar->setRange(0,max); //  scrollbar的范围在_widgetShowCount的值发生变化时需要改变
    } else {
        _pScrollBar->setValue(0);
        _pScrollBar->setVisible(false);
    }

    if(_devList.count() <= 0)
        return;
    int y = _devList.at(0)->y();
    if(y > 0) {
        y = 0;
        _pScrollBar->setValue(0);
    }
    const int h = _devList.at(0)->height();
    const int count = _devList.length();

    for(int i=0; i<count;i++) {
        _devList.at(i)->move(0, y + i * h);
        _devList.at(i)->show();
    }
    update();
}

void dlnaListWidget::upDevicePos()
{
    if(_devList.count() <= 0)
        return;
    int y = _devList.at(0)->y();
    if(y > 0) {
        y = 0;
        _pScrollBar->setValue(0);
    }
    const int h = _devList.at(0)->height();
    const int count = _devList.length();

    for(int i=0; i<count;i++)
        _devList.at(i)->move(0, y + i * h);
}

void dlnaListWidget::resizeEvent(QResizeEvent *)
{
    upWidget(true);
}

void dlnaListWidget::wheelEvent(QWheelEvent *e)
{
    //下滚 负数-120  上滚 正数 120
    if(_pScrollBar->isVisible()) {
        const int delta_y = e->angleDelta().y() / 6;
        const int curValue = _pScrollBar->value();
        _pScrollBar->setValue(curValue - delta_y);
    }
}
