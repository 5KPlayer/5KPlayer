#include "youtubehead.h"
#include "language.h"
#include <QDebug>
#include "dylabel.h"
#include "globalarg.h"
#include <QDesktopServices>
#include <QUrl>
YoutubeHead::YoutubeHead(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void YoutubeHead::updateLanguage()
{
    InitLanguage();
}

void YoutubeHead::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _step->updateUI(scaleRatio);
    upUI();
}

void YoutubeHead::Init()
{
    _scaleRatio = 1.0;
    _title      = new QLabel(this);
    _step       = new YoutubeLabel(this);
    _onetext    = new QLabel(this);
    _twotext    = new QLabel(this);
    _thrtext    = new QLabel(this);

    _title->setStyleSheet("Color:rgb(255,255,255)");
    _title->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    _onetext->setStyleSheet("Color:rgb(91,185,93)");
    _onetext->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _twotext->setStyleSheet("Color:rgb(60,160,212)");
    _twotext->setAlignment(Qt::AlignCenter);
    _thrtext->setStyleSheet("Color:rgb(244,167,69)");
    _thrtext->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    InitLanguage();
}

void YoutubeHead::upUI()
{

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(13*_scaleRatio);
    _title->setFont(font);
    font.setPixelSize(12*_scaleRatio);
    _onetext->setFont(font);
    _thrtext->setFont(font);
    _twotext->setFont(font);
    _step->updateUI(_scaleRatio);

    int distanceW = 20*_scaleRatio;
    int distanceH = 10*_scaleRatio;
    int stepH = 30*_scaleRatio;
    int LabH = 15*_scaleRatio;

    int titleWidth = (this->width()-distanceW*3)/2;
    _title->setGeometry(distanceW,distanceH,titleWidth,LabH);

    int textWidth = (this->width() - distanceW*2 - 6*_scaleRatio)/3;
    int textYpos = _step->y()+_step->height()+distanceH;
    _onetext->setGeometry(distanceW,textYpos,textWidth,LabH);
    _twotext->setGeometry(distanceW+textWidth+2*_scaleRatio,textYpos,textWidth,LabH);
    _thrtext->setGeometry(distanceW+textWidth*2+2*_scaleRatio,textYpos,textWidth,LabH);

    InitLanguage();

    int _stepStart  = _oneTextWidth/2+distanceW;
    int _stepEnd    = (this->width() - _thrTextWidth/2 - distanceW)-_stepStart;
    _step->setGeometry(_stepStart,_title->y()+_title->height()+distanceH, _stepEnd, stepH);
}

void YoutubeHead::InitLanguage()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(13*_scaleRatio);
    _title->setFont(font);
    font.setPixelSize(12*_scaleRatio);
    _onetext->setFont(font);
    _thrtext->setFont(font);
    _twotext->setFont(font);

    QString showtext;

    int titleLenth = _title->fontMetrics().width(Lge->getLangageValue("YTBHead/title"));
    if(titleLenth < _title->width()) {
        showtext = Lge->getLangageValue("YTBHead/title");
    } else {
        QString text = Lge->getLangageValue("YTBHead/title");
        for(int i=0;i<text.size();i++) {
            showtext = text.left(i).append("...");
            int showWidth = _title->fontMetrics().width(showtext);
            if(showWidth > _title->width()) {
                showtext = text.left(i-1).append("...");
                break;
            }
        }
    }
    _title->setText(showtext);

    int oneLenth = _onetext->fontMetrics().width(Lge->getLangageValue("YTBHead/stpOne"));
    if(oneLenth<_onetext->width()) {
        showtext = Lge->getLangageValue("YTBHead/stpOne");
        _oneTextWidth = oneLenth;
    } else {
        QString oneText = Lge->getLangageValue("YTBHead/stpOne");
        for(int i=0;i<oneText.size();i++) {
            showtext = oneText.left(i).append("...");
            int showWidth = _onetext->fontMetrics().width(showtext);
            if(showWidth > _onetext->width()) {
                showtext = oneText.left(i-1).append("...");
                break;
            }
        }
        _oneTextWidth = _onetext->fontMetrics().width(showtext);
    }
    _onetext->setText(showtext);
    int twoLenth = _twotext->fontMetrics().width(Lge->getLangageValue("YTBHead/stpTwo"));
    if(twoLenth<_twotext->width()) {
        showtext = Lge->getLangageValue("YTBHead/stpTwo");
    } else {
        QString twoText = Lge->getLangageValue("YTBHead/stpTwo");
        for(int i=0;i<twoText.size();i++) {
            showtext = twoText.left(i).append("...");
            int showWidth = _twotext->fontMetrics().width(showtext);
            if(showWidth > _twotext->width()) {
                showtext = twoText.left(i-1).append("...");
                break;
            }
        }
    }
    _twotext->setText(showtext);
    int thrLenth = _thrtext->fontMetrics().width(Lge->getLangageValue("YTBHead/stpThr"));
    if(thrLenth<_thrtext->width()) {
        showtext = Lge->getLangageValue("YTBHead/stpThr");
        _thrTextWidth = thrLenth;
    } else {
        QString thrText = Lge->getLangageValue("YTBHead/stpThr");
        for(int i=0;i<thrText.size();i++) {
            showtext = thrText.left(i).append("...");
            int showWidth = _thrtext->fontMetrics().width(showtext);
            if(showWidth > _thrtext->width()) {
                showtext = thrText.left(i-1).append("...");
                break;
            }
        }
        _thrTextWidth = _thrtext->fontMetrics().width(showtext);;
    }
    _thrtext->setText(showtext);
}

void YoutubeHead::resizeEvent(QResizeEvent *)
{
    upUI();
}
