#include "dlnasharewidget.h"
#include "dlnasharelistwidget.h"
#include "dlnamanager.h"
#include "svgbutton.h"
#include "librarydata.h"
#include "openfile.h"
#include "configure.h"

#include <QLabel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

#define HEADBTNHEIGHT  31
DLnaShareWidget::DLnaShareWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    init();
}

void DLnaShareWidget::updateLanguage()
{
    _shareWidget->updateLanguage();
}

void DLnaShareWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _addFolderBtn->updateUI(_scaleRatio);
    _addFileBtn->updateUI(_scaleRatio);
    _delBtn->updateUI(_scaleRatio);
    _shareWidget->updateUI(_scaleRatio);
    upUI();
}

void DLnaShareWidget::upShareWidgetSlot(bool isUp)
{
    if(isUp) {
        _shareWidget->clearDLnaWidget();
        _shareWidget->addDLnaWidget(DLNA->getShareMaxRows());
        _shareWidget->upDLnaSubWidget();
        DLNA->_selDlnaData.clear();
        upDelBtnEnable(DLNA->_selDlnaData.count());
    } else {
        if(DLNA->getShareMaxRows() > _shareWidget->getShowWidgetRows()) {
            _shareWidget->addDLnaWidget(1);
            _shareWidget->upDLnaSubWidget();
            DLNA->_selDlnaData.clear();
        }
    }
}

void DLnaShareWidget::delSelDLnaLib()
{
    _delBtn->setEnabled(false);
    DLNA->removeDataInSel();
}

void DLnaShareWidget::upDelBtnEnable(bool state)
{
    _delBtn->setEnabled(state);
}

void DLnaShareWidget::addFolderBtnClick()
{
    QString path = QFileDialog::getExistingDirectory(NULL,Lge->getLangageValue("OpenFile/opfolder"),"",QFileDialog::ReadOnly);
    if(!DLNA->pathIsExitsData(path))
        DLNA->addOrDelFolder(path,true);
}

void DLnaShareWidget::addFileBtnClick()
{
    QString openPath;
    if(_prePath == "") {
            openPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    } else {
        openPath = _prePath;
    }

    QStringList fileList;

    fileList = OpenFileDialog->getFileNames(Lge->getLangageValue("OpenFile/opmedia"),
                                            openPath,
                                            Lge->getLangageValue("OpenFile/allfile") + ";;" +
                                            Lge->getLangageValue("OpenFile/videoFilt") + "(" +
                                            Config->getUserVideoFormat() + ");;" +
                                            Lge->getLangageValue("OpenFile/musicFilt") + "(" +
                                            Config->getUserAudioFormat() + ");");

    if (fileList.isEmpty())
        return;

    QString file = fileList.at(0);
    QFileInfo fileinfo(file);
    _prePath = fileinfo.path();

    QStringList virPaths = DLNA->pathsIsExitsData(fileList);
    if(virPaths.count() > 0)
        OpenFileDialog->addDLnaDialog(virPaths);
}

void DLnaShareWidget::resizeEvent(QResizeEvent *event)
{
    upUI();
    _shareWidget->upDLnaSubWidget();
}

void DLnaShareWidget::init()
{
    _addFolderBtn       = new SvgButton(this);
    _addFileBtn         = new SvgButton(this);
    _delBtn             = new SvgButton(this);
    _line = new QLabel(this);
    _line->setStyleSheet("background-color: rgb(29, 29, 30);");

    _prePath        = "";
    _scaleRatio     = 1.0;
    _shareWidgetinit= false;
    _addFolderBtn->initSize(HEADBTNHEIGHT*_scaleRatio,HEADBTNHEIGHT*_scaleRatio);
    _addFileBtn->initSize(HEADBTNHEIGHT*_scaleRatio,HEADBTNHEIGHT*_scaleRatio);
    _delBtn->initSize(HEADBTNHEIGHT*_scaleRatio,HEADBTNHEIGHT*_scaleRatio);


    _addFileBtn->setImagePrefixName("add");
    _addFolderBtn->setImagePrefixName("folder");
    _delBtn->setImagePrefixName("delete");

    _delBtn->setEnabled(false);
    _shareWidget = new DLnaShareListWidget(this);

    connect(_addFolderBtn,SIGNAL(clicked()),SLOT(addFolderBtnClick()));
    connect(_addFileBtn,SIGNAL(clicked()),SLOT(addFileBtnClick()));
    connect(_delBtn,SIGNAL(clicked()),SLOT(delSelDLnaLib()));
    connect(_shareWidget,SIGNAL(delbtnEnable(bool)),SLOT(upDelBtnEnable(bool)));
    connect(DLNA,SIGNAL(upShareWidget(bool)),SLOT(upShareWidgetSlot(bool)));
}

void DLnaShareWidget::upUI()
{
    _addFolderBtn->move(HEADBTNHEIGHT*_scaleRatio,0);
    _addFileBtn->move(_addFolderBtn->x() + HEADBTNHEIGHT*_scaleRatio,0);
    _delBtn->move(_addFileBtn->x()+HEADBTNHEIGHT*_scaleRatio,0);
    _line->setGeometry(0,(HEADBTNHEIGHT+1)*_scaleRatio,this->width(),1*_scaleRatio);
    _shareWidget->setGeometry(0,(HEADBTNHEIGHT+3)*_scaleRatio,this->width()-4*_scaleRatio,this->height()-(HEADBTNHEIGHT+2)*_scaleRatio);
}
