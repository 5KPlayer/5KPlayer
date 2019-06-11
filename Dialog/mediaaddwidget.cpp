#include "mediaaddwidget.h"
#include "librarydata.h"
#include "globalarg.h"
#include <QElapsedTimer>
#include <QApplication>
#include "QtDebug"
mediaAddWidget::mediaAddWidget(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    Init();
}

void mediaAddWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void mediaAddWidget::setAddfilePath(const QStringList &pathList)
{
    QStringList fileList = LibData->getValidFilePath(pathList);
    _fileCount = fileList.size();
    _loadFileCount = 0;
    _progrress->setRange(_loadFileCount,_fileCount);
    LibData->clearShowItemName();
    LibData->addpathList(fileList);
}

void mediaAddWidget::Init()
{
    _scaleRatio    = 1.0;
    _fileCount     = 0;
    _loadFileCount = 0;
    _fileThumbnail = new QLabel(this);
    _load          = new QLabel(this);
    _loadpath      = new QLabel(this);
    _progrressLabel= new QLabel(this);
    _progrress     = new QProgressBar(this);
    _cancel        = new DYLabel(this,true);

    _load->setText(Lge->getLangageValue("MediaAdd/load"));
    _cancel->setText(Lge->getLangageValue("Main/cancel"));
    _progrressLabel->setText("-/-");
    _load->setStyleSheet("color:rgb(182,183,184)");
    _loadpath->setStyleSheet("color:rgb(182,183,184)");
    _progrressLabel->setStyleSheet("color:rgb(182,183,184)");
    _fileThumbnail->setStyleSheet("background-color: rgb(0, 0, 0)");
    _cancel->setAlignment(Qt::AlignCenter);
    _fileThumbnail->setAlignment(Qt::AlignCenter);
    _progrress->setTextVisible(false);
    connect(_cancel,SIGNAL(clicked()),SLOT(clickCancel()));

    upUI();
}

void mediaAddWidget::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    _load->setFont(font);
    _loadpath->setFont(font);
    _progrressLabel->setFont(font);
    _cancel->setFont(font);
    this->resize(500*_scaleRatio,110*_scaleRatio);
    _fileThumbnail->setGeometry(10*_scaleRatio,5*_scaleRatio,100*_scaleRatio,100*_scaleRatio);
    _load->setGeometry(120*_scaleRatio,10*_scaleRatio,60*_scaleRatio,20*_scaleRatio);
    _loadpath->setGeometry(_load->x()+_load->width(),10*_scaleRatio,310*_scaleRatio,20*_scaleRatio);
    _progrress->setGeometry(120*_scaleRatio,45*_scaleRatio,310*_scaleRatio,10*_scaleRatio);
    _progrressLabel->setGeometry(435*_scaleRatio,40*_scaleRatio,65*_scaleRatio,20*_scaleRatio);
    _cancel->setGeometry(365*_scaleRatio,70*_scaleRatio,85*_scaleRatio,20*_scaleRatio);
}

void mediaAddWidget::clickCancel()
{
    emit stopanalyze();
    this->parentWidget()->close();
}

void mediaAddWidget::loadfinish()
{
    QString proLabel = QString::number(_fileCount);
    proLabel.append("/").append(QString::number(_fileCount));
    _progrressLabel->setText(proLabel);
    _progrress->setValue(_fileCount);
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < 500)
    {
        qApp->processEvents();
    }
    this->parentWidget()->close();
}

void mediaAddWidget::upLoadWidget(const QList<MEDIAINFO> &mediaList)
{
    _loadFileCount += mediaList.size();
    QString proLabel = QString::number(_loadFileCount);
    proLabel.append("/").append(QString::number(_fileCount));
    _progrressLabel->setText(proLabel);
    _loadpath->setText(mediaList.at(0).filepath);
    _progrress->setValue(_loadFileCount);
    int imageH=_fileThumbnail->height(),imageW=_fileThumbnail->width();

    STREAMINFO stream = LibData->getVideoStream(mediaList.at(0));
    if (stream.resolution.height() != 0) {
        if(stream.resolution.height()>stream.resolution.width()) {
            imageH = _fileThumbnail->height();
            imageW  = _fileThumbnail->height()*stream.resolution.width()/stream.resolution.height();
        } else {
            imageW  = _fileThumbnail->width();
            imageH = _fileThumbnail->width()*stream.resolution.height()/stream.resolution.width();
        }
    }

    //qDebug() << imageH<<imageW;
    if(!mediaList.at(0).screenshot.isEmpty()) {
        QPixmap pixmap, fitpixmap;
        pixmap.loadFromData(mediaList.at(0).screenshot);
        fitpixmap=pixmap.scaled(imageW,imageH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _fileThumbnail->setPixmap(fitpixmap);
    }
}
