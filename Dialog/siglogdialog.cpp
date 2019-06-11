#include "siglogdialog.h"
#include "dylabel.h"

#include <QLabel>
#include "globalarg.h"
SigLogDialog::SigLogDialog(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    _scaleRatio = 1.0;
    _info       = new QLabel(this);
    _ok         = new DYLabel(this,true);
    _info->setWordWrap(true);

    _info->setStyleSheet("color:rgb(182,183,184)");

    _info->setAlignment(Qt::AlignTop);

    _ok->setText(Lge->getLangageValue("Main/ok"));
    _ok->setAlignment(Qt::AlignCenter);
    connect(_ok,SIGNAL(clicked()),this->parentWidget(),SLOT(close()));
    UpUI();
}

void SigLogDialog::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    UpUI();
}

void SigLogDialog::setLogInfo(QString info)
{
    _info->setText(info);
}

void SigLogDialog::UpUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    _info->setFont(font);
    this->resize(400*_scaleRatio,145*_scaleRatio);
    _info->setGeometry(16*_scaleRatio,4*_scaleRatio,372*_scaleRatio,76*_scaleRatio);
    _ok->setGeometry(300*_scaleRatio,93*_scaleRatio,80*_scaleRatio,20*_scaleRatio);
}
