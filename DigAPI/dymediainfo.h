#ifndef DYMEDIAINFO_H
#define DYMEDIAINFO_H

#include <QList>
#include <QString>

struct Basic{
    int i_id;
    int i_codec;
    int i_original_fourcc;
    int i_profile;
    int i_level;
    int i_bitrate;
    QString codecName;
    QString language;
    QString description;
};

struct Video{
    Basic *_basic;
    int i_width;
    int i_height;
    int i_dar_w;
    int i_dar_h;
    int i_frame_rate_num;
    int i_frame_rate_den;

    Video() {
        _basic = NULL;
    }

    ~Video() {
        if(_basic == NULL) {
            delete _basic;
            _basic = NULL;
        }
    }
};
struct Audio{
    Basic *_basic;
    int i_channels;
    int i_rate;

    Audio() {
        _basic = NULL;
    }
    ~Audio() {
        if(_basic == NULL) {
            delete _basic;
            _basic = NULL;
        }
    }
};
struct Text{
    Basic *_basic;
    QString encoding;

    Text() {
        _basic = NULL;
    }
    ~Text() {
        if(_basic == NULL) {
            delete _basic;
            _basic = NULL;
        }
    }
};

struct libvlc_media_player_t;

class DYMediaInfo
{
public:
    DYMediaInfo();
    ~DYMediaInfo();

    void parse(libvlc_media_player_t *);

    int videoCount() const{return _videoList.count();}
    int audioCount() const{return _audioList.count();}
    int textCount()  const{return _textList.count();}

    QList<Video*> videoList() const{return _videoList;}
    QList<Audio*> audioList() const{return _audioList;}
    QList<Text*>  textList()  const{return _textList;}

private:
    void clear();
    int gcd(const int &, const int &) const;//计算最大公约数

private:
    QList<Video*> _videoList;
    QList<Audio*> _audioList;
    QList<Text*>  _textList;
};

#endif // DYMEDIAINFO_H
