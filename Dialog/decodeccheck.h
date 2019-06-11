#ifndef DECODECCHECK_H
#define DECODECCHECK_H

#include <QObject>
#include "updateinterface.h"

class DYLabel;
class QLabel;
class QTimer;
class DecodecCheck : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit DecodecCheck(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    int getSupQsv();
signals:

private:
    void init();
    void upUi();
    void videoEncoderCheck();

private slots:
    void clickCancel();
    void timeOut();
private:
    QLabel         *_info;
    DYLabel        *_cancel;        // 取消load按钮
    double          _scaleRatio;
    bool            _bIsSupQsv;
    QTimer          *_timer;
};

#endif // DECODECCHECK_H
