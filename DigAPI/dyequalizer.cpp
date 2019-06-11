#include "dyequalizer.h"
#include <cmath>
#include <vlc/vlc.h>

#include "dymediaPlayer.h"

DYEqualizer::DYEqualizer(DYMediaPlayer *vlcMediaPlayer) : QObject(vlcMediaPlayer)
{
    m_pParent = vlcMediaPlayer;
    _vlcEqualizer = libvlc_audio_equalizer_new();
}

DYEqualizer::~DYEqualizer()
{
    if (_vlcEqualizer) {
        libvlc_audio_equalizer_release(_vlcEqualizer);
    }
}

float DYEqualizer::amplificationForBandAt(uint bandIndex) const
{
    if (_vlcEqualizer) {
        float ret = libvlc_audio_equalizer_get_amp_at_index(_vlcEqualizer, bandIndex);
        if (!std::isnan(ret)) {
            return ret;
        }
    }
    return -1.0;
}

uint DYEqualizer::bandCount() const
{
    if (_vlcEqualizer) {
        return libvlc_audio_equalizer_get_band_count();
    }
    return 0;
}

float DYEqualizer::bandFrequency(uint bandIndex) const
{
    if (_vlcEqualizer) {
        return libvlc_audio_equalizer_get_band_frequency(bandIndex);
    }
    return -1.0;
}

float DYEqualizer::preamplification() const
{
    if (_vlcEqualizer) {
        return libvlc_audio_equalizer_get_preamp(_vlcEqualizer);
    }
    return 0.0;
}

uint DYEqualizer::presetCount() const
{
    return libvlc_audio_equalizer_get_preset_count();
}

QString DYEqualizer::presetNameAt(int index) const
{
    if(index < 0)
        return "Disable";
    const char *name = libvlc_audio_equalizer_get_preset_name(index);
    if (name == NULL) {
        return QString();
    }
    return QString(name);
}

void DYEqualizer::loadFromPreset(int index)
{
    if(index < 0) {
        setEnabled(false);
        return;
    }
    if (_vlcEqualizer) {
        libvlc_audio_equalizer_release(_vlcEqualizer);
    }
    _vlcEqualizer = libvlc_audio_equalizer_new_from_preset(index);
    if (_vlcEqualizer) {
        //emit presetLoaded();
        setEnabled(true);
    }
}

void DYEqualizer::setAmplificationForBandAt(float amp, uint bandIndex)
{
    if (!_vlcEqualizer) {
        return;
    }
    libvlc_audio_equalizer_set_amp_at_index(_vlcEqualizer, amp, bandIndex);
    libvlc_media_player_set_equalizer(m_pParent->core(), _vlcEqualizer);
}

void DYEqualizer::setEnabled(bool enabled)
{
    if (enabled && _vlcEqualizer != NULL) {
        libvlc_media_player_set_equalizer(m_pParent->core(), _vlcEqualizer);
    } else {
        libvlc_media_player_set_equalizer(m_pParent->core(), NULL);
    }
}

void DYEqualizer::setPreamplification(float value)
{
    if (!_vlcEqualizer) {
        return;
    }
    libvlc_audio_equalizer_set_preamp(_vlcEqualizer, value);
    libvlc_media_player_set_equalizer(m_pParent->core(), _vlcEqualizer);
}
