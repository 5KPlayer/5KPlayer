#ifndef DELMEDIAWIDGET_H
#define DELMEDIAWIDGET_H

#include <QWidget>
#include "updateinterface.h"
#include <QLabel>
#include "dylabel.h"

class DelMediaWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit DelMediaWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);
private:
    void Init();
    void UpUI();
private slots:
    void clickMove();
    void clickKeep();

private:
    QLabel* _info;
    QLabel* _deal;
    DYLabel* _cancel;
    DYLabel* _move;
    DYLabel* _keep;
    double _scaleRatio;
};

#endif // DELMEDIAWIDGET_H
