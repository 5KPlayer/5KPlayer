#include "delmediawidget.h"
#include "cdialog.h"
#include "globalarg.h"

DelMediaWidget::DelMediaWidget(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    Init();
    this->resize(400,175);
}

void DelMediaWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    UpUI();
}

void DelMediaWidget::Init()
{
    _scaleRatio = 1.0;
    _info   = new QLabel(this);
    _deal   = new QLabel(this);
    _cancel = new DYLabel(this,true);
    _move   = new DYLabel(this,true);
    _keep   = new DYLabel(this,true);
    _info->setWordWrap(true);
    _deal->setWordWrap(true);

    _info->setStyleSheet("color:rgb(182,183,184)");
    _deal->setStyleSheet("color:rgb(115,117,128)");

    _info->setAlignment(Qt::AlignTop);
    _deal->setAlignment(Qt::AlignTop);
    _info->setText(Lge->getLangageValue("DelMedia/info"));

    _deal->setText(Lge->getLangageValue("DelMedia/deal"));

    _cancel->setText(Lge->getLangageValue("Main/cancel"));
    _move->setText("Move to Trash");
    _keep->setText(Lge->getLangageValue("Main/ok"));
    _cancel->setAlignment(Qt::AlignCenter);
    _move->setAlignment(Qt::AlignCenter);
    _keep->setAlignment(Qt::AlignCenter);
    connect(_cancel,SIGNAL(clicked()),this->parentWidget(),SLOT(close()));
    connect(_move,SIGNAL(clicked()),SLOT(clickMove()));
    connect(_keep,SIGNAL(clicked()),SLOT(clickKeep()));

    _move->setVisible(false);
    UpUI();
}

void DelMediaWidget::UpUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    _deal->setFont(font);
    _cancel->setFont(font);
    _keep->setFont(font);
    font.setBold(true);
    _info->setFont(font);
    this->resize(400*_scaleRatio,175*_scaleRatio);
    _info->setGeometry(15*_scaleRatio,18*_scaleRatio,375*_scaleRatio,50*_scaleRatio);
    _deal->setGeometry(15*_scaleRatio,58*_scaleRatio,375*_scaleRatio,50*_scaleRatio);
    _cancel->setGeometry(195*_scaleRatio,120*_scaleRatio,80*_scaleRatio,20*_scaleRatio);
    //_move->setGeometry(180*_scaleRatio,128*_scaleRatio,95*_scaleRatio,20*_scaleRatio);
    _keep->setGeometry(290*_scaleRatio,120*_scaleRatio,95*_scaleRatio,20*_scaleRatio);
}

void DelMediaWidget::clickMove()
{
    CDialog* dialog = (CDialog*)this->parentWidget();
    dialog->done(1);
}

void DelMediaWidget::clickKeep()
{
    CDialog* dialog = (CDialog*)this->parentWidget();
    dialog->done(2);
}

