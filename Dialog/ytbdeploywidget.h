#ifndef YTBDEPLOYWIDGET_H
#define YTBDEPLOYWIDGET_H

#include <QWidget>
#include "updateinterface.h"
#include "dylabel.h"
#include "upgradeytb.h"

class QProgressBar;
class YtbDeployWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit YtbDeployWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

signals:
private:
    void init();
    void upUI();

public slots:
private slots:
    void cancelClick();
    void progressUp(int progress);
    void ytbUpdone();
    void ytbUpFail();
private:
    QLabel*         _info;
    QLabel*         _status;
    DYLabel*        _cancel;
    QProgressBar*   _progrress;     // 完成load进度条

    double          _scaleRatio;
    int             _downloading;
};

#endif // YTBDEPLOYWIDGET_H
