#ifndef YOUTUBEHEAD_H
#define YOUTUBEHEAD_H

#include <QLabel>

#include "transeventwidget.h"
#include "updateinterface.h"
#include "youtubelabel.h"
class DYLabel;
class YoutubeHead : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit YoutubeHead(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

signals:

private:
    void Init();
    void upUI();
    void InitLanguage();
protected:
    void resizeEvent(QResizeEvent *);
private:
    QLabel* _title;
    YoutubeLabel* _step;
    QLabel* _onetext;
    QLabel* _twotext;
    QLabel* _thrtext;

    double _scaleRatio;
    double _oneTextWidth;
    double _thrTextWidth;
};

#endif // YOUTUBEHEAD_H
