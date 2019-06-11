#ifndef ANALYZEWIDGET_H
#define ANALYZEWIDGET_H

#include "transeventwidget.h"
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include "svgbutton.h"
#include "dylabel.h"
#include "urlprowidget.h"
#include "updateinterface.h"
#include "dystructu.h"
class AnalyzeWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit AnalyzeWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

private:
    void Init();
    void upUI();
    void pathanalyze();
    bool analyzepath();     // 分析路径是否是网址
    void setprogressSty(bool isInval);

    void InitLanguage();

protected:
    void resizeEvent(QResizeEvent *);

signals:
    void addUrlAnalyze(QString url);
public slots:
    void analyzeClick();                      // 点击分析按钮
    void analyAndPaste();                     // 点击粘贴&分析

private slots:
    void editChange(QString text);
    void clickYoutube();
    void clickMore();
private:
    DYLabel*        _pastebtn;
    SvgButton*      _analyzebtn;
    QLabel*         _message;
    QLineEdit*      _edit;
    QProgressBar*   _progrress;

    DYLabel*        _ytbBtn;
    DYLabel*        _moreBtn;
    double          _scaleRatio;

    QString          _UrlInvalid;
    QString          _UrlEmpty;
};

#endif // ANALYZEWIDGET_H
