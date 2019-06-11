#ifndef DYDVDINFO_H
#define DYDVDINFO_H

#include <QThread>
#include <QMap>

class DYDVDInfo : public QThread
{
    Q_OBJECT
public:
    explicit DYDVDInfo(QObject *parent = 0);
    QStringList dvdList() const;
    QString findDVD(const int &index);

protected:
    virtual void run();

private:
#if defined( _WIN32 )
    virtual void initDVDList();
#endif

signals:
    void findStarted();
    void findEnded();

private:
    QMap<QString, QString> _dvdMap;
};

#endif // DYDVDINFO_H
