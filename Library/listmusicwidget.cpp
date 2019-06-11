#include "listmusicwidget.h"
#include "listmusic.h"
#include "librarydata.h"
#include "globalarg.h"
#include "openfile.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QMimeData>
#include <QDrag>
#include <QResizeEvent>
#include <QFileInfo>
#include <QLabel>


#define MUSICHEIGTH     30

ListMusicWidget::ListMusicWidget(QWidget *parent) : QWidget(parent)
{
    _chooseWidget = NULL;
    _ctrl_isdown= false;
    _shift_isdown=false;
    _bSelectall  = false;
    _init           = true;
    _min_select = -1;
    _showItemName = "";
    LibData->selmedialist.clear();

    _singleTimer = new QTimer(this);
    _singleTimer->setInterval(200);
    _singleTimer->setSingleShot(true);

    connect(_singleTimer, SIGNAL(timeout()), SLOT(singleClicked()));


    this->setAcceptDrops(true);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);

    _pListWidget    = new TransEventWidget(this);
    _pScrollBar     = new QScrollBar(Qt::Vertical, this);

    QHBoxLayout *hlay = new QHBoxLayout(this);
    hlay->setContentsMargins(0,0,0,0);
    hlay->setSpacing(0);

    hlay->addWidget(_pListWidget);
    hlay->addWidget(_pScrollBar);
    this->setLayout(hlay);


    connect(_pScrollBar,SIGNAL(valueChanged(int)),SLOT(scroolBarChange(int)));

    _scaleRatio = 1.0;
}

ListMusicWidget::~ListMusicWidget()
{
    qDeleteAll(wlist);
    wlist.clear();
}

void ListMusicWidget::updateLanguage()
{

}

void ListMusicWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    for(int i=0;i<wlist.size();++i) {
        wlist.at(i)->updateUI(_scaleRatio);
        wlist.at(i)->resize(this->width() - 4*_scaleRatio,MUSICHEIGTH*_scaleRatio);
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
                                               .arg(4  * _scaleRatio).arg(20*_scaleRatio)
                                              );
}

void ListMusicWidget::initData(QList<MEDIAINFO> mediaList)
{
    _widgetData     = mediaList;
    _pScrollMaxValue= 0;
    _pScrollBar->setVisible(false);
    initWidget(_init,true);
}

void ListMusicWidget::upWidgetData(QList<MEDIAINFO> mediaList)
{
    _widgetData = mediaList;
    initWidget(false,false);
}

void ListMusicWidget::initWidget(bool init, bool initScrool)
{
    if(_widgetData.count() < 1) {
        clearWidget();
        return;
    }
    int widgetShowCount = this->height()/(MUSICHEIGTH*_scaleRatio)+4;
    int max = (_widgetData.count()*(MUSICHEIGTH*_scaleRatio) - this->height());
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
        for(int i=0;i<widgetShowCount;i++) {
            ListMusic* musicwidget = new ListMusic(_pListWidget);
            musicwidget->resize(this->width()-4*_scaleRatio,MUSICHEIGTH*_scaleRatio);
            wlist.append(musicwidget);
        }
    } else {
        if(widgetShowCount>wlist.count()) {
            int addCount = widgetShowCount - wlist.count();
            for(int i=0;i<addCount;i++) {
                ListMusic* musicwidget = new ListMusic(_pListWidget);
                musicwidget->resize(this->width()-4*_scaleRatio,MUSICHEIGTH*_scaleRatio);
                wlist.append(musicwidget);
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

void ListMusicWidget::clearWidget()
{
    LibData->selmedialist.clear();

    if(LibData->selectItemIsDrop()) {
        this->setAcceptDrops(false);
    } else {
        this->setAcceptDrops(true);
    }
    for(int i=0;i<wlist.count();i++) {
        wlist.at(i)->setVisible(false);
    }
}

void ListMusicWidget::updateSubWidget()
{
    if(wlist.count() <= 0)
        return;
    int y = wlist.at(0)->y();
    if(y > 0) {
        y = 0;
        _pScrollBar->setValue(0);
    }
    const int h = wlist.at(0)->height();
    const int count = wlist.length();

    for(int i=0; i<count;i++)
        wlist.at(i)->move(0, y + i * h);
}

void ListMusicWidget::clearSelWidget()
{
    if(LibData->selmedialist.size()>0){
        LibData->selmedialist.clear();
        for(int i=0;i<wlist.size();++i) {
            wlist.at(i)->setIsSelect(false,false);
        }
    }
    _bSelectall = false;
    _min_select = -1;
    emit delbtnEnable(LibData->selmedialist.size());
}

void ListMusicWidget::delSelWidget(bool ismove)
{
    LibData->delmedia(LibData->selmedialist,_bSelectall,false,ismove);
    clearSelWidget();
    _widgetData = LibData->getItemShowList();
    initWidget(false,false);
}

void ListMusicWidget::singleClicked()
{
    _isMoveFlag = false;
    QWidget *w = _pListWidget->childAt(_singleClickedPoint);
    if(w == NULL)
    {
        for(int i=0;i<wlist.size();++i) {
            wlist.at(i)->setIsSelect(false,false);
        }
        LibData->selmedialist.clear();
        _bSelectall = false;
        emit delbtnEnable(LibData->selmedialist.size());
        return;
    }


    if(w->inherits("TransEventLabel"))
        _chooseWidget = qobject_cast<ListMusic*>(w->parent());
    else
        _chooseWidget = qobject_cast<ListMusic*>(w);


    if(_chooseWidget == NULL/* ||
            _chooseWidget == _area->viewport()/* || _chooseWidget == _scrollW*/) {
        return;
    }

    bool flag = true;
    for(int i=0;i<LibData->selmedialist.count();i++) {
        if((QString::compare(_chooseWidget->getDataSoure().filepath ,LibData->selmedialist.at(i).filepath,Qt::CaseInsensitive) == 0) && !_isReleased) {
            flag = false;
            break;
        }
    }
    if(flag) {
        _min_select = -1;

        if(!_ctrl_isdown) {
            for(int i=0;i<wlist.size();++i) {
                if(wlist.at(i)->getIsSelect() && _min_select == -1) {
                    _min_select = i;
                    _max_select = i;
                }
                wlist.at(i)->setIsSelect(false,false);
            }
            LibData->selmedialist.clear();
        }

        if(_ctrl_isdown) {      // ctrl 按下时的处理
            _chooseWidget->setIsSelect(true,true);
            if(_chooseWidget->getIsSelect()) {
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
                wlist.at(min)->setIsSelect(true,true);
                LibData->selmedialist.append(wlist.at(min)->getDataSoure());
            }
        } else {
            _min_select = wlist.indexOf(_chooseWidget);
            _max_select = _min_select;
            _chooseWidget->setIsSelect(true, false);
            LibData->selmedialist.append(_chooseWidget->getDataSoure());
        }
        _bSelectall = false;
        emit delbtnEnable(LibData->selmedialist.size());
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

void ListMusicWidget::scroolBarChange(int pos)
{
    if(wlist.count() < 1)
        return;
    const int h = wlist.at(0)->height();

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
    wlist.at(0)->move(0, -tmp_y);

    //设置每个子模块的数据
    for(ListMusic * music : wlist)
    {
        if(index < _widgetData.count())
        {
            int i = index++;
            music->setDataSoure(_widgetData.at(i));
            music->updateUI(_scaleRatio);
            // 设置播放标记
            if (QString::compare(LibData->getplayinfo().filepath,_widgetData.at(i).filepath,Qt::CaseInsensitive) == 0)
                music->setplayFlag(true);
            else
                music->setplayFlag(false);

            // 设置选中标记
            music->setIsSelect(false,false);
            for(int j=0;j<LibData->selmedialist.size();++j) {
                if(QString::compare(LibData->selmedialist.at(j).filepath,_widgetData.at(i).filepath,Qt::CaseInsensitive) == 0) {
                    music->setIsSelect(true,false);
                    break;
                }
            }
            music->updateUI(_scaleRatio);
            music->setVisible(true);
        }
        else
        {
            music->setVisible(false);
        }
    }

    updateSubWidget();
}

void ListMusicWidget::mousePressEvent(QMouseEvent *e)
{
    _isReleased = false;
    _singleTimer->start();

    _singleClickedPoint = QPoint(e->pos().x(), e->pos().y());
}

void ListMusicWidget::mouseReleaseEvent(QMouseEvent *)
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

void ListMusicWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    _singleTimer->stop();//停止单击事件触发器

    const QPoint &p = QPoint(e->pos().x(), e->pos().y());

    QWidget *w = _pListWidget->childAt(p);
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
//        if(LibData->b_isConCPT) {
//            if(_detailWidget->getSupAirPlay()){
//                OpenFileDialog->setPlayType(OPenFile::CONNECTION);
//                OpenFileDialog->sendNetworkInfo(_detailWidget->getDataSoure().filepath);
//                LibData->setplaymedia(_detailWidget->getDataSoure());
//                for(int i=0;i<wlist.size();++i) {
//                    IcosVideo* video = wlist.at(i);
//                    if(video->getDataSoure().filepath == LibData->selmedialist.at(0).filepath)
//                        video->setplayFlag(true);
//                    else
//                        video->setplayFlag(false);
//                }
//            }
//            return;
//        }

        if(LibData->selmedialist.count() < 1)
            return;
        if(LibData->setplaymedia(LibData->selmedialist.at(0))){
            for(int i=0;i<wlist.size();++i) {
                ListMusic * video = wlist.at(i);
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

void ListMusicWidget::dragEnterEvent(QDragEnterEvent *e)
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

void ListMusicWidget::dragMoveEvent(QDragMoveEvent *e)
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

void ListMusicWidget::dropEvent(QDropEvent *e)
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
        QPoint p(e->pos().x(), e->pos().y());
        foreach (ListMusic *w, wlist)
        {
            QRect r = QRect(w->pos(), QSize(w->width() + 10, w->height() + 10));
            if(r.contains(p) )
            {
                findIndex = wlist.indexOf(w);
                break;
            }
        }

        int showCount = wlist.count();
        for(int i=0;i<wlist.count();i++) {
            if(wlist.at(i)->isHidden()) {
                showCount = i;
                break;
            }
        }

        if(findIndex == -1)
        {
            return;
            //findIndex = wlist.count() - 1;
        }
        if(findIndex >= showCount) {
            findIndex = showCount-1;
        }

        const bool b = findIndex == index;

        if(!b)//交换二者的位置
        {
            LibData->mediaListMove(wlist.at(index)->getDataSoure(),wlist.at(findIndex)->getDataSoure());
            _widgetData = LibData->getItemShowList();
            wlist.move(index, findIndex);

            LibData->selmedialist.clear();
            wlist.at(findIndex)->setIsSelect(false,false);
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

void ListMusicWidget::resizeEvent(QResizeEvent *e)
{
    initWidget(_init,false);
    for(int i=0;i<wlist.count();i++){
        wlist.at(i)->resize(this->width()-4*_scaleRatio,MUSICHEIGTH*_scaleRatio);
    }
    e->accept();
}

void ListMusicWidget::wheelEvent(QWheelEvent *e)
{
    //下滚 负数-120  上滚 正数 120
    if(_pScrollBar->isVisible()) {
        const int delta_y = e->angleDelta().y() / 6;
        const int curValue = _pScrollBar->value();
        _pScrollBar->setValue(curValue - delta_y);
    }
}

void ListMusicWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}

void ListMusicWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        if(event->modifiers()==Qt::ControlModifier) {
            LibData->selmedialist.clear();
            for(int i=0;i<wlist.size();++i) {
                wlist.at(i)->setIsSelect(true,false);
            }
            _bSelectall = true;
            LibData->selmedialist = LibData->getItemShowList();
            emit delbtnEnable(LibData->selmedialist.size());
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

void ListMusicWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->modifiers()!=Qt::ControlModifier) {
        _ctrl_isdown = false;
    }
    if(event->modifiers()!=Qt::ShiftModifier) {
        _shift_isdown = false;
    }
}

void ListMusicWidget::focusOutEvent(QFocusEvent *)
{
    _shift_isdown = false;
    _ctrl_isdown = false;
}
