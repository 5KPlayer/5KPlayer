#include "dlnafiledetailwidget.h"

#include "dylabel.h"
#include "globalarg.h"
#include "dlnamanager.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QProcess>
DLnaFileDetailWidget::DLnaFileDetailWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    init();
}

void DLnaFileDetailWidget::updateLanguage()
{
    upDetailInfo();
}

void DLnaFileDetailWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void DLnaFileDetailWidget::setDetailInfo(dlnaLib dlnaInfo)
{
    _dlnaInfo = dlnaInfo;
    upDetailInfo();
}

void DLnaFileDetailWidget::init()
{
    _videoIco      = new TransEventLabel(this);
    _titleName     = new TransEventLabel(this);

    _icoLine       = new TransEventLabel(this);
    _icoLine2      = new TransEventLabel(this);

    _formatLab     = new TransEventLabel(this);
    _pathLab       = new TransEventLabel(this);

    _convertbtn   = new DYLabel(this,true);

    _videoIco->setAlignment(Qt::AlignCenter);
    _titleName->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _formatLab->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _pathLab->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    _convertbtn->setAlignment(Qt::AlignCenter);
    _convertbtn->setMouseHead(true);

    _titleName->setStyleSheet("color:rgb(182,183,184)");
    _formatLab->setStyleSheet("color:rgb(115,117,128)");
    _pathLab->setStyleSheet("color:rgb(115,117,128)");
    _icoLine->setStyleSheet("background-color: rgb(31, 31, 35);");
    _icoLine2->setStyleSheet("background-color: rgb(46, 46, 51);");
    this->setStyleSheet("background-color: rgb(39, 39, 44);");
    _scaleRatio = 1.0;

    connect(_convertbtn,SIGNAL(clicked()),SLOT(clickConvertBtn()));
}

void DLnaFileDetailWidget::upUI()
{
    int xPos = 4*_scaleRatio;
    _videoIco->setGeometry(xPos,15*_scaleRatio,100*_scaleRatio,80*_scaleRatio);
    _titleName->setGeometry(130*_scaleRatio,2*_scaleRatio,this->width() - 140*_scaleRatio,40*_scaleRatio);
    _icoLine->setGeometry(110*_scaleRatio,45*_scaleRatio,this->width() - 120*_scaleRatio,1*_scaleRatio);
    _icoLine2->setGeometry(110*_scaleRatio,46*_scaleRatio,this->width() - 120*_scaleRatio,1*_scaleRatio);
    int fmtWidth;
    if(this->width() > 580*_scaleRatio) {
        fmtWidth = (580-100)*_scaleRatio - 130*_scaleRatio;
    } else {
        fmtWidth = this->width() - 240*_scaleRatio;
    }
    _formatLab->setGeometry(130*_scaleRatio,52*_scaleRatio,fmtWidth,20*_scaleRatio);
    _convertbtn->setGeometry(_formatLab->x()+_formatLab->width(),58*_scaleRatio,100*_scaleRatio,20*_scaleRatio);
    _pathLab->setGeometry(130*_scaleRatio,72*_scaleRatio,this->width() - 140*_scaleRatio,20*_scaleRatio);
}

void DLnaFileDetailWidget::upDetailInfo()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(14*_scaleRatio);
    _titleName->setFont(font);
    font.setPixelSize(12*_scaleRatio);
    _formatLab->setFont(font);
    _convertbtn->setFont(font);
    _pathLab->setFont(font);
    _convertbtn->setText(Lang("ListVideo/convert"));
    QString fmt = _dlnaInfo.fileExt + "(" + _dlnaInfo.fileCodec + ")";
    QString supfmt = "MP4(H264)";
    if(_dlnaInfo.fileType == 2) {
        supfmt = "MP3(AAC)";
    }
    QString title = Lang("DLNA/compatibel");
    _titleName->setText(DLNA->getCurShowText(_titleName,title));
#ifdef Q_OS_WIN
    _titleName->setToolTip(title);
#endif

    QString fmtLab = Lang("ListVideo/details") + " " + fmt;
    _formatLab->setText(fmtLab);

    QFileInfo file(_dlnaInfo.filePath);
    QString path = Lang("ListVideo/where") + " " + QDir::toNativeSeparators(file.absolutePath());
    _pathLab->setText(DLNA->getVirTextInfo(_pathLab,path));
#ifdef Q_OS_WIN
    _pathLab->setToolTip(path);
#endif

    _videoIco->setStyleSheet("background-color: rgb(0, 0, 0)");
    QPixmap pixmap,showpixmap;
    if(_dlnaInfo.fileshot.isEmpty()) {
        if(_dlnaInfo.fileType == 1)
            pixmap.load(":/res/png/videoshot.png");
        else
            pixmap.load(":/res/png/musicshot.png");
        _videoIco->setPixmap(pixmap);
    } else {
        pixmap.loadFromData(_dlnaInfo.fileshot);
        int width   = 100*_scaleRatio;
        int height  = 80*_scaleRatio;
        int displayH = 0;
        int displayW = 0;
        if(_dlnaInfo.fileAsp != "" &&
           _dlnaInfo.fileAsp != "0:0"){
            int wAsp,hAsp;
            QStringList aspect = _dlnaInfo.fileAsp.split(":");
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

        showpixmap = pixmap.scaled(displayW,displayH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        _videoIco->setPixmap(showpixmap);
    }
}

void DLnaFileDetailWidget::clickConvertBtn()
{
    Global->openConUrl(_dlnaInfo.filePath);
}

void DLnaFileDetailWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
