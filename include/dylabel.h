#ifndef DYLABEL_H
#define DYLABEL_H


#include <QObject>
#include <QLabel>

#include "updateinterface.h"
#include "transeventlabel.h"
class DYLabel : public QLabel, public UpdateInterface
{
    Q_OBJECT
public:
    DYLabel(QWidget * parent = 0,bool state = false);

    void setMouseHead(bool state);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void Enable(bool enable);
    void setBackClolr(QString norColor,QString entColor,QString pressColor);

signals:
    void clicked();
protected:
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
private:
    double  _scaleRatio;
    bool    _mouseHead;
    bool    _styleState;
    bool    _mouseEnter;
    bool    _btnEnable;
    QString _btnStyle;
    QString _enterColor;
    QString _leaveColor;
    QString _pressColor;
};

#endif // DYLABEL_H
