#include "decodeccheck.h"
#include <QLabel>
#include "cdialog.h"
#include "dylabel.h"
#include "globalarg.h"
#include "mfx/checkmfx.h"
#include <QDebug>
#include <QFile>
#include <QTimer>


DecodecCheck::DecodecCheck(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    init();
}

void DecodecCheck::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUi();
}

int DecodecCheck::getSupQsv()
{
    int ret = 0;
    if(_bIsSupQsv)
        ret = 1;
    return ret;
}

void DecodecCheck::init()
{
    _info       = new QLabel(this);
    _cancel     = new DYLabel(this,true);
    _timer      = new QTimer(this);
    _scaleRatio = 1.0;

    _cancel->setAlignment(Qt::AlignCenter);
    _cancel->setText(Lge->getLangageValue("Main/ok"));

    _cancel->setVisible(false);

    connect(_timer,SIGNAL(timeout()),SLOT(timeOut()));
    _timer->setInterval(1500);
    _timer->setSingleShot(true);

    _info->setText(Lge->getLangageValue("QSV/checking"));
    _info->setStyleSheet("color:rgb(182,183,184)");

    this->parentWidget()->setStyleSheet("background: rgb(29,29,30)");

    videoEncoderCheck();
    connect(_cancel,SIGNAL(clicked()),SLOT(clickCancel()));
    upUi();
}

void DecodecCheck::upUi()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    _info->setFont(font);
    _cancel->setFont(font);

    this->resize(400*_scaleRatio,175*_scaleRatio);
    _info->setGeometry(5*_scaleRatio,0*_scaleRatio,385*_scaleRatio,50*_scaleRatio);
    _cancel->setGeometry(300*_scaleRatio,128*_scaleRatio,80*_scaleRatio,20*_scaleRatio);
}

void DecodecCheck::videoEncoderCheck()
{
    _timer->start();
    CheckMfx mfx;
    _bIsSupQsv = mfx.checkMfxSupHw();
}

void DecodecCheck::clickCancel()
{
    CDialog* dialog = (CDialog*)this->parentWidget();
    if(_bIsSupQsv)
        dialog->done(1);
    else
        dialog->done(0);
}

void DecodecCheck::timeOut()
{
    if(_bIsSupQsv)
        _info->setText(Lge->getLangageValue("QSV/checksup"));
    else
        _info->setText(Lge->getLangageValue("QSV/checknosup"));
    _cancel->setVisible(true);
}
