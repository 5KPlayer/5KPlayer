#ifndef DLNASHAREWIDGET_H
#define DLNASHAREWIDGET_H

#include <QWidget>
#include "transeventwidget.h"
#include "updateinterface.h"
#include "dystructu.h"
class QLabel;
class SvgButton;
class DLnaShareListWidget;
class DLnaShareWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit DLnaShareWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

signals:
private:
    void init();
    void upUI();
public slots:
    void upShareWidgetSlot(bool isUp);
private slots:
    void delSelDLnaLib();
    void upDelBtnEnable(bool state);
    void addFolderBtnClick();
    void addFileBtnClick();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    SvgButton*              _addFolderBtn;
    SvgButton*              _addFileBtn;
    SvgButton*              _delBtn;
    QLabel*                 _line;
    DLnaShareListWidget*    _shareWidget;
    QString                 _prePath;
    double                  _scaleRatio;
    bool                    _shareWidgetinit;
};

#endif // DLNASHAREWIDGET_H
