#include "urlformatwidget.h"
#include "librarydata.h"
#include "globalarg.h"
URLFormatWidget::URLFormatWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void URLFormatWidget::updateLanguage()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _quality       ->setFont(font);
    _qualityInfo   ->setFont(font);
    _HDflag        ->setFont(font);
    _format        ->setFont(font);
    _formatInfo    ->setFont(font);
    _fileSize      ->setFont(font);
    _SizeInfo      ->setFont(font);

    _quality->setText(Lge->getLangageValue("YTBFormat/quality"));
    _format->setText(Lge->getLangageValue("YTBFormat/format"));
    _fileSize->setText(Lge->getLangageValue("YTBFormat/size"));
    if(_formatdata.streamType == 1) {
        _qualityInfo->setText(Lge->getLangageValue("Menu/audio"));
    }
}

void URLFormatWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    UpUI();
    _checkBox->updateUI(scaleRatio);
}

void URLFormatWidget::setDataSouce(const STREAMINFO &formatinfo)
{
    _formatdata = formatinfo;
    QString quality;
    if(formatinfo.streamType == 2) {
        if(formatinfo.resolution.width() == 0) {
            quality = "N/A";
        } else {
            quality =  QString::number(formatinfo.resolution.width()).append("x").append(QString::number(formatinfo.resolution.height()));
        }
    } else {
        quality = Lge->getLangageValue("Menu/audio");
    }
    _qualityInfo->setText(quality);
    if(formatinfo.resolution.height()>= 1080 || (formatinfo.bitRate > 30 && formatinfo.resolution.height()>= 720))
        _HDflag->show();
//    QString format = formatinfo.ext;
//    if(formatinfo.codec.contains("mp4") || formatinfo.ext == "mp4") {   // 这里判断不是很准确，需要改进
//        if(formatinfo.streamType == 1) {
//            format.append("(AAC)");
//        } else {
//            format.append("(H.264)");
//        }
//    }
    _formatInfo->setText(formatinfo.ext);
    QString fileText = "N/A";
    if(formatinfo.fileSize>0)
        fileText = LibData->getfileSize(formatinfo.fileSize,1);
    _SizeInfo->setText(fileText);
}

STREAMINFO URLFormatWidget::getDataSouce()
{
    return _formatdata;
}

void URLFormatWidget::setCheckBox(bool check)
{
    _checkBox->setChecked(check);
}

void URLFormatWidget::checkBoxClick(bool check)
{
    if(!check) {
        _checkBox->setChecked(true);
    } else {
        _checkBox->setChecked(true);
    }
    emit upFormat(_formatdata);
}

void URLFormatWidget::Init()
{
    _checkBox       = new DYCheckBox(this);
    _quality        = new TransEventLabel(this);
    _qualityInfo    = new TransEventLabel(this);
    _HDflag         = new TransEventLabel(this);
    _format         = new TransEventLabel(this);
    _formatInfo     = new TransEventLabel(this);
    _fileSize       = new TransEventLabel(this);
    _SizeInfo       = new TransEventLabel(this);

    _quality->setStyleSheet("Color:rgb(182,183,184)");
    _qualityInfo->setStyleSheet("Color:rgb(182,183,184)");
    _HDflag->setStyleSheet("Color:rgb(0,128,0)");
    _format->setStyleSheet("Color:rgb(182,183,184)");
    _formatInfo->setStyleSheet("Color:rgb(182,183,184)");
    _fileSize->setStyleSheet("Color:rgb(182,183,184)");
    _SizeInfo->setStyleSheet("Color:rgb(182,183,184)");

    _format->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _fileSize->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _HDflag->setText("HD");
    _HDflag->hide();
    _scaleRatio = 1.0;
    connect(_checkBox,SIGNAL(checkChange(bool)),SLOT(checkBoxClick(bool)));
    _quality->setText(Lge->getLangageValue("YTBFormat/quality"));
    _format->setText(Lge->getLangageValue("YTBFormat/format"));
    _fileSize->setText(Lge->getLangageValue("YTBFormat/size"));
}

void URLFormatWidget::UpUI()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _quality       ->setFont(font);
    _qualityInfo   ->setFont(font);
    _HDflag        ->setFont(font);
    _format        ->setFont(font);
    _formatInfo    ->setFont(font);
    _fileSize      ->setFont(font);
    _SizeInfo      ->setFont(font);
    _checkBox->setGeometry(5*_scaleRatio,(this->height()-16*_scaleRatio)/2,16*_scaleRatio,16*_scaleRatio);
    int YPos = (this->height()-20*_scaleRatio)/2;
    _quality->setGeometry(25*_scaleRatio,YPos,50*_scaleRatio,20*_scaleRatio);
    _qualityInfo->setGeometry(80*_scaleRatio,YPos,70*_scaleRatio,20*_scaleRatio);
    _HDflag->setGeometry(150*_scaleRatio,YPos,30*_scaleRatio,20*_scaleRatio);
    _format->setGeometry(this->width()-280*_scaleRatio,YPos,80*_scaleRatio,20*_scaleRatio);
    _formatInfo->setGeometry(this->width()-196*_scaleRatio,YPos,72*_scaleRatio,20*_scaleRatio);
    _fileSize->setGeometry(this->width()-115*_scaleRatio,YPos,60*_scaleRatio,20*_scaleRatio);
    _SizeInfo->setGeometry(this->width()-52*_scaleRatio,YPos,50*_scaleRatio,20*_scaleRatio);

}

void URLFormatWidget::resizeEvent(QResizeEvent *)
{
    UpUI();
}
