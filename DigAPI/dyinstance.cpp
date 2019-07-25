#include "dyinstance.h"

#include <vlc/vlc.h>
#include <QDebug>
#include "globalarg.h"
#include "dyenums.h"

#ifdef Q_OS_WIN
#include "common/asprintf.h"
#endif

void logCallback(void *data,
                 int level,
                 const libvlc_log_t *ctx,
                 const char *fmt,
                 va_list args)
{
    Q_UNUSED(ctx)

    DYInstance *instance = static_cast<DYInstance *>(data);
    if (instance->logLevel() > level) {
        return;
    }

    char *result;
    if (vasprintf(&result, fmt, args) < 0) {
        return; // LCOV_EXCL_LINE
    }
    QString message(result);
    free(result);

    message.prepend("libvlc: ");

    switch (level) {
    case DYVlc::ErrorLevel:
        qCritical(message.toUtf8().data(), NULL);
        break;
    case DYVlc::WarningLevel:
        qWarning(message.toUtf8().data(), NULL);
        break;
    case DYVlc::NoticeLevel:
    case DYVlc::DebugLevel:
    default:
        qDebug(message.toUtf8().data(), NULL);
        break;
    }
}
#include <QColor>
DYInstance::DYInstance(QObject *parent) : QObject(parent)
{
    //_lib = NULL;
    _vlcInstance = NULL;
    _logLevel = DYVlc::ErrorLevel;

    QStringList args;

    //读取配置文件
    QString encode   = Global->subtitleEncode();
    QString fontName = Global->subtitleFont();

    args << "--intf=auto"
         << "--vout=direct3d"
         << "--ignore-config"
         << "--no-osd"    //On Screen Display
         << "--no-stats"  //Locally collect statistics
         << "--no-video-title-show"
         << "--no-snapshot-preview" //不显示字幕和截图预览
         << "--file-caching=1000"
         << "--disc-caching=1000"
         << "--network-caching=1000"
         << QString("--subsdec-encoding=%1").arg(encode)               //编码格式
         << QString("--freetype-font=%1").arg(fontName)                //字体集 //  Leelawadee UI 泰语
         << QString("--sub-autodetect-fuzzy=0");

// Convert arguments to required format
#if defined(Q_OS_WIN32) // Will be removed on Windows if confirmed working
    char **argv = (char **)malloc(sizeof(char **) * args.count());
    for (int i = 0; i < args.count(); ++i)
        argv[i] = (char *)qstrdup(args.at(i).toUtf8().data());
#else
    char *argv[args.count()];
    for (int i = 0; i < args.count(); ++i)
        argv[i] = (char *)qstrdup(args.at(i).toUtf8().data());
#endif

    // Create new libvlc instance
    _vlcInstance = libvlc_new(args.count(), argv);

    // Check if instance is running
    if (_vlcInstance) {
        //_lib = libvlc_media_library_new(_vlcInstance);
        //libvlc_media_library_load(_lib);

        libvlc_log_set(_vlcInstance, logCallback, this);
    } else {
        qCritical() << "VLC-Qt Error: libvlc failed to load!";
    }
}

DYInstance::~DYInstance()
{
    if (_vlcInstance) {
        //libvlc_media_library_release(_lib);
        libvlc_release(_vlcInstance);
    }
}

libvlc_instance_t *DYInstance::core()
{
    return _vlcInstance;
}

DYVlc::LogLevel DYInstance::logLevel() const
{
    return _logLevel;
}

void DYInstance::setLogLevel(DYVlc::LogLevel level)
{
    _logLevel = level;
}

