#ifndef MEDIANOSUPAIRPLAY_H
#define MEDIANOSUPAIRPLAY_H

#include <QWidget>
#include "updateinterface.h"

class QLabel;
class DYLabel;
class MediaNoSupAirPlay : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit MediaNoSupAirPlay(QWidget *parent = 0,QString filePath = "");

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private:
    void Init(const QString &filePath);
    void UpUI();

private slots:
    void converterClick();
private:
    QLabel* _info;
    QLabel* _deal;
    DYLabel*_later;
    DYLabel*_converter;
    double  _scaleRatio;
};

#endif // MEDIANOSUPAIRPLAY_H
