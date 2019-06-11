#include "dyaudio.h"
#include "dymediaplayer.h"

#include <vlc/vlc.h>
#include <QMap>
#include <QtDebug>

#define Player_Core m_pParent->core()

DYAudio::DYAudio(DYMediaPlayer *player) : QObject(player)
{
    m_pParent = player;
}

DYAudio::~DYAudio()
{
}

bool DYAudio::isMute() const
{
    bool mute = false;
    if (Player_Core) {
        mute = libvlc_audio_get_mute(Player_Core);
    }
    return mute;
}

int DYAudio::track() const
{
    int track = -1;
    if (Player_Core) {
        track = libvlc_audio_get_track(Player_Core);
    }

    return track;
}

int DYAudio::trackCount() const
{
    int count = -1;
    if (Player_Core) {
        count = libvlc_audio_get_track_count(Player_Core);
    }

    return count;
}

QMap<int, QString> DYAudio::tracks() const
{
    QMap<int, QString> tracks;

    if (Player_Core) {
        libvlc_track_description_t *desc, *first;
        first = desc = libvlc_audio_get_track_description(Player_Core);

        while(desc != NULL) {
            tracks.insert(desc->i_id, QString().fromUtf8(desc->psz_name));
            desc = desc->p_next;
        }
        if(first != NULL)
            libvlc_track_description_list_release(first);
    }

    return tracks;
}

DYVlc::AudioChannel DYAudio::channel() const
{
    DYVlc::AudioChannel channel = DYVlc::AudioChannelError;
    if (Player_Core) {
        channel = DYVlc::AudioChannel(libvlc_audio_get_channel(Player_Core));
    }
    return channel;
}

int DYAudio::getDelay()
{
    return libvlc_audio_get_delay(Player_Core);
}

void DYAudio::setDelay(const int &t)
{
    // -1失败 0成功
    libvlc_audio_set_delay(Player_Core, t);
}

int DYAudio::realVolume() const
{
    return libvlc_audio_get_volume(Player_Core);
}

void DYAudio::setVolums(int vol)
{
    if(!Player_Core)
        return;
    i_sliderValue = vol;
    const int volume = valueToVolue(vol);
    libvlc_audio_set_volume(Player_Core, volume);
}

void DYAudio::toggleVolunMute()
{
    if(!Player_Core)
        return;
    libvlc_audio_toggle_mute(Player_Core);
}

void DYAudio::setTrack(int track)
{
    if (Player_Core) {
        libvlc_audio_set_track(Player_Core, track);
    }
}

void DYAudio::setChannel(DYVlc::AudioChannel channel)
{
    if (Player_Core) {
        // Don't change if channel is the same
        if (channel != DYAudio::channel()) {
            libvlc_audio_set_channel(Player_Core, channel);
        }
    }
}

void DYAudio::setChannel(const int &channel)
{
    if (Player_Core) {
        if (channel != DYAudio::channel() && (channel >= 1 && channel <=5))
        {
            libvlc_audio_set_channel(Player_Core, libvlc_audio_output_channel_t(channel));
        }
    }

}

void DYAudio::addVolums()
{
    int vol = i_sliderValue;

    if(vol == 144)
        return;

    vol += 12;
    if(vol > 144)
        vol = 144;

    setVolums(vol);
}

void DYAudio::decVolums()
{
    int vol = i_sliderValue;

    if(vol == 0)
        return;
    vol -= 12;
    if(vol < 0)
        vol = 0;

    setVolums(vol);
}
#include <QtMath>
int DYAudio::valueToVolue(int v) const
{
    // (x-a)^2 + y^2 = 120^2
    // x:滑块值  y:vlc的音量
    if(v < 0 || v > 144)
        v = 12;

    const int r = 160;
    const int a = 144;
    const int x = (v-a);
    const int maxY = 120;
    const int b = r-maxY;

    return qSqrt(r*r-x*x)-b;
}

int DYAudio::volumeToValue() const
{
    if(!Player_Core)
        return -1;

    const int volume = libvlc_audio_get_volume(Player_Core);

    const int r = 160;
    const int y = volume+40;
    const int a = 144;

    return qAbs(qSqrt(r*r-y*y)-a);

}
