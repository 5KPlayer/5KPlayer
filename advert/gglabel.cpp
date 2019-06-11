#include "gglabel.h"

#include <QMouseEvent>
#include <QFileInfo>
#include <QMovie>

#include <Windows.h>
#include <winuser.h>

GGLabel::GGLabel(QWidget *parent)
    : QLabel(parent)
{
    this->setMouseTracking(true);
    this->setOpenExternalLinks(true);
    this->setStyleSheet("background-color:transparent;");
    _proMovie = NULL;
    _mouseHead  = false;
}

void GGLabel::setMouseHead(bool state)
{
    _mouseHead = state;
}

void GGLabel::setGGData(QString Url)
{
    _dataUrl = Url;
}

QString GGLabel::getGGData()
{
    return _dataUrl;
}

void GGLabel::icoMoveClear()
{
    if(_proMovie) {
        _proMovie->deleteLater();
        _proMovie = NULL;
    }
    this->clear();
}

void GGLabel::setFontColor(QString color)
{
    this->setStyleSheet(QString("background-color:transparent; color: %1")
                         .arg(color));
}

QString GGLabel::getFontColor()
{
    return "#ffffff";
}

void GGLabel::setPixMapPath(QString norPath, QString enPath)
{
    _norPixPath = norPath;
    _enPixPath  = enPath;
    showPixMap(norPath);
}

void GGLabel::showPixMap(QString path)
{
    this->clear();
    if(path.isEmpty())
        return;
    QFileInfo file(path);
    if(file.suffix() == "gif") {
        if(_proMovie != NULL) {
            _proMovie->setFileName(path);
        } else {
            _proMovie   = new QMovie;
            _proMovie->setFileName(path);
        }
        _proMovie->setScaledSize(QSize(this->width(),this->height()));
        this->setScaledContents(true);
        this->setMovie(_proMovie);

        _proMovie->start();
    } else {
        QPixmap map;
        map.load(path);
        this->setPixmap(map.scaled(this->width(),this->height(),
                                   Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }
}

void GGLabel::mouseMoveEvent(QMouseEvent *ev)
{
    QLabel::mouseMoveEvent(ev);
    if(_mouseHead)
        ::SetCursor(LoadCursor(NULL, IDC_HAND));
}

void GGLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    QLabel::mouseReleaseEvent(ev);
    if(this->rect().contains(ev->pos()) && _mouseHead)
        emit clicked();
}

void GGLabel::enterEvent(QEvent *)
{
    if(_mouseHead) {
        ::SetCursor(LoadCursor(NULL, IDC_HAND));
    }
    if(!_enPixPath.isEmpty())
        showPixMap(_enPixPath);
}

void GGLabel::leaveEvent(QEvent *)
{
    if(_mouseHead) {
        ::SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    if(!_norPixPath.isEmpty())
        showPixMap(_norPixPath);
    else
        if(this->text().isEmpty())
            this->clear();
}

