#include "dymedia.h"
#include <QDir>
#include <QtDebug>
#include <QSysInfo>
#include "vlc/vlc.h"
#include "dyenums.h"
#include "dyinstance.h"
#include "dymediaplayer.h"
#include "globalarg.h"
#include "librarydata.h"
#include "ffmpeg/displayscreen.h"

DYMedia::DYMedia(const QString &location, bool localFile, DYInstance *instance) : QObject(instance)
{
    _vlcMedia = NULL;
    initMedia(location, localFile, instance);
}

DYMedia::~DYMedia()
{
    //removeCoreConnections();
    if(_vlcMedia) {
        libvlc_media_release(_vlcMedia);
        _vlcMedia = NULL;
    }
}

libvlc_media_t *DYMedia::core()
{
    return _vlcMedia;
}

QString DYMedia::curFile() const
{
    return _currentLocation;
}

void DYMedia::initMedia(const QString &location, bool localFile, DYInstance *instance)
{
    _currentLocation = location;
    QString l = location;
    if (localFile)
        l = QDir::toNativeSeparators(l);

    // Create a new libvlc media descriptor from location
    if(_vlcMedia) {
        //removeCoreConnections();
        libvlc_media_release(_vlcMedia);
        _vlcMedia = NULL;
    }

    if (localFile)
        _vlcMedia = libvlc_media_new_path(instance->core(), l.toUtf8().data());
    else
        _vlcMedia = libvlc_media_new_location(instance->core(), l.toUtf8().data());

    //--------------开启硬件加速 只能在这个地方，不能在libvlc_new里面添加--------------//
    libvlc_media_add_option(_vlcMedia, ":avcodec-hw=none");
    if(LibData->isHDCurVideo() && Global->useHardware() == 1) {
        int hwType = Global->userHwType();
        QString codec = LibData->getCurMediaCodec().toLower();
        switch (hwType) {
        case 0:
            // 启用dxva2
            libvlc_media_add_option(_vlcMedia, ":avcodec-hw=dxva2");
            break;
        case 1:
            // Intel QuickSync
            if(codec =="h264") {
                if(DisScreen->supQsvDecoder(LibData->getcurmediaPath(),"h264_qsv") == 1) {
                    libvlc_media_add_option(_vlcMedia, ":avcodec-codec=h264_qsv");
                }
            }
            break;
        case 2:
            // NVIDIA CUVID
            if(codec =="h264") {
                if(DisScreen->supQsvDecoder(LibData->getcurmediaPath(),"h264_cuvid") == 1) {
                    libvlc_media_add_option(_vlcMedia, ":avcodec-codec=h264_cuvid");
                }
            }
            break;
        default:
            break;
        }
    }
//    bool useDxva2 = Global->dxva2();
//    if (useDxva2)
//        libvlc_media_add_option(_vlcMedia, ":avcodec-hw=dxva2");
//    else
//        libvlc_media_add_option(_vlcMedia, ":avcodec-hw=none");

//    QString codec = LibData->getCurMediaCodec().toLower();
//    if(Global->qsv())
//    {
//        if(codec == "h264") {
//            if(DisScreen->supQsvDecoder(LibData->getcurmediaPath(),"h264_qsv") == 1) {
//                libvlc_media_add_option(_vlcMedia, ":avcodec-codec=h264_qsv");
//            }
//        }
//    }
//    if(Global->cuda()) {
//        if(codec == "h264") {
//            if(DisScreen->supQsvDecoder(LibData->getcurmediaPath(),"h264_cuvid") == 1) {
//                libvlc_media_add_option(_vlcMedia, ":avcodec-codec=h264_cuvid");
//                qDebug() << "h264_cuvid";
//            }
//        }
//        else if(codec == "hevc") {
//            if(DisScreen->supQsvDecoder(LibData->getcurmediaPath(),"hevc_cuvid") == 1) {
//                libvlc_media_add_option(_vlcMedia, ":avcodec-codec=hevc_cuvid");
//            }
//        }
//    }
    //------------------------------------------------------------------------//

    libvlc_media_add_option(_vlcMedia, ":file-caching=1000");
    libvlc_media_add_option(_vlcMedia, ":disc-caching=1000");
    libvlc_media_add_option(_vlcMedia, ":network-caching=1000");

    //设置windows系统版本号
    const QString version = QString(":windows-version=%1").arg(Global->winVersion());
    libvlc_media_add_option(_vlcMedia, version.toLatin1().constData());

    //设置DVD打开方式
    QString dvdType = QString(":dvd-type=%1").arg(Global->dvdOpenType());
    libvlc_media_add_option(_vlcMedia,dvdType.toLatin1().constData());

    //createCoreConnections();
}
/*
static void libvlc_callback(const libvlc_event_t *event, void *data)
{
    DYMedia *core = static_cast<DYMedia *>(data);
    if(event->type == libvlc_MediaParsedChanged)
        emit core->parsedChanged((bool)event->u.media_parsed_changed.new_status);
}

void DYMedia::createCoreConnections()
{
    libvlc_event_manager_t *vlcEventManager = libvlc_media_event_manager(_vlcMedia);
    libvlc_event_attach(vlcEventManager, libvlc_MediaParsedChanged, libvlc_callback, this);
}

void DYMedia::removeCoreConnections()
{
    libvlc_event_manager_t *vlcEventManager = libvlc_media_event_manager(_vlcMedia);
    libvlc_event_detach(vlcEventManager, libvlc_MediaParsedChanged, libvlc_callback, this);
}
*/
