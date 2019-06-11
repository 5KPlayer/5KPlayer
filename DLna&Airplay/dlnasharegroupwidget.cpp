#include "dlnasharegroupwidget.h"
#include "globalarg.h"
#include <QLabel>

DLnaShareGroupWidget::DLnaShareGroupWidget(QWidget *parent, QString title)
    : TransEventWidget(parent)
{
    _title  = new QLabel(this);
    _line   = new QLabel(this);

    _titleText = title;
    _title->setText(Lang(_titleText));
    _title->setStyleSheet("color:rgb(183,183,183)");
    _title->setAlignment(Qt::AlignCenter);
    _line->setStyleSheet("background-color:rgb(183,183,183)");
    _scaleRatio = 1.0;
}

void DLnaShareGroupWidget::updateLanguage()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _title->setFont(font);
    _title->setText(Lang(_titleText));
    upUI();
}

void DLnaShareGroupWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void DLnaShareGroupWidget::upUI()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _title->setFont(font);

    int titleW = _title->fontMetrics().width(Lang(_titleText));
    int xPos = 4 * _scaleRatio;
    _title->setGeometry(xPos,0,titleW,this->height());
    _line->setGeometry(8 * _scaleRatio + _title->width(),(this->height() - 1*_scaleRatio)/2,
                       this->width() - (12 * _scaleRatio + _title->width()),1*_scaleRatio);
}

void DLnaShareGroupWidget::resizeEvent(QResizeEvent *event)
{
    upUI();
}
