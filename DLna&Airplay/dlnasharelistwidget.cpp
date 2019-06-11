#include "dlnasharelistwidget.h"

#include "dlnamanager.h"
#include "openfile.h"
#include "dlnasharegroupwidget.h"
#include "dlnafiledetailwidget.h"
#include "globalarg.h"
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QMimeData>

#define DLNASIZE  110
#define FILE_TYPE "text/uri-list"
DLnaShareListWidget::DLnaShareListWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    init();
}

void DLnaShareListWidget::updateLanguage()
{
    for(int i=0;i<_dlnaFolders.count();i++) {
        _dlnaFolders.at(i)->updateLanguage();
    }
    for(int i=0;i<_dlnaVideos.count();i++) {
        _dlnaVideos.at(i)->updateLanguage();
    }
    for(int i=0;i<_dlnaMusics.count();i++) {
        _dlnaMusics.at(i)->updateLanguage();
    }
    _dlnaDetailWidget->updateLanguage();
    _folderGroup->updateLanguage();
    _videoGroup->updateLanguage();
    _musicGroup->updateLanguage();
}

void DLnaShareListWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _dlnaDetailWidget->updateUI(_scaleRatio);
    _folderGroup->updateUI(_scaleRatio);
    _videoGroup->updateUI(_scaleRatio);
    _musicGroup->updateUI(_scaleRatio);
    for(int i=0;i<_dlnaFolders.size();++i) {
        _dlnaFolders.at(i)->updateUI(_scaleRatio);
        _dlnaFolders.at(i)->resize(DLNASIZE*_scaleRatio,DLNASIZE*_scaleRatio);
    }
    for(int i=0;i<_dlnaVideos.size();++i) {
        _dlnaVideos.at(i)->updateUI(_scaleRatio);
        _dlnaVideos.at(i)->resize(DLNASIZE*_scaleRatio,DLNASIZE*_scaleRatio);
    }
    for(int i=0;i<_dlnaMusics.size();++i) {
        _dlnaMusics.at(i)->updateUI(_scaleRatio);
        _dlnaMusics.at(i)->resize(DLNASIZE*_scaleRatio,DLNASIZE*_scaleRatio);
    }
    _area->verticalScrollBar()->setStyleSheet(QString(Global->scrollBarSty)
                                               .arg(4  * _scaleRatio).arg(20 * _scaleRatio));
}

void DLnaShareListWidget::clearDLnaWidget()
{
    _dlnaDetailWidget->setVisible(false);
    _showRows   = 0;
    for(int i=0;i<_dlnaFolders.count();i++) {
        _dlnaFolders.at(i)->setVisible(false);
    }
    for(int i=0;i<_dlnaVideos.count();i++) {
        _dlnaVideos.at(i)->setVisible(false);
    }
    for(int i=0;i<_dlnaMusics.count();i++) {
        _dlnaMusics.at(i)->setVisible(false);
    }

    _dlnaFolders.clear();
    _dlnaVideos.clear();
    _dlnaMusics.clear();
    DLNA->_selDlnaData.clear();
    emit delbtnEnable(false);
}

void DLnaShareListWidget::addDLnaWidget(const dlnaLib &dlnaInfo)
{
    DLnaFileWidget* dLnaWidget = new DLnaFileWidget;
    dLnaWidget->resize(DLNASIZE*_scaleRatio,DLNASIZE*_scaleRatio);
    dLnaWidget->setWidgetData(dlnaInfo);
    dLnaWidget->updateUI(_scaleRatio);

    for(int i=0;i<DLNA->_selDlnaData.size();++i) {
        if(DLNA->_selDlnaData.at(i).filePath == dlnaInfo.filePath
           && DLNA->_selDlnaData.at(i).fileType == dlnaInfo.fileType){
            dLnaWidget->setWidgetSelect(true);
            break;
        }
    }

    dLnaWidget->setParent(_scrollW);
    // == 3是文件
    if(dlnaInfo.folderType == 3) {
        // 1 video 2 music
        if(dlnaInfo.fileType == 1) {
            _dlnaVideos.append(dLnaWidget);
        } else {
            _dlnaMusics.append(dLnaWidget);
        }
    } else {
        _dlnaFolders.append(dLnaWidget);
    }
}

bool DLnaShareListWidget::addDLnaWidget(int addRows)
{
    QList<dlnaLib> tempLib = DLNA->getDLnaData();
    QList<dlnaLib> folderList;
    QList<dlnaLib> videoList;
    QList<dlnaLib> musicList;

    folderList.clear();
    videoList.clear();
    musicList.clear();

    for(int i=0;i<tempLib.count();i++) {
        if(tempLib.at(i).folderType == 3) {
            if(tempLib.at(i).fileType == 1) {
                videoList.append(tempLib.at(i));
            } else {
                musicList.append(tempLib.at(i));
            }
        } else {
            folderList.append(tempLib.at(i));
        }
    }

    //子界面的 宽
    const int subWidgetWidth = DLNASIZE*_scaleRatio;
    //滚动主界面的宽度
    const int w = _area->width() - 4;

    //每行放多少个
    int n = (w+10) / (subWidgetWidth+10);
    if(n < 1) return false;

    //横向间距
    int k = n-1;
    if(k <=0)
        return false;

    // 算folder能放多少行
    int folderRow   = 0;
    int videoRow    = 0;
    int musicRow    = 0;
    if(folderList.count() > 0) {
        if(folderList.count() % n == 0) {
            folderRow = folderList.count() / n;
        } else {
            folderRow = folderList.count() / n + 1;
        }
    }

    if(videoList.count() > 0) {
        if(videoList.count() % n == 0) {
            videoRow = videoList.count() / n;
        } else {
            videoRow = videoList.count() / n + 1;
        }
    }

    if(musicList.count() > 0) {
        if(musicList.count() % n == 0) {
            musicRow = musicList.count() / n;
        } else {
            musicRow = musicList.count() / n + 1;
        }
    }

    if((_dlnaFolders.count() + _dlnaVideos.count() + _dlnaMusics.count())
        == (folderList.count() + videoList.count() + musicList.count()))
        return false;
    int addRow = 0;
    if(_dlnaFolders.count() < folderList.count()) {
        int col = 0;
        for(int i =_dlnaFolders.count();i<folderList.count();i++) {
            if(col == n) {
                col = 0;
                addRow++;
                if(addRow == addRows)
                    return true;
            }
            addDLnaWidget(folderList.at(i));
            col++;
        }
        addRow++;
        if(addRow < addRows) {
            addRow = folderRow;
        }
    }

    if(addRows > addRow) {
        if(_dlnaVideos.count() < videoList.count()) {
            int col = 0;
            for(int i=_dlnaVideos.count();i<videoList.count();i++) {
                if(col == n) {
                    col = 0;
                    addRow++;
                    if(addRow == addRows)
                        return true;
                }
                addDLnaWidget(videoList.at(i));
                col++;
            }
            addRow++;
            if(addRow < addRows) {
                addRow = folderRow + videoRow;
            }
        }
    }

    if(addRows > addRow) {
        if(_dlnaMusics.count() < musicList.count()) {
            int col = 0;
            for(int i=_dlnaMusics.count();i<musicList.count();i++) {
                if(col == n) {
                    col = 0;
                    addRow++;
                    if(addRow == addRows) {
                        break;
                    }
                }
                addDLnaWidget(musicList.at(i));
                col++;
            }
        }
    } else {
        return false;
    }
    return true;
}

void DLnaShareListWidget::delDLnaWidget(const dlnaLib &dlnaInfo)
{
    int remPos = -1;
    if(dlnaInfo.folderType == 3) {
        if(dlnaInfo.fileType == 1) {
            for(int i=0;i<_dlnaVideos.count();i++) {
                dlnaLib lib = _dlnaVideos.at(i)->getWidgetData();
                if(lib.fileName == dlnaInfo.fileName
                   && lib.filePath == dlnaInfo.filePath) {
                    remPos = i;
                    break;
                }
            }
        } else {
            for(int i=0;i<_dlnaMusics.count();i++) {
                dlnaLib lib = _dlnaMusics.at(i)->getWidgetData();
                if(lib.fileName == dlnaInfo.fileName
                   && lib.filePath == dlnaInfo.filePath) {
                    remPos = i;
                    break;
                }
            }
        }
    } else {
        for(int i=0;i<_dlnaFolders.count();i++) {
            dlnaLib lib = _dlnaFolders.at(i)->getWidgetData();
            if(lib.fileName == dlnaInfo.fileName
               && lib.filePath == dlnaInfo.filePath) {
                remPos = i;
                break;
            }
        }
    }

    if(remPos != -1) {
        if(dlnaInfo.folderType == 3) {
            if(dlnaInfo.fileType == 1) {
                _dlnaVideos.at(remPos)->setVisible(false);
                _dlnaVideos.removeAt(remPos);
            } else {
                _dlnaMusics.at(remPos)->setVisible(false);
                _dlnaMusics.removeAt(remPos);
            }
        } else {
            _dlnaFolders.at(remPos)->setVisible(false);
            _dlnaFolders.removeAt(remPos);
        }
        upDLnaSubWidget();
    }
}

int DLnaShareListWidget::getShowWidgetRows()
{
    return _showRows;
}

int DLnaShareListWidget::getShowWidgetCols()
{
    return _showCols;
}

void DLnaShareListWidget::upDLnaSubWidget()
{
    if(_dlnaFolders.count() < 0 &&
       _dlnaVideos.count() < 0 &&
       _dlnaMusics.count() < 0) {
        return;
    }

    bool isShowDetail = DLNA->getDLnaDetailVisible();
    _dlnaDetailWidget->setVisible(isShowDetail);

    //子界面的 宽 高
    const int subWidgetWidth = DLNASIZE*_scaleRatio;
    const int subWidgetHeight = DLNASIZE*_scaleRatio;
    //滚动主界面的宽度
    const int w = _area->width() - 4;

    //每行放多少个
    int n = (w+10) / (subWidgetWidth+10);
    if(n < 1) return;

    //横向间距
    int k = n-1;
    if(k <=0)
        return;
    int m = (w - subWidgetWidth * n)/(n-1);

    int row = 0;
    int rows = 0;
    int fixDetail = 0;

    if(_dlnaFolders.count() <= 0) {
        _folderGroup->setVisible(false);
    } else {
        _folderGroup->setVisible(true);
        _folderGroup->move(0,0);

        //排列 子界面
        int crow = -1;
        for(int i=0; i<_dlnaFolders.count(); ++i)
        {
            row = i / n;
            if(row > crow && crow != -1) {
                row++;
            }
            const int column = i % n;
            const int x = column *(m + subWidgetWidth);
            const int y = row * (10 + subWidgetHeight) + 30*_scaleRatio;
            _dlnaFolders.at(i)->move(x, y);
            _dlnaFolders.at(i)->setVisible(true);
        }

        //防止点中最后一行
        if(row == crow)
            row++;
        rows += row+1;
        fixDetail += 20*_scaleRatio;
    }


    if(_dlnaVideos.count() <= 0) {
        _videoGroup->setVisible(false);
    } else {
        _videoGroup->setVisible(true);
        int videoDetail = rows * (10 + subWidgetHeight) + fixDetail;
        _videoGroup->move(0,videoDetail);

        //排列 子界面
        int crow = -1;
        for(int i=0; i<_dlnaVideos.count(); ++i)
        {
            row = i / n;
            if(row > crow && crow != -1) {
                row++;
            }
            const int column = i % n;
            const int x = column *(m + subWidgetWidth);
            const int y = row * (10 + subWidgetHeight) + videoDetail + 30*_scaleRatio;
            _dlnaVideos.at(i)->move(x, y);
            _dlnaVideos.at(i)->setVisible(true);

            if(isShowDetail) {
                if(_dlnaVideos.at(i)->getWidgetData().filePath == DLNA->_selDlnaData.at(0).filePath) {
                    crow = row;
                    _dlnaDetailWidget->resize(w, 100*_scaleRatio);
                    _dlnaDetailWidget->setDetailInfo(DLNA->_selDlnaData.at(0));
                    _dlnaDetailWidget->move(0, y+subWidgetHeight+10);
                }
            }
        }
        //防止点中最后一行
        if(row == crow)
            row++;
        rows += row+1;
        fixDetail += 20*_scaleRatio;
    }


    if(_dlnaMusics.count() <= 0) {
        _musicGroup->setVisible(false);
    } else {
        _musicGroup->setVisible(true);
        int musicDetail = rows * (10 + subWidgetHeight) + fixDetail;
        _musicGroup->move(0,musicDetail);

        //排列 子界面
        int crow = -1;
        for(int i=0; i<_dlnaMusics.count(); ++i)
        {
            row = i / n;
            if(row > crow && crow != -1) {
                row++;
            }
            const int column = i % n;
            const int x = column *(m + subWidgetWidth);
            const int y = row * (10 + subWidgetHeight) + musicDetail + 30*_scaleRatio;
            _dlnaMusics.at(i)->move(x, y);
            _dlnaMusics.at(i)->setVisible(true);
            if(isShowDetail) {
                if(_dlnaMusics.at(i)->getWidgetData().filePath == DLNA->_selDlnaData.at(0).filePath) {
                    crow = row;
                    _dlnaDetailWidget->setDetailInfo(DLNA->_selDlnaData.at(0));
                    _dlnaDetailWidget->resize(w, 100*_scaleRatio);
                    _dlnaDetailWidget->move(0, y+subWidgetHeight+10);
                }
            }
        }
        //防止点中最后一行
        if(row == crow)
            row++;
        rows += row+1;
    }

    //记住滚动位置
    int value = _area->verticalScrollBar()->value();
    _area->verticalScrollBar()->setValue(0);

    //重置滚动界面的 大小 及 坐标
    _scrollW->setGeometry(0, 0, w,
                          (rows+1) * (10 + subWidgetHeight));

    //还原滚动位置
    _area->verticalScrollBar()->setValue(value);
}

void DLnaShareListWidget::init()
{
    this->setAcceptDrops(true);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);

    QHBoxLayout *hlay = new QHBoxLayout(this);
    hlay->setContentsMargins(0,0,0,0);
    hlay->setSpacing(0);

    _area = new TransEventScrollArea(this);
    hlay->addWidget(_area);
    this->setLayout(hlay);

    _area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scrollW = new TransEventWidget(_area);
    _area->setWidget(_scrollW);


    _folderGroup    = new DLnaShareGroupWidget(_scrollW,"OpenFile/folder");
    _videoGroup     = new DLnaShareGroupWidget(_scrollW,"OpenFile/video");
    _musicGroup     = new DLnaShareGroupWidget(_scrollW,"OpenFile/music");

    _dlnaDetailWidget   = new DLnaFileDetailWidget(_scrollW);
    _dlnaDetailWidget->setVisible(false);

    _folderGroup->setVisible(false);
    _videoGroup->setVisible(false);
    _musicGroup->setVisible(false);
    _singleTimer = new QTimer(this);
    _singleTimer->setInterval(200);
    _singleTimer->setSingleShot(true);

    connect(_singleTimer, SIGNAL(timeout()), SLOT(singleClicked()));
    connect(_area->verticalScrollBar(),SIGNAL(valueChanged(int)),SLOT(scroolBarChange(int)));

    _ctrl_isdown    = false;
    _shift_isdown   = false;
    _bSelectall     = false;
    _scaleRatio = 1.0;
    _showRows   = 0;
    _showCols   = 1;
    _area->setFrameShape(QFrame::NoFrame);
    updateUI(_scaleRatio);
}

void DLnaShareListWidget::upDLnaShareMaxRow()
{
    int maxRows = (this->height()+10)/(110*_scaleRatio+10)+1;
    int maxCols = (this->width()+10)/(110*_scaleRatio+10)+1;

    if(DLNA->getShareMaxRows() != maxRows) {
        DLNA->setShareMaxRows(maxRows,true);
    }
    if(DLNA->getShareMaxCols() != maxCols) {
        DLNA->setShareMaxCols(maxCols);
    }
}

void DLnaShareListWidget::singleClicked()
{
    _isMoveFlag = false;
    QWidget *w = _scrollW->childAt(_singleClickedPoint);
    if(w == NULL)
    {
        for(int i=0;i<_dlnaFolders.size();++i) {
            _dlnaFolders.at(i)->setWidgetSelect(false);
        }
        for(int i=0;i<_dlnaVideos.size();++i) {
            _dlnaVideos.at(i)->setWidgetSelect(false);
        }
        for(int i=0;i<_dlnaMusics.size();++i) {
            _dlnaMusics.at(i)->setWidgetSelect(false);
        }
        DLNA->_selDlnaData.clear();
        _bSelectall = false;
        emit delbtnEnable(DLNA->_selDlnaData.size());
        upDLnaSubWidget();
        return;
    }


    if(w->inherits("TransEventLabel")) {
        _dlnaWidget = qobject_cast<DLnaFileWidget*>(w->parent());
    } else if(w->inherits("TransEventIco")) {
        _dlnaWidget= qobject_cast<DLnaFileWidget*>(w->parent()->parent());
    } else {
        _dlnaWidget = qobject_cast<DLnaFileWidget*>(w);
    }


    if(_dlnaWidget == NULL ||
       _dlnaWidget == _area->viewport()) {
        return;
    }

    bool flag = true;
    for(int i=0;i<DLNA->_selDlnaData.count();i++) {
        if(DLNA->libInfoCompar(_dlnaWidget->getWidgetData(),DLNA->_selDlnaData.at(i)) && !_isReleased) {
            flag = false;
            break;
        }
    }

    if(flag) {

        if(!_ctrl_isdown) {
            for(int i=0;i<_dlnaFolders.size();++i) {
                _dlnaFolders.at(i)->setWidgetSelect(false);
            }
            for(int i=0;i<_dlnaVideos.size();++i) {
                _dlnaVideos.at(i)->setWidgetSelect(false);
            }
            for(int i=0;i<_dlnaMusics.size();++i) {
                _dlnaMusics.at(i)->setWidgetSelect(false);
            }
            DLNA->_selDlnaData.clear();
        }

        if(_ctrl_isdown) {      // ctrl 按下时的处理
            _dlnaWidget->setWidgetSelect(true,true);
            if(_dlnaWidget->getWidgetSelect()) {
                DLNA->_selDlnaData.append(_dlnaWidget->getWidgetData());
            } else {
                DLNA->removeSelForData(_dlnaWidget->getWidgetData());
            }
        } else {
            _dlnaWidget->setWidgetSelect(true, false);
            DLNA->_selDlnaData.append(_dlnaWidget->getWidgetData());
        }
        _bSelectall = false;
        emit delbtnEnable(DLNA->_selDlnaData.size());
        upDLnaSubWidget();
    }
}

void DLnaShareListWidget::scroolBarChange(int pos)
{
    if(pos > _area->verticalScrollBar()->maximum() - 180) {
        if(addDLnaWidget(1)) {
            upDLnaSubWidget();
        }
    }

}

void DLnaShareListWidget::mousePressEvent(QMouseEvent *e)
{
    _isReleased = false;
    _singleTimer->start();

    _singleClickedPoint = QPoint(e->pos().x(), e->pos().y() + _area->verticalScrollBar()->value());
}

void DLnaShareListWidget::mouseReleaseEvent(QMouseEvent *)
{
    _isReleased = true;
    if(_singleTimer->isActive())
    {
        _singleTimer->stop();
        singleClicked();
    }
}

void DLnaShareListWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    _singleTimer->stop();//停止单击事件触发器

    const QPoint &p = QPoint(e->pos().x(), e->pos().y() + _area->verticalScrollBar()->value());

    QWidget *w = _scrollW->childAt(p);
    if(w == NULL) {
        e->accept();
        return;
    }

    if(_ctrl_isdown || _shift_isdown)
        return;

    singleClicked();

    if (e->button() == Qt::LeftButton){
        if(_dlnaWidget) {
            dlnaLib dlnaInfo = _dlnaWidget->getWidgetData();
            QString path = dlnaInfo.filePath;
            QString name = dlnaInfo.fileName;
            switch (dlnaInfo.folderType) {
            case 2:
                DLNA->openVirFolder(name);
                break;
            case 1:
                DLNA->openFolderPath(path);
                break;
            case 3:
                DLNA->openFilePath(path);
                break;
            default:
                break;
            }
        }
    }
}

void DLnaShareListWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if(_ctrl_isdown || _shift_isdown) {
        e->ignore();
    } else if(e->mimeData()->hasFormat(FILE_TYPE)) {
        e->accept();
    } else {
        e->ignore();
    }
}

void DLnaShareListWidget::dragMoveEvent(QDragMoveEvent *e)
{
    if(_ctrl_isdown || _shift_isdown) {
        e->ignore();
    }
    else if(e->mimeData()->hasFormat(FILE_TYPE)) {
        e->accept();
    } else {
        e->ignore();
    }
}

void DLnaShareListWidget::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->hasFormat(FILE_TYPE)) {
        QList<QUrl> urls = e->mimeData()->urls();
        if(urls.isEmpty())
            return;

        qSort(urls);
        QStringList filepath;
        foreach(QUrl url, urls) {
            QFileInfo file(url.toLocalFile());
            if(file.isDir()) {
                if(!DLNA->pathIsExitsData(url.toLocalFile()))
                    DLNA->addOrDelFolder(url.toLocalFile(),true);
            } else if(file.isFile()) {
                if(!DLNA->pathIsExitsData(url.toLocalFile()))
                    filepath.append(url.toLocalFile());
            }
        }
        if(filepath.size() > 0) {
            OpenFileDialog->addDLnaDialog(filepath);
        }
    }
}

void DLnaShareListWidget::wheelEvent(QWheelEvent *e)
{
    const int deltaY = e->angleDelta().y() / 8;

    bool isAdd = false;
    if(e->angleDelta().y() == -120){
        isAdd = addDLnaWidget(1);
    }

    if(isAdd)
        upDLnaSubWidget();

    int value = _area->verticalScrollBar()->value() - deltaY;

    if(value < 0)
        value = 0;
    else if(value > _area->verticalScrollBar()->maximum())
        value = _area->verticalScrollBar()->maximum();

    _area->verticalScrollBar()->setValue(value);
}

void DLnaShareListWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        if(event->modifiers()==Qt::ControlModifier) {
            DLNA->_selDlnaData.clear();
            for(int i=0;i<_dlnaFolders.size();++i) {
                _dlnaFolders.at(i)->setWidgetSelect(true);
            }
            for(int i=0;i<_dlnaVideos.size();++i) {
                _dlnaVideos.at(i)->setWidgetSelect(true);
            }
            for(int i=0;i<_dlnaMusics.size();++i) {
                _dlnaMusics.at(i)->setWidgetSelect(true);
            }
            _bSelectall = true;
            DLNA->_selDlnaData = DLNA->getDLnaData();
            emit delbtnEnable(DLNA->_selDlnaData.size());
            upDLnaSubWidget();
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

void DLnaShareListWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->modifiers()!=Qt::ControlModifier) {
        _ctrl_isdown = false;
    }
    if(event->modifiers()!=Qt::ShiftModifier) {
        _shift_isdown = false;
    }
}

void DLnaShareListWidget::resizeEvent(QResizeEvent *e)
{
    _folderGroup->resize(_area->width() - 4*_scaleRatio,30*_scaleRatio);
    _videoGroup->resize(_area->width() - 4*_scaleRatio,30*_scaleRatio);
    _musicGroup->resize(_area->width() - 4*_scaleRatio,30*_scaleRatio);
    if((_area->width()+10) / (120*_scaleRatio) > 0)
        _showCols = (_area->width()+10) / (120*_scaleRatio);
    upDLnaShareMaxRow();
    e->accept();
}
