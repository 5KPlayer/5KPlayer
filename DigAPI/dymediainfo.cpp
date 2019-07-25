#include "dymediainfo.h"
#include "vlc.h"
#include <QtDebug>

#define LIBVLC_USE_PTHREAD_CANCEL 1
#include "vlc/plugins/vlc_fourcc.h"

DYMediaInfo::DYMediaInfo() { }

DYMediaInfo::~DYMediaInfo()
{
    clear();
}

void DYMediaInfo::parse(libvlc_media_player_t *m)
{
    clear();
    libvlc_media_t* media = libvlc_media_player_get_media(m);
    if(media && libvlc_media_is_parsed(media) == -1) {
        libvlc_media_parse(media);
    }
    libvlc_media_track_t **t;
    const int count = libvlc_media_tracks_get(media, &t);
    if(count == 0)
        return;

    for(int i=0; i<count; ++i) {
        Basic *basic = new Basic;
        basic->i_id = t[i]->i_id;
        basic->i_codec = t[i]->i_codec;
        basic->i_original_fourcc = t[i]->i_original_fourcc;
        basic->i_profile = t[i]->i_profile;
        basic->i_level = t[i]->i_level;
        basic->i_bitrate = t[i]->i_bitrate;
        basic->language = t[i]->psz_language;
        basic->description = t[i]->psz_description;

        if(basic->language.isEmpty())
            basic->language = "N/A";

        if(basic->description.isEmpty())
            basic->description = "N/A";

        char str[5];
        vlc_fourcc_to_char(t[i]->i_original_fourcc, str);
        str[4] = '\0';

        basic->codecName = str;

        if(t[i]->i_type == libvlc_track_type_t::libvlc_track_audio)
        {
            Audio *audio = new Audio;
            audio->_basic = basic;
            audio->i_channels = t[i]->audio->i_channels;
            audio->i_rate = t[i]->audio->i_rate;

            _audioList << audio;
        }
        else if(t[i]->i_type == libvlc_track_type_t::libvlc_track_video)
        {
            const int w = t[i]->video->i_width;
            const int h = t[i]->video->i_height;
            const int n = t[i]->video->i_sar_num;
            const int d = t[i]->video->i_sar_den;

            Video *video = new Video;
            video->_basic = basic;
            video->i_width  = w;
            video->i_height = h;
            video->i_frame_rate_num = t[i]->video->i_frame_rate_num;
            video->i_frame_rate_den = t[i]->video->i_frame_rate_den;

            if(n == d) {
                const int tmp = gcd(w, h);
                video->i_dar_w  = w/tmp;
                video->i_dar_h  = h/tmp;
            } else {
                const int tmp = gcd(w * n, h * d);
                video->i_dar_w  = w*n/tmp;
                video->i_dar_h  = h*d/tmp;
            }

            _videoList << video;
        }
        else if(t[i]->i_type == libvlc_track_type_t::libvlc_track_text)
        {
            Text *text = new Text;
            text->_basic = basic;
            text->encoding = t[i]->subtitle->psz_encoding;
            if(text->encoding.isEmpty())
                text->encoding = "N/A";

            _textList << text;
        }
    }
    libvlc_media_tracks_release(t, count);
}

void DYMediaInfo::clear()
{
    qDeleteAll(_videoList);
    qDeleteAll(_audioList);
    qDeleteAll(_textList);

    _videoList.clear();
    _audioList.clear();
    _textList.clear();
}

int DYMediaInfo::gcd(const int &m, const int &n) const
{
    if(m == 0 || n == 0) return 1;

    int temp=0;
    int a = m > n ? m : n;//a取较大数
    int b = m + n - a;    //b取较小数

    while(b) {
        temp = b;
        b = a % b;
        a = temp;
    }
    if(a == 0)
        return 1;
    return a;
}
