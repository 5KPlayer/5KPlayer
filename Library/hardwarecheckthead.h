#ifndef HARDWARECHECKTHEAD_H
#define HARDWARECHECKTHEAD_H

#include <QObject>
#include <QThread>

class HardwareCheckThead : public QThread
{
    Q_OBJECT
public:
    explicit HardwareCheckThead(QObject *parent = 0);

    void setCheckType(QString hwName);
signals:
    void sendHwSupport(bool isSupport);

public slots:
private:
    QString _hwName;
protected:
     void run();
};

#endif // HARDWARECHECKTHEAD_H
