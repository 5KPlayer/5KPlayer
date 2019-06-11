#include "previewlabel.h"
#include <QTime>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QResizeEvent>
#include "globalarg.h"

PreviewLabel::PreviewLabel(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("QWidget{background-color: rgb(0,0,0)}");
    pixlabel = new QLabel(this);
    timeLabel = new QLabel(this);
    pixlabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    timeLabel->setStyleSheet("QLabel{color:white;}");
    timeLabel->setText("xx:xx:xx.xxx");
    QFont font;
    font.setPixelSize(12 * Global->_screenRatio);
    timeLabel->setFont(font);
    QFontMetrics fm(font);
    minLabelWidth = fm.width(QString("00:00:00.000"));
    timeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->setMinimumWidth(minLabelWidth);
}

PreviewLabel::~PreviewLabel()
{

}

void PreviewLabel::resizeEvent(QResizeEvent *event)
{
    timeLabel->resize(this->width(),20 * Global->_screenRatio);
    timeLabel->move(0,0);
    pixlabel->move((this->width() - pixlabel->width()) / 2,20 * Global->_screenRatio);
}

void PreviewLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit previewLabelDoubleClicked();
}

void PreviewLabel::setLabelSize(QSize size)
{
    QFont font;
    font.setPixelSize(12 * Global->_screenRatio);
    timeLabel->setFont(font);
    QFontMetrics fm(font);
    minLabelWidth = fm.width(QString("00:00:00.000"));
    timeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->setMinimumWidth(minLabelWidth);

    pixlabel->setFixedSize(size.width(),size.height() - 20 * Global->_screenRatio);
    if(!this->image.isNull())
        pixlabel->setPixmap(QPixmap::fromImage(this->image.scaled(pixlabel->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    this->resize(size);
}

void PreviewLabel::setPixLabelNodata()
{
    QFont font;
    font.setPixelSize(12 * Global->_screenRatio);
    pixlabel->setFont(font);
    pixlabel->setText("No Data");
    pixlabel->setStyleSheet("color: red;");
}

void PreviewLabel::setPixmap(QImage image)
{
    this->image = image;
    if(!this->image.isNull())
    {
        pixlabel->setPixmap(QPixmap::fromImage(this->image.scaled(pixlabel->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        pixlabel->setText("");
    }
//    pixlabel->setPixmap(QPixmap::fromImage(image.scaled(pixlabel->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
}

void PreviewLabel::setPts(int64_t pts)
{
    timeLabel->setText(ptsToString(pts));
}

QString PreviewLabel::ptsToString(int64_t pts)
{
    QTime time;
    int secs = pts / 1000;
    int h = secs / 3600;
    int min = (secs % 3600) / 60;
    secs = secs % 60;
    int msec = pts % 1000;
    time.setHMS(h,min,secs,msec);
    return time.toString("hh:mm:ss.zzz");
}
