#ifndef APPLENOTIFY_H
#define APPLENOTIFY_H

#include <QObject>
#include <QUsb>

class AppleNotify : public QObject
{
    Q_OBJECT
public:
    explicit AppleNotify(QObject *parent = 0);

    void supportIPod();
    void supportIPad();
    void supportIPhone();

private slots:
    void onDeviceInserted(QtUsb::FilterList list);

signals:
    void appleDeviceInserted();

private:
    QUsbManager *m_pUsbManager;
    QStringList supports; //pid list
};

#endif // APPLENOTIFY_H
