#ifndef LIVERECENTPLAY_H
#define LIVERECENTPLAY_H

#include "updateinterface.h"
#include "dylabel.h"
class QLabel;
class LiveUrl;
class LiveRecentPlay : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit LiveRecentPlay(QWidget *parent = 0);
    ~LiveRecentPlay();

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void setRecentPathList(const QStringList &recentList);
    void addRecent(const QString &LivePaht);

signals:
    void upWidgetUI();
    void sendLivePlay(const QString &LivePaht);

public slots:
    void recentShowBtnClick();
    void recentDelPath(const QString &LivePaht);

private:
    void Init();
    void upUI();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent(QPaintEvent *event);

private:
    DYLabel*    _showRecentBtn;
    DYLabel*    _title;
    LiveUrl*    _recentList[5];

    bool        _recentShow;
    QStringList _recentPath;
    double      _scaleRatio;
};

#endif // LIVERECENTPLAY_H
