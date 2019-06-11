#ifndef DLNASHAREGROUPWIDGET_H
#define DLNASHAREGROUPWIDGET_H

#include <QWidget>
#include "transeventwidget.h"
#include "updateinterface.h"

class QLabel;
class DLnaShareGroupWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit DLnaShareGroupWidget(QWidget *parent = 0,QString title = "");

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
private:
    void upUI();
signals:

protected:
    void resizeEvent(QResizeEvent *event);
private:
    QLabel* _title;
    QLabel* _line;

    QString _titleText;
    double  _scaleRatio;
};

#endif // DLNASHAREGROUPWIDGET_H
