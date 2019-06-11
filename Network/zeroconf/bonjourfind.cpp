#include "bonjourfind.h"
#include "librarydata.h"

#include <QHostInfo>

BonjourFind::BonjourFind(QObject *parent) : QObject(parent)
{
    connect(&m_airplayFind, SIGNAL(serviceAdded(QZeroConfService *)), LibData,SLOT(addBjSev(QZeroConfService*)));
    connect(&m_airplayFind, SIGNAL(serviceRemoved(QZeroConfService *)),LibData,SLOT(delBjSev(QZeroConfService*)));
    m_airplayFind.startBrowser("_airplay._tcp");

    connect(&m_dacpFind, SIGNAL(serviceAdded(QZeroConfService *)),  SLOT(addDacpSever(QZeroConfService*)));
    connect(&m_dacpFind, SIGNAL(serviceRemoved(QZeroConfService *)),SLOT(delDacpSever(QZeroConfService*)));
    m_dacpFind.startBrowser("_dacp._tcp");

    connect(&m_playerFind, SIGNAL(serviceAdded(QZeroConfService *)),  LibData,SLOT(addBjSev(QZeroConfService*)));
    connect(&m_playerFind, SIGNAL(serviceRemoved(QZeroConfService *)),LibData,SLOT(delBjSev(QZeroConfService*)));
    m_playerFind.startBrowser("_5Kplayer._tcp");
    m_playerFind.clearServiceTxtRecords();
    m_playerFind.addServiceTxtRecord("Qt", "the best!");
    m_playerFind.addServiceTxtRecord("ZeroConf is nice too");
    m_playerFind.startServicePublish(QHostInfo::localHostName().toLatin1().data(), "_5Kplayer._tcp", "local", 11437);
}

BonjourFind::~BonjourFind()
{
    m_playerFind.stopServicePublish();

    m_airplayFind.stopBrowser();
    m_playerFind.stopBrowser();
    m_dacpFind.stopBrowser();
}

QList<QZeroConfService *> BonjourFind::dacpList() const
{
    return m_dacpList;
}

void BonjourFind::addDacpSever(QZeroConfService *server)
{
    m_dacpList << server;
}

void BonjourFind::delDacpSever(QZeroConfService *server)
{
    m_dacpList.removeOne(server);
}
