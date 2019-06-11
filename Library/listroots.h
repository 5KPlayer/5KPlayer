#ifndef LISTROOTS_H
#define LISTROOTS_H

#include <QWidget>
#include "transeventwidget.h"
#include "dylabel.h"
#include "updateinterface.h"
class ListRoots : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit ListRoots(bool show = true,QWidget *parent = 0);

    void setRootText(const QString &name, const QString &color = "");
    void setBtnNameShow(bool isShow);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
signals:
    void clickShow();
private:
    void Init();
    void upUI();
protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private:
    bool _isShowBtn;
    QLabel* _name;
    DYLabel* _showBtn;
    double _scaleRatio;
    QString _show;
    QString _hide;
    bool    _isShow;
};

#endif // LISTROOTS_H
