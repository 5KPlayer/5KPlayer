#ifndef URLANALYZEEXIT_H
#define URLANALYZEEXIT_H

#include <QWidget>
#include "updateinterface.h"
#include "dylabel.h"

class UrlAnalyzeExit : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit UrlAnalyzeExit(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);
signals:

public slots:
    void continueClick();
    void cancelClick();
private:
    void Init();
    void upUI();
private:
    QLabel* _info;
    QLabel* _deal;
    DYLabel* _cancel;
    DYLabel* _continue;
    double _scaleRatio;
};

#endif // URLANALYZEEXIT_H
