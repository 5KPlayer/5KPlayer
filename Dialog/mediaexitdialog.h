#ifndef MEDIAEXITDIALOG_H
#define MEDIAEXITDIALOG_H
/*
  双击媒体播放，文件所在路径没有相应媒体的dialog
*/
#include <QWidget>
#include "updateinterface.h"
#include <QLabel>
#include "dylabel.h"
class MediaExitDialog : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit MediaExitDialog(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);
signals:

private:
    void Init();
    void UpUI();

public slots:
    void clickCleanUp();
    void clickRemoveit();
private:
    QLabel* _info;
    QLabel* _deal;
    DYLabel* _cancel;
    DYLabel* _cleanUp;
    DYLabel* _removeit;
    double _scaleRatio;
};

#endif // MEDIAEXITDIALOG_H
