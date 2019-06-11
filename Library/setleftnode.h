#ifndef SETLEFTNODE_H
#define SETLEFTNODE_H

#include <QWidget>
#include <QLabel>
#include "transeventwidget.h"
#include "transeventico.h"
#include "dystructu.h"
#include "dylabel.h"
#include "updateinterface.h"
#include "svgbutton.h"

class SetLeftNode : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit SetLeftNode(QWidget *parent = 0);

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void setNodeData(QString icoPath,QString title);
    void setFontColor(QString color);
    void setSelect(bool select);

private:
    void Init();
    void upUI();

protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private:
    TransEventIco*  _ico;
    QLabel*         _name;

    QString         _svgPath;
    QString         _nameText;
    bool            _isSelect;
    double          _scaleRatio;
};

#endif // SETLEFTNODE_H
