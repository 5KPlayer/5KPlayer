#ifndef BONJOURFIND_H
#define BONJOURFIND_H

#include <QObject>
#include "singleton.h"
#include "qzeroconf.h"

#define BJFind BonjourFind::getInstance()

class BonjourFind : public QObject,public Singleton<BonjourFind>
{
    Q_OBJECT
    friend class  Singleton<BonjourFind>;
    friend class  QSharedPointer<BonjourFind>;

public:
    QList<QZeroConfService *> dacpList() const;

private slots:
    void addDacpSever(QZeroConfService *);
    void delDacpSever(QZeroConfService *);

private:
    Q_DISABLE_COPY(BonjourFind)
    explicit BonjourFind(QObject *parent = 0);
    ~BonjourFind();

    QZeroConf m_airplayFind;
    QZeroConf m_playerFind;
    QZeroConf m_dacpFind;

    QList<QZeroConfService *> m_dacpList;
};

#endif // BONJOURFIND_H
