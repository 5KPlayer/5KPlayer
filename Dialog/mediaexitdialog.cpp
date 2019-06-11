#include "mediaexitdialog.h"
#include "cdialog.h"
#include "globalarg.h"
MediaExitDialog::MediaExitDialog(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    Init();
}

void MediaExitDialog::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    UpUI();
}

void MediaExitDialog::Init()
{
     _scaleRatio= 1.0;
    _info = new QLabel(this);
    _deal = new QLabel(this);
    _cancel    = new DYLabel(this,true);
    _cleanUp   = new DYLabel(this,true);
    _removeit  = new DYLabel(this,true);
    _info->setWordWrap(true);
    _deal->setWordWrap(true);

    _info->setStyleSheet("color:rgb(182,183,184)");
    _deal->setStyleSheet("color:rgb(115,117,128)");

    _info->setAlignment(Qt::AlignTop);
    _deal->setAlignment(Qt::AlignTop);
    _info->setText(Lge->getLangageValue("MediaExit/info"));
    _deal->setText(Lge->getLangageValue("MediaExit/deal"));

    _cancel->setText(Lge->getLangageValue("Main/cancel"));
    _cleanUp->setText(Lge->getLangageValue("MediaExit/clearup"));
    _removeit->setText(Lge->getLangageValue("MediaExit/remove"));
    _cancel->setAlignment(Qt::AlignCenter);
    _cleanUp->setAlignment(Qt::AlignCenter);
    _removeit->setAlignment(Qt::AlignCenter);
    connect(_cancel,SIGNAL(clicked()),this->parentWidget(),SLOT(close()));
    connect(_cleanUp,SIGNAL(clicked()),SLOT(clickCleanUp()));
    connect(_removeit,SIGNAL(clicked()),SLOT(clickRemoveit()));

    UpUI();
}

void MediaExitDialog::UpUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    _deal->setFont(font);
    _cancel->setFont(font);
    _cleanUp->setFont(font);
    _removeit->setFont(font);
    font.setBold(true);
    _info->setFont(font);
    this->resize(400*_scaleRatio,175*_scaleRatio);
    _info->setGeometry(15*_scaleRatio,18*_scaleRatio,375*_scaleRatio,50*_scaleRatio);
    _deal->setGeometry(15*_scaleRatio,68*_scaleRatio,375*_scaleRatio,50*_scaleRatio);
    _cancel->setGeometry(15*_scaleRatio,128*_scaleRatio,80*_scaleRatio,20*_scaleRatio);
    _cleanUp->setGeometry(180*_scaleRatio,128*_scaleRatio,95*_scaleRatio,20*_scaleRatio);
    _removeit->setGeometry(290*_scaleRatio,128*_scaleRatio,95*_scaleRatio,20*_scaleRatio);
}

void MediaExitDialog::clickCleanUp()
{
    CDialog* dialog = (CDialog*)this->parentWidget();
    dialog->done(1);
}

void MediaExitDialog::clickRemoveit()
{
    CDialog* dialog = (CDialog*)this->parentWidget();
    dialog->done(2);
}
