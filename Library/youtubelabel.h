#ifndef YOUTUBELABEL_H
#define YOUTUBELABEL_H

#include <QObject>
#include "transeventlabel.h"
#include "updateinterface.h"
class YoutubeLabel : public TransEventLabel,public UpdateInterface
{
    Q_OBJECT
public:
    explicit YoutubeLabel(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
signals:
private:
    void upUI();
public slots:
protected:
    void paintEvent(QPaintEvent *);
private:
    double _scaleRatio;
};

#endif // YOUTUBELABEL_H
