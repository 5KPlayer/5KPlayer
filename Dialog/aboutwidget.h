#ifndef ABOUTWIDGET_H
#define ABOUTWIDGET_H

#include "updateinterface.h"

class QLabel;

class AboutWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit AboutWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private:
    void initLabel(QLabel **label, const QString &str);

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    QLabel *p_icoLabel;
    QLabel *p_versionLabel;
    QLabel *p_contentLabel;
    QLabel *p_linkTitleLabel;
    QLabel *p_linkLabel;
    QLabel *p_copyrightLabel;

    int _space;
};

#endif // ABOUTWIDGET_H
