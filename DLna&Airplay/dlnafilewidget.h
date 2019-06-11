#ifndef DLNAFILEWIDGET_H
#define DLNAFILEWIDGET_H

#include <QWidget>
#include "updateinterface.h"
#include "transeventwidget.h"
#include "transeventico.h"
#include "transeventlabel.h"
#include "dystructu.h"

class QLabel;
class DLnaFileWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit DLnaFileWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void    setWidgetData(dlnaLib data);
    dlnaLib getWidgetData();
    void    setWidgetSelect(bool select = false,bool ctrl_down = false);
    bool    getWidgetSelect();
    QImage  conImage(QImage image);
private:
    void init();
    void upUI();
    bool fileIsExits(QString path);

protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *e);   //鼠标按下
    void mouseReleaseEvent(QMouseEvent *);
private:
    TransEventIco*      _ico;
    TransEventLabel*    _fileIco;
    QLabel*             _title;

    dlnaLib         _libInfo;
    bool            _isSelect;
    bool            _isSupport;
    bool            _isSvg;
    double          _scaleRatio;
};

#endif // DLNAFILEWIDGET_H
