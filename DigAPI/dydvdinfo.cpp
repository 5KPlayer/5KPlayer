#include "dydvdinfo.h"

#include <vlc/vlc.h>
#include <vlc/plugins/vlc_common.h>
#include <vlc/plugins/vlc_charset.h>

#include <QtDebug>

DYDVDInfo::DYDVDInfo(QObject *parent) : QThread(parent)
{
}

void DYDVDInfo::run()
{
    emit findStarted();
#ifdef _WIN32
    initDVDList();
#endif
    emit findEnded();
}

QStringList DYDVDInfo::dvdList() const
{
    QStringList dvdList;
    foreach (const QString key, _dvdMap.keys()) {
        dvdList << key + " - " + _dvdMap.value(key);
    }
    return dvdList;
}

QString DYDVDInfo::findDVD(const int &index)
{
    const QList<QString> keys = _dvdMap.keys();
    if(index < 0 || index >= keys.count())
        return "";
    return keys.at(index);
    //return _dvdMap.value(keys.at(index));
}

#ifdef _WIN32
void DYDVDInfo::initDVDList()
{
    _dvdMap.clear();
    wchar_t szDrives[512];
    szDrives[0] = '\0';
    if( GetLogicalDriveStringsW( sizeof( szDrives ) - 1, szDrives ) )
    {
        wchar_t *drive = szDrives;
        UINT oldMode = SetErrorMode( SEM_FAILCRITICALERRORS );
        while( *drive )
        {
            if( GetDriveTypeW(drive) == DRIVE_CDROM )
            {
                wchar_t psz_name[512] = L"";
                GetVolumeInformationW( drive, psz_name, 511, NULL, NULL, NULL, NULL, 0 );

                char *psz_drive = FromWide( drive );
                QString displayName = psz_drive;
                char *psz_title = FromWide( psz_name );

                _dvdMap[displayName] = psz_title;

                free( psz_drive );
                free( psz_title );
            }

            /* go to next drive */
            while( *(drive++) );
        }
        SetErrorMode(oldMode);
    }

    /*
    char *psz_config = config_GetPsz( p_intf, "dvd" );
    free( psz_config );
    //intf_Eject( p_intf, qtu( ui.deviceCombo->currentText() ) );
    */
}
#endif
