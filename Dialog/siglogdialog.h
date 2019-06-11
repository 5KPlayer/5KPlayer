#ifndef SIGLOGDIALOG_H
#define SIGLOGDIALOG_H

#include <QWidget>
#include "updateinterface.h"

class DYLabel;
class QLabel;
class SigLogDialog : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SigLogDialog(QWidget *parent = 0);

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void setLogInfo(QString info);
private:
    void UpUI();
private:
    QLabel      *_info;
    DYLabel     *_ok;
    double       _scaleRatio;
};

#endif // SIGLOGDIALOG_H
