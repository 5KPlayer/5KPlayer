#include "icosmodewidget.h"
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMimeData>
#include <QDrag>
#include <QTimer>
#include <QDataStream>
#include <QtDebug>
#include <QFileInfo>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QScrollBar>
#include "librarydata.h"
#include "openfile.h"
#include "playermainwidget.h"
#include "globalarg.h"
#include "ffmpegconvert.h"


#define ICOWIDWH 120

IcosModeWidget::IcosModeWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    _chooseWidget = NULL;
    _showDetailInfo = false;
    _ctrl_isdown= false;
    _shift_isdown=false;
    _bSelectall  = false;
    _min_select = -1;
    _showCount = 0;
    _showItemName = "";
    LibData->selmedialist.clear();

    _singleTimer = new QTimer(this);
    _singleTimer->setInterval(200);
    _singleTimer->setSingleShot(true);

    connect(_singleTimer, SIGNAL(timeout()), SLOT(singleClicked()));


    QHBoxLayout *hlay = new QHBoxLayout(this);
    hlay->setContentsMargins(0,0,0,0);
    hlay->setSpacing(0);

    _area = new TransEventScrollArea(this);
    hlay->addWidget(_area);
    this->setLayout(hlay);

    //_area->verticalScrollBar()->setEnabled(false);
    //_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scrollW = new TransEventWidget(_area);
    _area->setWidget(_scrollW);

    _detailWidget = new ListsVideo(_scrollW);

    connect(FFmpeg,SIGNAL(sendConvertProcess(QString,double)),_detailWidget,SLOT(upProcess(QString,double)));
    connect(_area->verticalScrollBar(),SIGNAL(valueChanged(int)),SLOT(scroolBarChange(int)));
    connect(LibData,SIGNAL(upWidgetAirPlay(bool)),_detailWidget,SLOT(upAirPlayCon(bool)));

    _scaleRatio = 1.0;
    _area->setFrameShape(QFrame::NoFrame);
}

IcosModeWidget::~IcosModeWidget()
{
    qDeleteAll(wlist);
    wlist.clear();
}

void IcosModeWidget::updateLanguage()
{
    if(_detailWidget)
        _detailWidget->updateLanguage();
    for(int i=0;i<wlist.size();++i) {
        wlist.at(i)->updateLanguage();
    }
}

void IcosModeWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _detailWidget->updateUI(_scaleRatio);
    for(int i=0;i<wlist.size();++i) {
        wlist.at(i)->updateUI(_scaleRatio);
        wlist.at(i)->resize(ICOWIDWH*_scaleRatio,ICOWIDWH*_scaleRatio);
    }
    _area->verticalScrollBar()->setStyleSheet(QString(// 设置垂直滚动条基本样式
                                                      "QScrollBar:vertical{width:%1px;padding-top:0px;padding-bottom:0px;"
                                                      "margin:0px,0px,0px,0px;background: rgb(22,22,23);}"
                                                      // 滚动条
                                                      "QScrollBar::handle:vertical{width:%1px;background:rgba(51,51,56,75%);}"
                                                      // 鼠标放到滚动条上
                                                      "QScrollBar::handle:vertical:hover{width:%1px;background:rgba(51,51,56,75%);}"
                                                      // 当滚动条滚动的时候，上面的部分和下面的部分
                                                      "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:rgba(0,0,0,10%);}"
                                                      // 设置上箭头
                                                      "QScrollBar::sub-line:vertical{height:0px;width:0px;subcontrol-position:top;}"
                                                      // 设置下箭头
                                                      "QScrollBar::add-line:vertical{height:0px;width:0px;subcontrol-position:bottom;}")
                                               .arg(4  * _scaleRatio)
                );
}

void IcosModeWidget::addWidget(const MEDIAINFO &videoInfo)
{
    IcosVideo* icoWidget = new IcosVideo;
    MEDIAINFO playinfo = LibData->getplayinfo();
    icoWidget->resize(ICOWIDWH*_scaleRatio,ICOWIDWH*_scaleRatio);
    icoWidget->setDataSoure(videoInfo);
    icoWidget->updateUI(_scaleRatio);

    if(playinfo.filepath == videoInfo.filepath)
        icoWidget->setplayFlag(true);
    for(int i=0;i<LibData->selmedialist.size();++i) {
        if(LibData->selmedialist.at(i).filepath == videoInfo.filepath){
            icoWidget->setSelect(true);
            break;
        }
    }
    icoWidget->setParent(_scrollW);
    wlist.append(icoWidget);
    _showCount++;
}

void IcosModeWidget::removeWidget(IcosVideo *w)
{
    if(wlist.removeOne(w))
        updateSubWidget();
}

void IcosModeWidget::removeWidget(int index)
{
    if(index < 0 || index >= wlist.count())
        return;

    wlist.removeAt(index);
    updateSubWidget();
}

void IcosModeWidget::clearWidget()
{
    _showDetailInfo = false;
    _showCount = 0;
    LibData->selmedialist.clear();
    _detailWidget->setVisible(_showDetailInfo);

    if(LibData->selectItemIsDrop()) {
        this->setAcceptDrops(false);
    } else {
        this->setAcceptDrops(true);
    }

    _area->verticalScrollBar()->setValue(0);
    _scrollW->setGeometry(0, 0,0,0);

    qDeleteAll(wlist);
    wlist.clear();

    _scrollW->update();
}

void IcosModeWidget::updateSubWidget()
{
    if(LibData->selmedialist.size() != 1)
        _showDetailInfo = false;
    else {
        _showDetailInfo = true;
        _detailWidget->setDataSoure(LibData->selmedialist.at(0));
        _detailWidget->repaint();
    }
    _detailWidget->setVisible(_showDetailInfo);

    if(wlist.count() < 1) {
        return;
    }

    //子界面的 宽 高
    const int subWidgetWidth = wlist.at(0)->width();
    const int subWidgetHeight = wlist.at(0)->height();
    //滚动主界面的宽度
    const int w = _area->width() - 4;
    //每行放多少个
    int n = (w+10) / (subWidgetWidth+10);
    if(n < 1) return;

    //横向间距
    int m = (w - subWidgetWidth * n)/(n-1);
    //排列 子界面
    int row = 0;
    int crow = -1;
    int fixDetail = 0;
    for(int i=0; i<wlist.count(); ++i)
    {
        row = i / n;
        if(row > crow && crow != -1) {
            row++;
            fixDetail = 20*_scaleRatio;//_detailWidget的高度比icowidget的高度小20
        }
        const int column = i % n;
        const int x = column *(m + subWidgetWidth);
        const int y = row * (10 + subWidgetHeight) - fixDetail;
        wlist.at(i)->move(x, y);
        wlist.at(i)->setVisible(true);

        if(/*_chooseWidget!=NULL &&*/ _showDetailInfo) {
            if(wlist.at(i)->getDataSoure().filepath == LibData->selmedialist.at(0).filepath) {
                crow = row;
                _detailWidget->resize(w, 100*_scaleRatio);
                _detailWidget->move(0, y+subWidgetHeight+10);
            }
        }
    }
    //防止点中最后一行
    if(row == crow)
        row++;


    //记住滚动位置
    int value = _area->verticalScrollBar()->value();
    _area->verticalScrollBar()->setValue(0);

    //重置滚动界面的 大小 及 坐标
    _scrollW->setGeometry(0, 0, w,
                          (row+1) * (10 + subWidgetHeight));

    //还原滚动位置
    _area->verticalScrollBar()->setValue(value);
}

IcosVideo *IcosModeWidget::getChooseWidget()
{
    return _chooseWidget;
}

void IcosModeWidget::upIcoWidget()
{
    _chooseWidget = NULL;
    //    emit delbtnEnable(false);
}

void IcosModeWidget::clearSelWidget()
{
    if(LibData->selmedialist.size()>0){
        LibData->selmedialist.clear();
        for(int i=0;i<wlist.size();++i) {
            wlist.at(i)->setSelect(false,false);
        }
    }
    _bSelectall = false;
    _min_select = -1;
    emit delbtnEnable(LibData->selmedialist.size());
}

void IcosModeWidget::delSelWidget(bool ismove)
{
    LibData->delmedia(LibData->selmedialist,_bSelectall,true,ismove);
    clearSelWidget();
    updateSubWidget();
}

void IcosModeWidget::widgetAddVideo(int count)
{
    bool isAdd = false;
    for(int i=0;i<count;++i) {
        MEDIAINFO media = LibData->getNextShow(_showCount);
        if(!media.title.isEmpty()) {
            addWidget(media);
            isAdd = true;
        }
    }
    if(isAdd)
        updateSubWidget();
}

void IcosModeWidget::singleClicked()
{
    _isMoveFlag = false;
    QWidget *w = _scrollW->childAt(_singleClickedPoint);
    if(w == NULL)
    {
        for(int i=0;i<wlist.size();++i) {
            wlist.at(i)->setSelect(false);
        }
        LibData->selmedialist.clear();
        _bSelectall = false;
        emit delbtnEnable(LibData->selmedialist.size());
        updateSubWidget();
        return;
    }


    if(w->inherits("TransEventLabel"))
        _chooseWidget = qobject_cast<IcosVideo*>(w->parent());
    else
        _chooseWidget = qobject_cast<IcosVideo*>(w);


    if(_chooseWidget == NULL ||
            _chooseWidget == _area->viewport()/* || _chooseWidget == _scrollW*/) {
        return;
    }

    bool flag = true;
    for(int i=0;i<LibData->selmedialist.count();i++) {
        if(_chooseWidget->getDataSoure().filepath == LibData->selmedialist.at(i).filepath && !_isReleased) {
            flag = false;
            break;
        }
    }
    if(flag) {
        _min_select = -1;

        if(!_ctrl_isdown) {
            for(int i=0;i<wlist.size();++i) {
                if(wlist.at(i)->isSelect() && _min_select == -1) {
                    _min_select = i;
                    _max_select = i;
                }
                wlist.at(i)->setSelect(false);
            }
            LibData->selmedialist.clear();
        }

        if(_ctrl_isdown) {      // ctrl 按下时的处理
            _chooseWidget->setSelect(true,true);
            if(_chooseWidget->isSelect()) {
                LibData->selmedialist.append(_chooseWidget->getDataSoure());
            } else {
                LibData->selmedialist = LibData->removeItemForList(LibData->selmedialist,_chooseWidget->getDataSoure());
            }
        } else if(!_ctrl_isdown && _shift_isdown) { // shift按下的处理
            int curselect = wlist.indexOf(_chooseWidget);
            int min,max;
            if(_min_select == -1) {
                min = curselect;
                max = curselect;
            } else {
                if(curselect>_min_select) {
                    min = _min_select;
                    max = curselect;
                } else {
                    min = curselect;
                    max = _max_select;
                }
            }

            _max_select = max;

            for(;min<=_max_select;min++) {
                wlist.at(min)->setSelect(true);
                LibData->selmedialist.append(wlist.at(min)->getDataSoure());
            }
        } else {
            _min_select = wlist.indexOf(_chooseWidget);
            _max_select = _min_select;
            _chooseWidget->setSelect(true, false);
            LibData->selmedialist.append(_chooseWidget->getDataSoure());
        }
        _bSelectall = false;
        emit delbtnEnable(LibData->selmedialist.size());
        updateSubWidget();
    }
    if(!_isReleased) {
        QByteArray itemData;
        QDataStream stream(&itemData, QIODevice::WriteOnly);
        stream << wlist.indexOf(_chooseWidget);

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(MIME_TYPE, itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap::fromImage(LibData->getSelPictures()));
        drag->exec(Qt::MoveAction);

        delete drag;
        drag = NULL;
    }
}

void IcosModeWidget::scroolBarChange(int pos)
{
    if(pos > _area->verticalScrollBar()->maximum() - 180) {
        const int subWidgetWidth = wlist.at(0)->width();
        //滚动主界面的宽度
        const int w = _area->width() - 2;
        //每行放多少个
        int n = (w+10) / (subWidgetWidth+10);
        widgetAddVideo(n);
    }
}

void IcosModeWidget::mousePressEvent(QMouseEvent *e)
{
    _isReleased = false;
    _singleTimer->start();

    _singleClickedPoint = QPoint(e->pos().x(), e->pos().y() + _area->verticalScrollBar()->value());
}

void IcosModeWidget::mouseReleaseEvent(QMouseEvent *)
{
    _isReleased = true;
    if(_singleTimer->isActive())
    {
        _singleTimer->stop();
        singleClicked();
    }
    if(_isMoveFlag) {
        _isMoveFlag = false;
        return;
    }
}

void IcosModeWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    _singleTimer->stop();//停止单击事件触发器

    const QPoint &p = QPoint(e->pos().x(), e->pos().y() + _area->verticalScrollBar()->value());

    QWidget *w = _scrollW->childAt(p);
    if(w == NULL) {
        e->accept();
        return;
    }

    //未用到
//    ListsVideo *detailWidget = NULL;
//    if(w->inherits("TransEventLabel"))
//        detailWidget = qobject_cast<ListsVideo*>(w->parent());
//    else
//        detailWidget = qobject_cast<ListsVideo*>(w);

//    if(_detailWidget ==  detailWidget) {
//        e->accept();
//        return;
//    }

    if(_ctrl_isdown || _shift_isdown)
        return;

    singleClicked();

    if (e->button() == Qt::LeftButton){
        // 检查是否连接到qt电脑，如果有，则需要判断播放的媒体是否支持airplay
        if(LibData->b_isConCPT) {
            if(_detailWidget->getSupAirPlay()){
                OpenFileDialog->setPlayType(OPenFile::CONNECTION);
                OpenFileDialog->sendNetworkInfo(_detailWidget->getDataSoure().filepath);
                LibData->setplaymedia(_detailWidget->getDataSoure());
                for(int i=0;i<wlist.size();++i) {
                    IcosVideo* video = wlist.at(i);
                    if(video->getDataSoure().filepath == LibData->selmedialist.at(0).filepath)
                        video->setplayFlag(true);
                    else
                        video->setplayFlag(false);
                }
            }
            return;
        }

        if(LibData->setplaymedia(LibData->selmedialist.at(0))){
            for(int i=0;i<wlist.size();++i) {
                IcosVideo* video = wlist.at(i);
                if(QString::compare(video->getDataSoure().filepath,LibData->getplayinfo().filepath,Qt::CaseInsensitive) == 0)
                    video->setplayFlag(true);
                else
                    video->setplayFlag(false);
            }
            LibData->b_doubleClickPlay = true;
            OpenFileDialog->openFile(LibData->getplayinfo().filepath);
            LibData->setplayItem("");
        } else {
            int res = OpenFileDialog->mediaExitDialog();
            switch (res) {
            case 1:
                LibData->cleanUpList();
                clearSelWidget();
                updateSubWidget();
                break;
            case 2 :
                delSelWidget(false);
                break;
            }
        }
    }
}

void IcosModeWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if(_ctrl_isdown || _shift_isdown) {
        e->ignore();
    } else if(e->mimeData()->hasFormat(MIME_TYPE)) {
        e->accept();
    } else if(e->mimeData()->hasFormat(MEDIA_TYPE)) {
        e->accept();
    } else {
        e->ignore();
    }
}

void IcosModeWidget::dragMoveEvent(QDragMoveEvent *e)
{
    _isMoveFlag = true;
    if(_ctrl_isdown || _shift_isdown) {
        e->ignore();
    }
    else if(e->mimeData()->hasFormat(MIME_TYPE)) {
        e->accept();
    } else if(e->mimeData()->hasFormat(MEDIA_TYPE)) {
        e->accept();
    } else {
        e->ignore();
    }
}

void IcosModeWidget::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->hasFormat(MIME_TYPE))
    {
        if(LibData->selmedialist.size() > 1)
            return;
        QByteArray itemData = e->mimeData()->data(MIME_TYPE);
        QDataStream stream(&itemData, QIODevice::ReadOnly);
        int index;
        stream >> index;

        //计算放下的左边的索引
        int findIndex = -1;
        QPoint p(e->pos().x(), e->pos().y() + _area->verticalScrollBar()->value());
        foreach (IcosVideo *w, wlist)
        {
            QRect r = QRect(w->pos(), QSize(w->width() + 10, w->height() + 10));
            if(r.contains(p) )
            {
                findIndex = wlist.indexOf(w);
                break;
            }
        }

        if(findIndex == -1)
        {
            return;
            //findIndex = wlist.count() - 1;
        }

        const bool b = findIndex == index;

        if(!b)//交换二者的位置
        {
            LibData->mediaListMove(wlist.at(index)->getDataSoure(),wlist.at(findIndex)->getDataSoure());
            wlist.move(index, findIndex);

            LibData->selmedialist.clear();
            wlist.at(findIndex)->setSelect(false,false);
            emit delbtnEnable(LibData->selmedialist.size());
        }
        updateSubWidget();
    }
    else if(e->mimeData()->hasFormat(MEDIA_TYPE)) {
        QList<QUrl> urls = e->mimeData()->urls();
        if(urls.isEmpty())
            return;

        qSort(urls);
        QStringList filepath;
        foreach(QUrl url, urls) {

            QFileInfo file(url.toLocalFile());
            if(file.isDir()) {
                LibData->setRootPath(url.toLocalFile());
                filepath.append(LibData->getFilePathonFolder(url.toLocalFile()));
            } else if(file.isFile()) {
                filepath.append(url.toLocalFile());
            }

        }

        LibData->clearFileInItemName(&filepath);

        OpenFileDialog->addMediaDialog(filepath);
    }
}

void IcosModeWidget::resizeEvent(QResizeEvent *e)
{
    updateSubWidget();
    e->accept();
}

void IcosModeWidget::wheelEvent(QWheelEvent *e)
{
    const int deltaY = e->angleDelta().y() / 8;

    if(e->angleDelta().y() == -120 && wlist.size() >0){
        const int subWidgetWidth = wlist.at(0)->width();
        //滚动主界面的宽度
        const int w = _area->width() - 2;
        //每行放多少个
        int n = (w+10) / (subWidgetWidth+10);
        widgetAddVideo(n);
    }
    int value = _area->verticalScrollBar()->value() - deltaY;

    if(value < 0)
        value = 0;
    else if(value > _area->verticalScrollBar()->maximum())
        value = _area->verticalScrollBar()->maximum();

    _area->verticalScrollBar()->setValue(value);
}

void IcosModeWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
    //this->setCursor(QCursor(Qt::ArrowCursor));
}

void IcosModeWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        if(event->modifiers()==Qt::ControlModifier) {
            LibData->selmedialist.clear();
            for(int i=0;i<wlist.size();++i) {
                wlist.at(i)->setSelect(true);
            }
            _bSelectall = true;
            LibData->selmedialist = LibData->getItemShowList();
            emit delbtnEnable(LibData->selmedialist.size());
            updateSubWidget();
        }
        break;
    default:
        break;
    }
    switch (event->modifiers()) {
    case Qt::ControlModifier:
        _ctrl_isdown = true;
        break;
    case Qt::ShiftModifier:
        _shift_isdown = true;
        break;
    default:
        break;
    }
}

void IcosModeWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->modifiers()!=Qt::ControlModifier) {
        _ctrl_isdown = false;
    }
    if(event->modifiers()!=Qt::ShiftModifier) {
        _shift_isdown = false;
    }
}

void IcosModeWidget::focusOutEvent(QFocusEvent *)
{
    _ctrl_isdown = false;
    _shift_isdown = false;
}

