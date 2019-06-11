#ifndef DYINSTANCE_H
#define DYINSTANCE_H

#include <QObject>
#include "dyenums.h"

struct libvlc_instance_t;
struct libvlc_media_library_t;

class DYInstance : public QObject
{
    Q_OBJECT
private:
    DYInstance(const DYInstance&);
    DYInstance& operator =(const DYInstance&);

public:
    explicit DYInstance(QObject *p = NULL);
    ~DYInstance();
    /*!
        \brief Returns libvlc instance object.
        \return libvlc instance (libvlc_instance_t *)
    */
    libvlc_instance_t *core();

    /*!
        \brief Returns current log level (default Vlc::ErrorLevel)
        \return log level
    */
    DYVlc::LogLevel logLevel() const;

    /*!
        \brief Set current log level
        \param level desired log level
        \see Vlc::LogLevel
    */
    void setLogLevel(DYVlc::LogLevel level);

private:
    libvlc_media_library_t *_lib;
    libvlc_instance_t *_vlcInstance;
    DYVlc::LogLevel _logLevel;
};

#endif // DYINSTANCE_H
