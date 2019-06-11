#include "mediainfowidget.h"
#include <QLabel>
#include <QPalette>
#include <QFont>
#include <QScrollBar>
#include <QFileInfo>
#include <QTextBrowser>
#include <QtDebug>
#include "combobox.h"
#include "globalarg.h"
#include "dymediaplayer.h"

MediaInfoWidget::MediaInfoWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    initLeftLabel(&_titleLabel, "");
    _titleLabel->resize(400, 32);
    _titleLabel->setAlignment(Qt::AlignCenter);
    initLeftLabel(&_sourceLabel, Lge->getLangageValue("MediaInfo/source"));
    initLeftLabel(&_trackInfoLabel, Lge->getLangageValue("MediaInfo/track"));
    initLeftLabel(&_formatLabel, Lge->getLangageValue("MediaInfo/format"));

    initRightLabel(&_pathLabel, "");
    initRightLabel(&_trackDetailLabel, "");

    QPalette p1;
    p1.setColor(QPalette::Text, QColor(115,117,128));
    p1.setColor(QPalette::WindowText, QColor(115,117,128));

    _formatDetailLabel = new QTextBrowser(this);
    _formatDetailLabel->resize(300, 160);
    _formatDetailLabel->setFrameShape(QFrame::NoFrame);
    _formatDetailLabel->setPalette(p1);
    _formatDetailLabel->setReadOnly(true);
    //_formatDetailLabel->setEnabled(false);

    initData();

    updateUI(1.0);

    QTextCursor cursor =  _formatDetailLabel->textCursor();
    cursor.movePosition(QTextCursor::Start);

    _formatDetailLabel->setTextCursor(cursor);
}

void MediaInfoWidget::initData()
{
    const QString file = MediaPlayer->media()->curFile();
    _pathLabel->setText(file);
    QFileInfo fi(file);
    _titleLabel->setText(fi.fileName());
    //MediaPlayer->parse();

    const int i_v = MediaPlayer->mediaInfo()->videoCount();
    const int i_a = MediaPlayer->mediaInfo()->audioCount();
    const int i_t = MediaPlayer->mediaInfo()->textCount();

    _trackDetailLabel->setText(QString("Video-%1, Audio-%2, Subtitle-%3")
                               .arg(i_v)
                               .arg(i_a)
                               .arg(i_t));
    for(int i=0;i<i_v;++i) {
        _formatDetailLabel->append(QString("Video %1: Codec: %2  Resolution: %3x%4")
                                   .arg(i+1)
                                   .arg(MediaPlayer->mediaInfo()->videoList().at(i)->_basic->codecName)
                                   .arg(MediaPlayer->mediaInfo()->videoList().at(i)->i_width)
                                   .arg(MediaPlayer->mediaInfo()->videoList().at(i)->i_height));
    }
    if(i_v > 0)
        _formatDetailLabel->append("");

    for(int i=0;i<i_a;++i) {
        _formatDetailLabel->append(QString("Audio %1: Codec: %2  Rate: %3  Channel: %4")
                                   .arg(i+1)
                                   .arg(MediaPlayer->mediaInfo()->audioList().at(i)->_basic->codecName)
                                   .arg(MediaPlayer->mediaInfo()->audioList().at(i)->i_rate)
                                   .arg(MediaPlayer->mediaInfo()->audioList().at(i)->i_channels));
    }
    if(i_a > 0)
        _formatDetailLabel->append("");

    for(int i=0;i<i_t;++i) {
        _formatDetailLabel->append(QString("Subtitle %1: Language: %2  Description: %3")
                                   .arg(i+1)
                                   .arg(MediaPlayer->mediaInfo()->textList().at(i)->_basic->language)
                                   .arg(MediaPlayer->mediaInfo()->textList().at(i)->_basic->description));
    }
}

void MediaInfoWidget::updateUI(const qreal &scaleRatio)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);
    font.setBold(true);

    _space = 12 * scaleRatio;

    QFontMetrics fontWidth(font);

    _titleLabel->setFont(font);
    _titleLabel->resize(400 * scaleRatio, 32 * scaleRatio);
    const int titleW = fontWidth.width(_titleLabel->text() + "...");
    if(titleW > _titleLabel->width()) {
        _titleLabel->setText(fontWidth.elidedText(_titleLabel->text(), Qt::ElideMiddle, _titleLabel->width()));
    }

    _sourceLabel->setFont(font);
    _sourceLabel->resize(100 * scaleRatio, 24 * scaleRatio);

    _trackInfoLabel->setFont(font);
    _trackInfoLabel->resize(100 * scaleRatio, 24 * scaleRatio);

    _formatLabel->setFont(font);
    _formatLabel->resize(100 * scaleRatio, 24 * scaleRatio);

    font.setBold(false);

    _pathLabel->setFont(font);
    _pathLabel->resize(300 * scaleRatio, 24 * scaleRatio);
    const int pathW = fontWidth.width(_pathLabel->text() + "...");
    if(pathW > _pathLabel->width()) {
        _pathLabel->setText(fontWidth.elidedText(_pathLabel->text(), Qt::ElideMiddle, _pathLabel->width()));
    }

    _formatDetailLabel->verticalScrollBar()->setStyleSheet(ComboBox::verticalStyle(scaleRatio,"rgba(0,0,0,0%)"));

    _trackDetailLabel->setFont(font);
    _trackDetailLabel->resize(300 * scaleRatio, 24 * scaleRatio);

    _formatDetailLabel->setFont(font);
    _formatDetailLabel->resize(300 * scaleRatio, 160 * scaleRatio);

    this->resize(460 * scaleRatio, 300 * scaleRatio);
}

void MediaInfoWidget::resizeEvent(QResizeEvent *)
{
    _titleLabel->move((this->width() - _titleLabel->width())/2, _space);
    _sourceLabel->move(0, _titleLabel->y() + _titleLabel->height() + _space);
    _trackInfoLabel->move(0, _sourceLabel->y() + _sourceLabel->height() + _space);
    _formatLabel->move(0, _trackInfoLabel->y() + _trackInfoLabel->height() + _space);

    _pathLabel->move(_sourceLabel->x() + _sourceLabel->width() + _space, _sourceLabel->y());
    _trackDetailLabel->move(_trackInfoLabel->x() + _trackInfoLabel->width() + _space, _trackInfoLabel->y());
    _formatDetailLabel->move(_formatLabel->x() + _formatLabel->width() + _space, _formatLabel->y());
}

void MediaInfoWidget::initLeftLabel(QLabel **label, const QString &str)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText, QColor(172,173,174));

    (*label) = new QLabel(str, this);
    (*label)->setPalette(p1);
    (*label)->resize(100, 24);
    (*label)->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void MediaInfoWidget::initRightLabel(QLabel **label, const QString &str)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText, QColor(115,117,128));

    (*label) = new QLabel(str, this);
    (*label)->setPalette(p1);
    (*label)->resize(300, 24);
    (*label)->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}
