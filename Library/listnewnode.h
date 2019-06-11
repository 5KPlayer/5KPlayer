#ifndef LISTNEWNODE_H
#define LISTNEWNODE_H

#include <QWidget>
#include "transeventwidget.h"
#include "dylineedit.h"
#include "dylabel.h"
#include "svgbutton.h"
#include "updateinterface.h"
class ListNewNode : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit ListNewNode(QWidget *parent = 0);
    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

private:
    void Init();
    void upUI();
    bool isVailName(QString itemName);

signals:
    void EditChanged(QString);
    void setmouseig(bool ig);
private slots:
    void EditShow();
    void EditChange(QString s);
    void EditEnter();
    void EditFoucsOut();

protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private:
    SvgButton* _ico;
    DYLabel* _text;
    dyLineEdit* _edit;
    bool isClick;
    bool isEnv;
    double _scaleRatio;
    QString _newtext;
};

#endif // LISTNEWNODE_H
