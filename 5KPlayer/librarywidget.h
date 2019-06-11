#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include "transeventwidget.h"
#include "updateinterface.h"
class libLeftWidget;
class LibRightWin;
class PlayListWidget;
class LibraryWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit LibraryWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
private:
    void Init();
protected:
    void resizeEvent(QResizeEvent *);
signals:
    void itemChange(QString ItemName);
public slots:
    void clickedLeftItem(QString item);
private:
    libLeftWidget* _leftWidget;
    PlayListWidget* _rightWidget;
    int _libLeftWidth;
    double _scaleRatio;
};

#endif // LIBRARYWIDGET_H
