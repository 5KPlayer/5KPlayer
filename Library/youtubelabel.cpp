#include "youtubelabel.h"
#include <QPainter>
#include <QDebug>
#include "globalarg.h"
YoutubeLabel::YoutubeLabel(QWidget *parent)
    : TransEventLabel(parent)
{
    _scaleRatio = 1.0;
}

void YoutubeLabel::updateLanguage()
{

}

void YoutubeLabel::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    this->update();
}

void YoutubeLabel::upUI()
{

}

void YoutubeLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;

    double edistance = 2*_scaleRatio;
    double ellipsesize = this->height()-2*edistance;

    double fontwidth = (ellipsesize/2);
    double distance = 20*_scaleRatio;
    double linewid = 4*_scaleRatio;
    double pointLineY = (this->height())/2;

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(fontwidth);
    font.setBold(true);
    pen.setWidth(0);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setBrush(QBrush(QColor(91,185,93),Qt::SolidPattern));
    painter.setPen(QColor(91,185,93));
    painter.drawEllipse(edistance,edistance,ellipsesize,ellipsesize);
    QRectF one(edistance+(ellipsesize-fontwidth)/2,edistance+(ellipsesize-fontwidth)/2,fontwidth,fontwidth);
    painter.drawRect(one);

    painter.setBrush(QBrush(QColor(60,160,212),Qt::SolidPattern));
    painter.setPen(QColor(60,160,212));
    painter.drawEllipse((this->width()-ellipsesize)/2,edistance,ellipsesize,ellipsesize);
    QRectF two((this->width()-ellipsesize+fontwidth)/2,edistance+(ellipsesize-fontwidth)/2,fontwidth,fontwidth);
    painter.drawRect(two);

    painter.setBrush(QBrush(QColor(244,167,69),Qt::SolidPattern));
    painter.setPen(QColor(244,167,69));
    painter.drawEllipse(this->width()-ellipsesize-edistance,edistance,ellipsesize,ellipsesize);
    QRectF thr(this->width()-edistance-ellipsesize+(ellipsesize-fontwidth)/2,edistance+(ellipsesize-fontwidth)/2,fontwidth,fontwidth);
    painter.drawRect(thr);

    int point = edistance+ellipsesize+distance;

    pen.setWidth(linewid);

    pen.setColor(QColor(60,160,212));
    painter.setPen(pen);
    painter.drawLine(QPoint(point,pointLineY),QPoint((this->width()-ellipsesize)/2-distance,pointLineY));

    pen.setColor(QColor(244,167,69));
    painter.setPen(pen);
    painter.drawLine(QPoint((this->width()+ellipsesize)/2+distance,pointLineY),QPoint((this->width()-ellipsesize-distance*2),pointLineY));

    pen.setWidth(1);
    painter.setPen(Qt::white);

    painter.drawText(one,Qt::AlignCenter,"1");
    painter.drawText(two,Qt::AlignCenter,"2");
    painter.drawText(thr,Qt::AlignCenter,"3");
}

