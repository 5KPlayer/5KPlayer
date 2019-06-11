#include "librarywidget.h"
#include "Library/libleftwidget.h"
#include "Library/playlistwidget.h"

LibraryWidget::LibraryWidget(QWidget *parent) : TransEventWidget(parent)
{
    Init();
}

void LibraryWidget::updateLanguage()
{
    _leftWidget->updateLanguage();
    _rightWidget->updateLanguage();
}

void LibraryWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _leftWidget->updateUI(_scaleRatio);
    _rightWidget->updateUI(_scaleRatio);
}

void LibraryWidget::Init()
{
    _leftWidget = new libLeftWidget(this);
    _rightWidget= new PlayListWidget(this);

    connect(_leftWidget,SIGNAL(ListClick(QString)),_rightWidget,SLOT(itemClickSlt(QString)));
    connect(this,SIGNAL(itemChange(QString)),_leftWidget,SLOT(itemChange(QString)));

    _scaleRatio = 1.0;
    _libLeftWidth = 217;
}

void LibraryWidget::resizeEvent(QResizeEvent *)
{
    _leftWidget->setGeometry(0,0,_libLeftWidth*_scaleRatio,this->height()-4*_scaleRatio);
    _rightWidget->setGeometry(_leftWidget->width(),0,this->width()- _leftWidget->width(),this->height()-4*_scaleRatio);
}

void LibraryWidget::clickedLeftItem(QString item)
{
    emit itemChange(item);
}

