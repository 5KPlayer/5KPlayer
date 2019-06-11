#ifndef DLNALABEL_H
#define DLNALABEL_H

#include <QObject>
#include <QLabel>

#include "updateinterface.h"
#include "transeventlabel.h"
class DLnaLabel : public QLabel, public UpdateInterface
{
    Q_OBJECT
public:
    explicit DLnaLabel(QWidget *parent = 0,bool state = false);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void setLabelAct(bool actstate);
    bool getLabelAct();
private:
    void upUI();
signals:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    double  _scaleRatio;
    bool    _clkstate;
    const QString norState = "color: rgb(117, 125, 136);border:1px solid rgb(3,88,71);";
    const QString actState = "color: rgb(1,75,93);background-color: rgb(30,223,187);";
};

#endif // DLNALABEL_H
