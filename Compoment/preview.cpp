#include "preview.h"
#include <QPixmap>
#include <QPainter>
#include <QLabel>
#include <globalarg.h>
#include "dymediaplayer.h"

Preview::Preview(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    //this->setAttribute(Qt::WA_TranslucentBackground);

    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    this->setModal(false);


    p_image     = new QLabel(this);
    p_timeLabel = new QLabel(this);
    p_timeLabel->setAlignment(Qt::AlignCenter);
    this->setStyleSheet("QLabel{background-color: black; color: white}");
}

bool Preview::showImage(const QByteArray &data, const int &time, const int &w, const int &h)
{
    QImage image;
    if(!image.loadFromData(data))
        return false;

    const QString state = MediaPlayer->rotate()->curState();
    if(state == "v") { //垂直翻转
        image = image.mirrored(false, true);
    } else if(state == "h") { //水平翻转
        image = image.mirrored(true, false);
    } else {
        QMatrix matrix;
        matrix.rotate(state.toInt());
        image = image.transformed(matrix, Qt::FastTransformation);
    }

    image = image.scaled(w,h);
    p_image->setPixmap(QPixmap::fromImage(image));
    p_image->resize(w, h);

    QFont font;
    font.setPixelSize(12 * Global->_screenRatio);
    font.setFamily(Global->getFontFamily());

    p_timeLabel->setFont(font);
    p_timeLabel->setText(Global->secondToTime(time));
    p_timeLabel->resize(p_image->width(), 20 * Global->_screenRatio);

    p_image->move(0, 0);
    p_timeLabel->move(0, p_image->height());

    this->resize(p_image->width(), p_timeLabel->y() + p_timeLabel->height());

    return true;
}
