#include "dyvideo.h"
#include "vlc/vlc.h"
#include "dymediaplayer.h"
#include "QtDebug"

#define Player_Core m_pParent->core()

DYVideo::DYVideo(DYMediaPlayer *player) : QObject(player)
{
    m_pParent = player;
}

QMap<int, QString> DYVideo::subtitles() const
{
    QMap<int, QString> tracks;

    if (Player_Core) {
        libvlc_track_description_t *desc, *first;
        first = desc = libvlc_video_get_spu_description(Player_Core);
        while(desc != NULL) {
            tracks.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
            desc = desc->p_next;
        }
        if (first != NULL)
            libvlc_track_description_list_release(first);
    }

    return tracks;
}

int DYVideo::subtitle() const
{
    int subtitle = -1;
    if (Player_Core/* && libvlc_media_player_has_vout(Player_Core)*/) {
        subtitle = libvlc_video_get_spu(Player_Core);
    }

    return subtitle;
}

int DYVideo::subtitleCount() const
{
    int count = -1;
    if (Player_Core) {
        count = libvlc_video_get_spu_count(Player_Core);
    }

    return count;
}

void DYVideo::setSubtitleFile(const QString &subtitle)
{
    if (Player_Core) {
        libvlc_video_set_subtitle_file(Player_Core, subtitle.toUtf8().data());
    }
}

int DYVideo::getSpuDelay()
{
    return libvlc_video_get_spu_delay(Player_Core);
}

void DYVideo::setSpuDelay(const int &t)
{
    // -1失败 0成功
    libvlc_video_set_spu_delay(Player_Core, t);
}

QMap<int, QString> DYVideo::chapters() const
{
    QMap<int, QString> chapters;

    if (Player_Core) {
        int i_title = title();
        if(i_title == -1)
            return chapters;

        libvlc_track_description_t *desc, *first;
        first = desc = libvlc_video_get_chapter_description(Player_Core, i_title);


        while(desc != NULL) {
            chapters.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
            desc = desc->p_next;
        }
        if(first != NULL)
            libvlc_track_description_list_release(first);
    }

    return chapters;
}

int DYVideo::chapter() const
{
    int track = -1;
    if (Player_Core) {
        track = libvlc_media_player_get_chapter(Player_Core);
    }
    return track;
}

int DYVideo::chapterCount() const
{
    int count = -1;
    if (Player_Core) {
        //int i_title = title();
        //if(i_title != -1)
        //    count = libvlc_media_player_get_chapter_count_for_title(Player_Core, i_title);
        count = libvlc_media_player_get_chapter_count(Player_Core);
    }
    return count;
}

QMap<int, QString> DYVideo::titles() const
{
    QMap<int, QString> titleMap;

    if (Player_Core) {
        libvlc_track_description_t *desc, *first;
        first = desc = libvlc_video_get_title_description(Player_Core);

        while(desc != NULL) {
            titleMap.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
            desc = desc->p_next;
        }
        if(first)
            libvlc_track_description_list_release(first);
    }

    return titleMap;
}

int DYVideo::title() const
{
    int track = -1;
    if (Player_Core) {
        track = libvlc_media_player_get_title(Player_Core);
    }
    return track;
}

int DYVideo::titleCount() const
{
    int count = -1;
    if (Player_Core) {
        count = libvlc_media_player_get_title_count(Player_Core);
    }
    return count;
}

int DYVideo::track() const
{
    int track = -1;
    if (Player_Core) {
        track = libvlc_video_get_track(Player_Core);
    }
    return track;
}

int DYVideo::trackCount() const
{
    int count = -1;
    if (Player_Core) {
        count = libvlc_video_get_track_count(Player_Core);
    }

    return count;
}

QMap<int, QString> DYVideo::tracks() const
{
    QMap<int, QString> tracks;

    if (Player_Core) {
        libvlc_track_description_t *desc, *first;
        first = desc = libvlc_video_get_track_description(Player_Core);

        while(desc != NULL) {
            tracks.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
            desc = desc->p_next;
        }
        if(first != NULL) {
            libvlc_track_description_list_release(first);
        }
    }
    return tracks;
}

bool DYVideo::takeSnapshot(const QString &path, const int &w, const int &h) const
{
    bool success = false;
    if (Player_Core && libvlc_media_player_has_vout(Player_Core)) {
        success = libvlc_video_take_snapshot(Player_Core, 0,
                                             QString(path).replace("%","%%").toUtf8().data(),
                                             w, h) + 1;
    }

    return success;
}

void DYVideo::showLogo(const QString &file, int x, int y, int opacity)
{
    if (Player_Core && libvlc_media_player_has_vout(Player_Core)) {
        libvlc_video_set_logo_string(Player_Core, libvlc_logo_file, file.toUtf8().data());
        libvlc_video_set_logo_int(Player_Core, libvlc_logo_x, x);
        libvlc_video_set_logo_int(Player_Core, libvlc_logo_y, y);
        libvlc_video_set_logo_int(Player_Core, libvlc_logo_opacity, opacity);

        libvlc_video_set_logo_int(Player_Core, libvlc_logo_enable, 1);
    }
}

void DYVideo::hideLogo()
{
    if (Player_Core && libvlc_media_player_has_vout(Player_Core)) {
        libvlc_video_set_logo_int(Player_Core, libvlc_logo_enable, 0);
    }
}

void DYVideo::showMarquee(const QString &text, int x, int y, int timeout, int opacity, int size, int color, int refresh)
{
    if (Player_Core && libvlc_media_player_has_vout(Player_Core)) {
        libvlc_video_set_marquee_string(Player_Core, libvlc_marquee_Text, text.toUtf8().data());
        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_X, x);
        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_Y, y);
        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_Timeout, timeout);
        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_Opacity, opacity);
        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_Size, size);
        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_Color, color);
        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_Refresh, refresh);

        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_Enable, 1);
    }
}

bool DYVideo::getVoutSize()
{
    if(Player_Core)
    {
        unsigned w=0, h=0;
        if(libvlc_video_get_size(Player_Core, 0, &w, &h) == 0) // 成功
        {
            return true;
        }
    }
    return false;
}

void DYVideo::setAdjustEnable(const bool &b)
{
    if(Player_Core)
        libvlc_video_set_adjust_int(Player_Core, libvlc_adjust_Enable, b ? 1 : 0);
}

void DYVideo::setContrast(const float &v)
{
    if(!Player_Core) return;
    setAdjustEnable(true);
    libvlc_video_set_adjust_float(Player_Core, libvlc_adjust_Contrast, v);
}

void DYVideo::setBrightness(const float &v)
{
    if(!Player_Core) return;
    setAdjustEnable(true);
    libvlc_video_set_adjust_float(Player_Core, libvlc_adjust_Brightness, v);
}

void DYVideo::setHue(const float &v)
{
    if(!Player_Core) return;
    setAdjustEnable(true);
    libvlc_video_set_adjust_float(Player_Core, libvlc_adjust_Hue, v);
}

void DYVideo::setSaturation(const float &v)
{
    if(!Player_Core) return;
    setAdjustEnable(true);
    libvlc_video_set_adjust_float(Player_Core, libvlc_adjust_Saturation, v);
}

void DYVideo::setGamma(const float &v)
{
    if(!Player_Core) return;
    setAdjustEnable(true);
    libvlc_video_set_adjust_float(Player_Core, libvlc_adjust_Gamma, v);
}

float DYVideo::getContrast()
{
    if(Player_Core) {
        return libvlc_video_get_adjust_float(Player_Core, libvlc_adjust_Contrast);
    }
    return 0.0f;
}

float DYVideo::getBrightness()
{
    if(Player_Core)
        return libvlc_video_get_adjust_float(Player_Core, libvlc_adjust_Brightness);
    return 0.0f;
}

float DYVideo::getHue()
{
    if(Player_Core) {
        return libvlc_video_get_adjust_float(Player_Core, libvlc_adjust_Hue);
    }
    return 0.0f;
}

float DYVideo::getSaturation()
{
    if(Player_Core)
        return libvlc_video_get_adjust_float(Player_Core, libvlc_adjust_Saturation);
    return 0.0f;
}

float DYVideo::getGamma()
{
    if(Player_Core)
        return libvlc_video_get_adjust_float(Player_Core, libvlc_adjust_Gamma);
    return 0.0f;
}

void DYVideo::hideMarquee()
{
    if (Player_Core && libvlc_media_player_has_vout(Player_Core)) {
        libvlc_video_set_marquee_int(Player_Core, libvlc_marquee_Enable, 0);
    }
}

void DYVideo::setvideoTrack(int track)
{
    if(!Player_Core)
        return;
    if(track != -1) //多视频轨道时，要先禁用再启用另外一个
        libvlc_video_set_track(Player_Core, -1);
    libvlc_video_set_track(Player_Core, track);
}

#include "globalarg.h"
void DYVideo::setAspeceRatio(const QString &ratio)
{
    if (Player_Core && libvlc_media_player_has_vout(Player_Core)) {
        if(ratio == "default" || ratio.isEmpty())
        {
            Global->_wRatio = Global->_wOriginalRatio;
            Global->_hRatio = Global->_hOriginalRatio;
            libvlc_video_set_aspect_ratio(Player_Core, "");
        }
        else
        {
            QStringList list = ratio.split(":");
            if(list.count() == 2)
            {
                Global->_wRatio = list.at(0).toInt();
                Global->_hRatio = list.at(1).toInt();
                libvlc_video_set_aspect_ratio(Player_Core, ratio.toUtf8().constData());
            }
        }
        if(!ratio.isEmpty())
            emit changeRatio();
    }
}

void DYVideo::setCropGeometry(const QString &ratio)
{
    if (Player_Core && libvlc_media_player_has_vout(Player_Core)) {
        libvlc_video_set_crop_geometry(Player_Core, ratio.toUtf8().data());
    }
}

void DYVideo::setScale(const DYVlc::Scale &scale)
{
    if (Player_Core && libvlc_media_player_has_vout(Player_Core)) {
        libvlc_video_set_scale(Player_Core, DYVlc::scale()[scale]);
    }
}

void DYVideo::setDeinterlace(const DYVlc::Deinterlacing &filter)
{
    setDeinterlace(DYVlc::deinterlacing()[filter]);
}

void DYVideo::setDeinterlace(const QString &filter)
{
    if (Player_Core)
    {
        if(filter.isEmpty() || filter.toLower() == "disabled")
            libvlc_video_set_deinterlace(Player_Core, NULL);
        else
            libvlc_video_set_deinterlace(Player_Core, filter.toLower().toUtf8().constData());
    }
}

void DYVideo::setSubtitle(int i_subtitle)
{
    if (Player_Core) {
        libvlc_video_set_spu(Player_Core, i_subtitle);
    }
}

void DYVideo::setChapter(int i_chapter)
{
    if (Player_Core) {
        libvlc_media_player_set_chapter(Player_Core, i_chapter);
    }
}

void DYVideo::setTitle(int i_title)
{
    if (Player_Core) {
        libvlc_media_player_set_title(Player_Core, i_title);
    }
}
