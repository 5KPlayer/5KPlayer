#include "transeventtextbrowser.h"
#include <QEvent>
#include <QMouseEvent>
#include <QMoveEvent>
TransEventTextBrowser::TransEventTextBrowser(QWidget *parent)
    : QTextBrowser(parent)
{

}

void TransEventTextBrowser::mousePressEvent(QMouseEvent *e)
{
    QTextBrowser::mousePressEvent(e);
    e->ignore();
}

void TransEventTextBrowser::mouseMoveEvent(QMouseEvent *e)
{
    QTextBrowser::mouseMoveEvent(e);
    e->ignore();
}

void TransEventTextBrowser::mouseReleaseEvent(QMouseEvent *e)
{
    QTextBrowser::mouseReleaseEvent(e);
    e->ignore();
}
