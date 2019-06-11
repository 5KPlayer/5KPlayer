#include "aboutwidget.h"
#include <QLabel>
#include <QFont>
#include <QPalette>
#include <QVersionNumber>
#include "globalarg.h"

AboutWidget::AboutWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    p_icoLabel = new QLabel(this);
    p_icoLabel->setStyleSheet("background:transparent");
    p_icoLabel->setPixmap(QPixmap(":/res/png/application.png"));

    QVersionNumber cur(Global->_version_maj, Global->_version_min);

    initLabel(&p_versionLabel, Lge->getLangageValue("about/version") + ": " + cur.toString());
    initLabel(&p_contentLabel, "");

    initLabel(&p_linkTitleLabel, Lge->getLangageValue("about/linkTitle"));
    QString strValue = QString("about_%1").arg(Global->getLanguage());
    QString url = Global->getLanguageUrl(strValue);
    initLabel(&p_linkLabel, "<a style='color: rgb(53,128,167);' " +
                            QString("href=%1>https://www.5kplayer.com</a>").arg(url));
    initLabel(&p_copyrightLabel, Lge->getLangageValue("about/copyright"));

    p_linkLabel->setOpenExternalLinks(true);

    p_contentLabel->setWordWrap(true);
    p_contentLabel->setText(Lge->getLangageValue("about/content"));
    updateUI(1.0);
}

void AboutWidget::updateUI(const qreal &scaleRatio)
{
    _space = 8 * scaleRatio;

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);
    font.setBold(true);

    p_linkTitleLabel->setFont(font);
    p_linkTitleLabel->adjustSize();

    p_copyrightLabel->setFont(font);
    p_copyrightLabel->adjustSize();

    font.setBold(false);

    p_versionLabel->setFont(font);
    p_versionLabel->adjustSize();

    p_contentLabel->setFont(font);
    //p_contentLabel->adjustSize();
    p_contentLabel->resize(420 * scaleRatio, 200 * scaleRatio);

    p_linkLabel->setFont(font);
    p_linkLabel->adjustSize();

    p_icoLabel->setFixedSize(80 * scaleRatio, 80 * scaleRatio);

    this->resize(440 * scaleRatio, 408 * scaleRatio);
}

void AboutWidget::resizeEvent(QResizeEvent *)
{
    const int w = this->width();
    p_icoLabel->move((w-p_icoLabel->width())/2, _space);

    p_versionLabel->move((w-p_versionLabel->width())/2,
                         p_icoLabel->y() + p_icoLabel->height() + _space);

    p_contentLabel->move(0, p_versionLabel->y() + p_versionLabel->height() + _space);

    p_linkTitleLabel->move(0, p_contentLabel->y() + p_contentLabel->height() + _space);

    p_linkLabel->move(0, p_linkTitleLabel->y() + p_linkTitleLabel->height() + _space/4);

    p_copyrightLabel->move(0, p_linkLabel->y() + p_linkLabel->height() + _space);
}

void AboutWidget::initLabel(QLabel **label, const QString &str)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText, Qt::white);

    (*label) = new QLabel(str, this);
    (*label)->setPalette(p1);
    (*label)->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}
