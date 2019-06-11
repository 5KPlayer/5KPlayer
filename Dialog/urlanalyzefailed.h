#ifndef URLANALYZEFAILED_H
#define URLANALYZEFAILED_H

#include "updateinterface.h"
#include "dylabel.h"
class UrlAnalyzeFailed : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit UrlAnalyzeFailed(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private:
    void UpUI();
signals:

public slots:
private:
    DYLabel*     _okBtn;
    QLabel*      _info;
    QLabel*      _detailed;
    double       _scaleRatio;
};

#endif // URLANALYZEFAILED_H
