#include "openninginfowidget.h"
#include <QPainter>
#include <QMouseEvent>
#include "globalarg.h"

OpenningInfoWidget::OpenningInfoWidget(QWidget *parent) : QWidget(parent)
{
#if 0
    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    this->setPalette(pal);
#else
    this->setAttribute(Qt::WA_TranslucentBackground);
#endif

    //this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    //this->setModal(false);
}

void OpenningInfoWidget::paintEvent(QPaintEvent *)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(16 * Global->_screenRatio);
    font.setBold(true);

    QPainter p(this);
    p.setFont(font);
    p.setPen(Qt::red);
    p.fillRect(this->rect(), QColor(0, 0, 0, 40));
    p.drawText(this->rect(), Qt::AlignCenter, "Loading, Please wait a moment...");
}

void OpenningInfoWidget::mousePressEvent(QMouseEvent *e)
{
    e->accept();
}
