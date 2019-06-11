#ifndef AIRMENUWIDGET_H
#define AIRMENUWIDGET_H

#include <QWidgetAction>
class TransEventLabel;
class TransEventWidget;
class AirMenuWidget : public QWidgetAction
{
public:
    explicit AirMenuWidget(QString ico,QString meuninfo,bool darwBtm,QWidget *parent = 0);

    void setChecked(bool check);

protected:
    QWidget * createWidget(QWidget *parent);
    bool eventFilter(QObject *, QEvent *);
private:
    TransEventWidget*    _widget;
    TransEventLabel*     _checkLab;
};

#endif // AIRMENUWIDGET_H
