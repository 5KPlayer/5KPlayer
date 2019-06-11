#ifndef DLNASERVERWIDGET_H
#define DLNASERVERWIDGET_H

#include <QWidget>
#include "transeventwidget.h"
#include "updateinterface.h"
#include "devstruct.h"
class QLabel;
class DYLabel;
class dlnaListWidget;
class QTimer;
class SvgButton;
class DLnaServerWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit DLnaServerWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
signals:
private:
    void init();
    void upUI();
    void upTextInfo();
    QString getCurShowText(QLabel* lab,QString text);

private slots:
    void checkDevSupInfo(QList<deviceInfo> devList, bool isAdd);
    void addFolderBtnClick();
    void addFileBtnClick();
protected:
    void mousePressEvent(QMouseEvent *);   //鼠标按下
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *event);
private:
    QLabel* _firstLab;
    QLabel* _secondLab;

    SvgButton*_addFolderBtn;
    SvgButton*_addFileBtn;

    QLabel* _line1;
    QLabel* _line2;
    QLabel* _devListLab;
    dlnaListWidget* _dlnaListW;

    QString _prePath;
    double  _scaleRatio;
    bool    _mouseRight;
};

#endif // DLNASERVERWIDGET_H
