#include "dlnafilewidget.h"

#include "globalarg.h"
#include "librarydata.h"
#include "dlnamanager.h"
#include "openfile.h"
#include "cmenu.h"
#include <QLabel>
#include <QSettings>
#include <QMouseEvent>
#include <QMenu>
DLnaFileWidget::DLnaFileWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    init();
}

void DLnaFileWidget::updateLanguage()
{
    QString title  = "";
    if(_libInfo.folderType == 3) {
        title  = _libInfo.fileName + "." + _libInfo.fileExt;
    } else {
        if(_libInfo.fileName == "yVideos" && _libInfo.folderType == 2) {
            title = QString("(YTB)").append(Lang("LibLeft/Videos"));
        } else if(_libInfo.fileName == "yMusic" && _libInfo.folderType == 2) {
            title = QString("(YTB)").append(Lang("LibLeft/Music"));
        } else {
            title = _libInfo.fileName;
        }
    }

    _title->setText(DLNA->getVirTextInfo(_title,title));
#ifdef Q_OS_WIN
    _title->setToolTip(title);
#endif
}

void DLnaFileWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void DLnaFileWidget::setWidgetData(dlnaLib data)
{
    _libInfo = data;
    if(_libInfo.folderType == 3) {
        QImage pix;

        if(DLNA->libInfoSupper(_libInfo)){
            _isSupport = true;
        } else {
            _isSupport = false;
        }

        if(_libInfo.fileType == 1) {
            if(_libInfo.fileshot.isEmpty()) {
                pix.load(":/res/png/videoshot.png");
            } else {
                QImage tmpMap;
                tmpMap.loadFromData(_libInfo.fileshot);
                int width   = this->width();
                int height  = this->height() - 30 * _scaleRatio;
                int displayH = 0;
                int displayW = 0;
                if(_libInfo.fileAsp != "" &&
                   _libInfo.fileAsp != "0:0"){
                    int wAsp,hAsp;
                    QStringList aspect = _libInfo.fileAsp.split(":");
                    if(aspect.count() >= 2) {
                        wAsp = aspect.at(0).toInt();
                        hAsp= aspect.at(1).toInt();
                    }
                    if(wAsp > hAsp) {
                        displayW = width;
                        displayH = width*hAsp/wAsp;
                    } else {
                        displayH = height;
                        displayW = height*wAsp/hAsp;
                    }
                } else {
                    displayH = height;
                    displayW = width;
                }

                if(DLNA->libInfoSupper(_libInfo)) {
                    pix = tmpMap.scaled(displayW,displayH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                } else {
                    pix = conImage(tmpMap.scaled(displayW,displayH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                }
            }
        } else {
            pix.load(":/res/png/musicshot.png");
        }
        _fileIco->setPixmap(QPixmap::fromImage(pix));
        _ico->hide();
    } else {
        _ico->load(QString(":/res/svg/Folder.svg"));
        _ico->show();
    }
    QString title  = "";
    if(_libInfo.folderType == 3) {
        title  = _libInfo.fileName + "." + _libInfo.fileExt;
    } else {
        if(_libInfo.fileName == "yVideos" && _libInfo.folderType == 2) {
            title = QString("(YTB)").append(Lang("LibLeft/Videos"));
        } else if(_libInfo.fileName == "yMusic" && _libInfo.folderType == 2) {
            title = QString("(YTB)").append(Lang("LibLeft/Music"));
        } else {
            title = _libInfo.fileName;
        }
    }

    _title->setText(DLNA->getVirTextInfo(_title,title));
#ifdef Q_OS_WIN
    _title->setToolTip(title);
#endif
}

dlnaLib DLnaFileWidget::getWidgetData()
{
    return _libInfo;
}

void DLnaFileWidget::setWidgetSelect(bool select, bool ctrl_down)
{
    if (ctrl_down)
        _isSelect = select ? !_isSelect:select;
    else
        _isSelect = select;

    if(_isSelect) {
        _fileIco->setStyleSheet("QLabel{border:2px solid rgb(0, 102, 204);}");
    } else {
        _fileIco->setStyleSheet("QLabel{border:2px solid rgb(17, 17, 18);}");
    }
    this->repaint();
}

bool DLnaFileWidget::getWidgetSelect()
{
    return _isSelect;
}

QImage DLnaFileWidget::conImage(QImage image)
{
    int height = image.height();
    int width = image.width();
    QImage ret(width, height, QImage::Format_Indexed8);
    ret.setColorCount(256);
    for(int i = 0; i < 256; i++)
    {
        ret.setColor(i, qRgb(i, i, i));
    }
    switch(image.format())
    {
    case QImage::Format_Indexed8:
        for(int i = 0; i < height; i ++)
        {
            const uchar *pSrc = (uchar *)image.constScanLine(i);
            uchar *pDest = (uchar *)ret.scanLine(i);
            memcpy(pDest, pSrc, width);
        }
        break;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        for(int i = 0; i < height; i ++)
        {
            const QRgb *pSrc = (QRgb *)image.constScanLine(i);
            uchar *pDest = (uchar *)ret.scanLine(i);

            for( int j = 0; j < width; j ++)
            {
                 pDest[j] = qGray(pSrc[j]);
            }
        }
        break;
    }
    return ret;
}

void DLnaFileWidget::init()
{
    _fileIco    = new TransEventLabel(this);
    _ico        = new TransEventIco(_fileIco);
    _title      = new QLabel(this);

    _title->setStyleSheet("Color:rgb(182,183,184)");
    _title->setAlignment(Qt::AlignCenter);

    _fileIco->setAlignment(Qt::AlignCenter);
    _fileIco->setStyleSheet("QLabel{border:2px solid rgb(17, 17, 18);}");

    _scaleRatio = 1.0;
    _isSupport  = true;
}

void DLnaFileWidget::upUI()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _title->setFont(font);
    _fileIco->setGeometry(0,0,this->width(),this->height()-23*_scaleRatio);
    _ico->setGeometry(2,2,_fileIco->width()-4,_fileIco->height()-4*_scaleRatio);
    _title->setGeometry(0,this->height()-23*_scaleRatio,this->width(),20*_scaleRatio);
}

bool DLnaFileWidget::fileIsExits(QString path)
{
    bool isExist = true;
    if (!LibData->checkMediaExits(path)){
        QString filelog = "\"" + path + "\" ";
        filelog.append(Lge->getLangageValue("RecentFile/info"));
        OpenFileDialog->openTooltipDialog(filelog);
        isExist = false;
    }
    return isExist;
}

void DLnaFileWidget::resizeEvent(QResizeEvent *event)
{
    upUI();
}

void DLnaFileWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton) {
        e->accept();
        //if(!_isSupport)
            //Menu->showRightMenu(cursor().pos());
    } else {
        e->ignore();
    }
}

void DLnaFileWidget::mouseReleaseEvent(QMouseEvent *e)
{
    e->ignore();
}

