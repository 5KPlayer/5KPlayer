#ifndef HARDWARECHECK_H
#define HARDWARECHECK_H

#include <QThread>

class HardwareCheck : public QThread
{
    Q_OBJECT
public:
    explicit HardwareCheck(QObject *parent = nullptr);

signals:
    void doShowInfo(const QVariant &info);

public slots:
    void checkHardware();
    void stopHardware();
private:
    bool b_isStoped;

protected:
    void run();

};

#endif // HARDWARECHECK_H
