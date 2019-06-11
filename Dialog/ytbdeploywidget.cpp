#include "ytbdeploywidget.h"
#include "cdialog.h"
#include <QProgressBar>
#include "globalarg.h"
YtbDeployWidget::YtbDeployWidget(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    init();
}

void YtbDeployWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();

}

void YtbDeployWidget::init()
{
    _scaleRatio = 1.0;
    _downloading= 3;
    _cancel     = new DYLabel(this,true);
    _cancel->setText(Lge->getLangageValue("Main/cancel"));
    _cancel->setAlignment(Qt::AlignCenter);

    _info       = new QLabel(this);
    _status     = new QLabel(this);

    _info->setWordWrap(true);
    _status->setWordWrap(true);

    _info->setStyleSheet("color:rgb(182,183,184)");
    _status->setStyleSheet("color:rgb(115,117,128)");

    _info->setAlignment(Qt::AlignTop);
    _status->setAlignment(Qt::AlignTop);
    //_info->setText(Lge->getLangageValue("UrlExit/info"));
    //_status->setText(Lge->getLangageValue("UrlExit/deal"));
    _info->setText(Lge->getLangageValue("YTBUpgrade/firstuser"));
    _status->setText(Lge->getLangageValue("YTBUpgrade/downing"));

    _progrress  = new QProgressBar(this);
    _progrress->setValue(0);
    _progrress->setTextVisible(false);

    connect(UpYTB,SIGNAL(ytbUpProgress(int)),SLOT(progressUp(int)));
    connect(UpYTB,SIGNAL(ytbUpFinish()),SLOT(ytbUpdone()));
    connect(UpYTB,SIGNAL(ytbUpFail()),SLOT(ytbUpFail()));
    connect(_cancel,SIGNAL(clicked()),SLOT(cancelClick()));

    if(!Global->_down_ytb) {
        UpYTB->checkUpGrade(true);
    }
    upUI();
}

void YtbDeployWidget::upUI()
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _status->setFont(font);
    font.setBold(true);
    _info->setFont(font);

    this->resize(400*_scaleRatio,145*_scaleRatio);

    _info->setGeometry(10*_scaleRatio,4*_scaleRatio,310*_scaleRatio,68*_scaleRatio);
    _status->setGeometry(10*_scaleRatio,72*_scaleRatio,310*_scaleRatio,16*_scaleRatio);
    _progrress->setGeometry(10*_scaleRatio,92*_scaleRatio,310*_scaleRatio,10*_scaleRatio);
    _cancel->setGeometry(240*_scaleRatio,108*_scaleRatio,80*_scaleRatio,20*_scaleRatio);
}

void YtbDeployWidget::cancelClick()
{
    UpYTB->ytdDownCancel();
    CDialog* dialog = (CDialog*)this->parentWidget();
    dialog->done(0);
}

void YtbDeployWidget::progressUp(int progress)
{
    _progrress->setValue(progress);
}

void YtbDeployWidget::ytbUpdone()
{
    CDialog* dialog = (CDialog*)this->parentWidget();
    dialog->done(1);
}

void YtbDeployWidget::ytbUpFail()
{
    _progrress->hide();
    _status->setText(Lge->getLangageValue("YTBUpgrade/fail"));
}
