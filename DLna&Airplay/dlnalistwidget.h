#ifndef DLNALISTWIDGET_H
#define DLNALISTWIDGET_H

#include <QWidget>

#include "transeventwidget.h"
#include "transeventscrollarea.h"
#include "updateinterface.h"

class dlnaDevInfoWidget;
class QScrollArea;
class dlnaListWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit dlnaListWidget(QWidget *parent = 0);
    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void upWidget(bool isAdd);
private:
    void upDevicePos();
signals:

private slots:
    void scroolBarChange(int pos);


protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *e);
private:
    double  _scaleRatio;
    QList<dlnaDevInfoWidget*>   _devList;
    TransEventWidget*           _pListWidget;      // listWidget主界面
    QScrollBar*                 _pScrollBar;        // 滚动条
};

#endif // DLNALISTWIDGET_H
