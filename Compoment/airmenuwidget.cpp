#include "airmenuwidget.h"
#include "transeventwidget.h"
#include "transeventlabel.h"
#include <QHBoxLayout>
#include <QPalette>
#include <QEvent>
#include "globalarg.h"
AirMenuWidget::AirMenuWidget(QString ico, QString textinfo, bool darwBtm, QWidget *parent)
: QWidgetAction(parent)
{
    _widget     = new TransEventWidget(parent);
    _widget->installEventFilter(this);
    _widget->setWindowFlags(Qt::FramelessWindowHint);
    _widget->setMouseTracking(true);

    if(darwBtm)
        _widget->setDrawLineToBottom(true);
    _checkLab                   = new TransEventLabel(_widget);
    TransEventLabel* _icoLab    = new TransEventLabel(_widget);
    TransEventLabel* _nameLab   = new TransEventLabel(_widget);

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * Global->_screenRatio);
    _checkLab->setFont(font);
    _nameLab->setFont(font);

    QHBoxLayout* layout = new QHBoxLayout;
    _icoLab->setPixmap(QPixmap(ico));
    _nameLab->setText(textinfo);

    _checkLab->setFixedSize(10,10);
    _icoLab->setFixedSize(20,17);
    _nameLab->setFixedSize(120,17);

    _checkLab->setAlignment(Qt::AlignCenter);
    _nameLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _icoLab->setAlignment(Qt::AlignCenter);

    layout->addWidget(_checkLab, 0, Qt::AlignCenter);
    layout->addWidget(_icoLab, 1, Qt::AlignCenter);
    layout->addWidget(_nameLab, 2, Qt::AlignHCenter|Qt::AlignLeft);
    _widget->setLayout(layout);
}

void AirMenuWidget::setChecked(bool check)
{
    if(check) {
        _checkLab->setText("√");
    } else {
        _checkLab->setText("");
    }
}

QWidget *AirMenuWidget::createWidget(QWidget *parent)
{
    _widget->setParent(parent);
    return _widget;
}

bool AirMenuWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Enter) {
        _widget->setStyleSheet("background-color: rgb(106,107,108);");
    } else if(event->type() == QEvent::Leave){
        _widget->setStyleSheet("");
    }
    return QWidgetAction::eventFilter(obj, event);
}
