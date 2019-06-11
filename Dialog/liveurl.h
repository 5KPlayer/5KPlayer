#ifndef LIVEURL_H
#define LIVEURL_H

#include <QLabel>

#include "updateinterface.h"
#include "transeventicoclk.h"
class SvgButton;
class LiveUrl : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit LiveUrl(QWidget *parent = 0);

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void setUrlPath(const QString &urlPath);
    QString getUrlPath();

signals:
    void playLiveUrl(const QString &urlPath);
    void delLiveUrl(const QString &urlPaht);

private slots:
    void clickDelBtn();
    void clickPlayBtn();

private:
    void Init();
    void upUI();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

private:
    SvgButton*          _urlDelBtn;
    SvgButton*          _urlPlayBtn;
    QLabel*             _urlPathLab;

    QString             _urlPath;
    double              _scaleRatio;
};

#endif // LIVEURL_H
