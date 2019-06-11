#ifndef MEDIAINFOWIDGET_H
#define MEDIAINFOWIDGET_H

#include "updateinterface.h"
class QLabel;
class QTextBrowser;

class MediaInfoWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit MediaInfoWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    void initLeftLabel(QLabel **label, const QString &str);
    void initRightLabel(QLabel **label, const QString &str);

    void initData();

private:
    QLabel *_titleLabel;
    QLabel *_sourceLabel;
    QLabel *_trackInfoLabel;
    QLabel *_formatLabel;

    QLabel *_pathLabel;
    QLabel *_trackDetailLabel;
    QTextBrowser *_formatDetailLabel;

    int _space;
};

#endif // MEDIAINFOWIDGET_H
