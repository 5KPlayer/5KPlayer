#ifndef DLNAFILEDETAILWIDGET_H
#define DLNAFILEDETAILWIDGET_H

#include <QWidget>
#include "updateinterface.h"
#include "transeventwidget.h"
#include "transeventico.h"
#include "transeventlabel.h"
#include "transeventlabelbtn.h"
#include "transeventicoclk.h"
#include "dystructu.h"
class DYLabel;
class DLnaFileDetailWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit DLnaFileDetailWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setDetailInfo(dlnaLib dlnaInfo);
private:
    void init();
    void upUI();
    void upDetailInfo();
private slots:
    void clickConvertBtn();
protected:
    void resizeEvent(QResizeEvent *);
private:
    TransEventLabel* _videoIco; // 图标
    TransEventLabel* _titleName;

    TransEventLabel* _icoLine;
    TransEventLabel* _icoLine2;

    TransEventLabel* _formatLab;
    TransEventLabel* _pathLab;

    DYLabel*          _convertbtn;

    dlnaLib           _dlnaInfo;
    double            _scaleRatio;
};

#endif // DLNAFILEDETAILWIDGET_H
