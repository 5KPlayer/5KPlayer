#ifndef DYVLCMEDIA_H
#define DYVLCMEDIA_H

#include <QObject>
#include <QString>
#include <QUrl>

#include "dyenums.h"

class DYInstance;

struct libvlc_media_t;

class DYMedia : public QObject
{
    Q_OBJECT
public:
    explicit DYMedia(const QString &location, bool localFile, DYInstance *instance);

    ~DYMedia();

    libvlc_media_t *core();

    QString curFile() const;

signals:
    /*!
        \brief Signal sent on parsed change
        \param status new parsed status
    */
    void parsedChanged(bool status);

private:
    void initMedia(const QString &location,
                   bool localFile,
                   DYInstance *instance);

    //void createCoreConnections();
    //void removeCoreConnections();

    libvlc_media_t *_vlcMedia;

    QString _currentLocation;
};

#endif // DYVLCMEDIA_H
