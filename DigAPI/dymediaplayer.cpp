#include "dymediaplayer.h"
#include "vlc/vlc.h"
#include "globalarg.h"

#include "librarydata.h"
#include "openfile.h"
#include <QDir>
#include <QtDebug>
#include "medianosupairplay.h"
#include "playermainwidget.h"

DYMediaPlayer::DYMediaPlayer(QObject *p) : QObject(p)
{
    _instance = new DYInstance(this);
//    delete _instance;
//    _instance = NULL;
    _dyVideoWidget= NULL;
    _dyMedia = NULL;
    _vlcMediaPlayer = NULL;

    _dyMediaInfo = new DYMediaInfo;
    _dyVideo = new DYVideo(this);
    _dyAudio = new DYAudio(this);
    _dyEqualizer = new DYEqualizer(this);
    m_pRotate = new VLCCoreInteraction(this);
}

DYMediaPlayer::~DYMediaPlayer()
{
    _dyAudio->disconnect();
    deinit();

    delete _dyVideo;
    delete _dyAudio;
    delete _dyEqualizer;
    delete _dyMediaInfo;
    delete m_pRotate;
}

void DYMediaPlayer::deinit()
{
    if(_instance)
    {
        this->audio()->muteChanged(false);

        vclConnections(false);
        if(_dyMedia)
        {
            delete _dyMedia;
            _dyMedia = NULL;
        }

        if(_vlcMediaPlayer != NULL) {
            libvlc_media_player_release(_vlcMediaPlayer);
            _vlcMediaPlayer = NULL;
        }

//        delete _instance;
//        _instance = NULL;
    }
}

void DYMediaPlayer::playFile(const QString &file, const bool &isFile)
{
    deinit();

    //_instance = new DYInstance(this);
    _vlcMediaPlayer = libvlc_media_player_new(_instance->core());
    vclConnections(true);
    //防止播放过程中切换文件   视频->音频（音频不需要置顶）
    if(Global->stayOnTop() == CGlobal::General_Stay::Stay_Playing && !PMW->isHidden())
        PMW->offTop();

    if(OpenFileDialog->curPlayType() == OPenFile::Video ||
            OpenFileDialog->curPlayType() == OPenFile::DVD ||
            OpenFileDialog->curPlayType() == OPenFile::DVDMirror)
    {
        this->rotate()->closeFlip();
        this->rotate()->closeRotate();
    }

    if(isFile)
        _dyMedia = new DYMedia(QDir::toNativeSeparators(file), true, _instance);
    else
        _dyMedia = new DYMedia(file, false, _instance);

    libvlc_media_player_set_media(_vlcMediaPlayer, _dyMedia->core());

    play();
}

libvlc_media_player_t *DYMediaPlayer::core() const
{
    return _vlcMediaPlayer;
}

int DYMediaPlayer::length() const
{
    return libvlc_media_player_get_length(_vlcMediaPlayer);
}

bool DYMediaPlayer::hasVout()
{
    unsigned b = libvlc_media_player_has_vout(_vlcMediaPlayer);
    return b > 0;
}

DYVlc::State DYMediaPlayer::state() const
{
    if(!_vlcMediaPlayer)  return DYVlc::Idle;

    if (!libvlc_media_player_get_media(_vlcMediaPlayer))
        return DYVlc::Idle;

    libvlc_state_t state;
    state = libvlc_media_player_get_state(_vlcMediaPlayer);
    return DYVlc::State(state);
}

DYMedia *DYMediaPlayer::media() const
{
    return _dyMedia;
}

DYVideo *DYMediaPlayer::video() const
{
    return _dyVideo;
}

DYAudio *DYMediaPlayer::audio() const
{
    return _dyAudio;
}

DYEqualizer *DYMediaPlayer::equalizer() const
{
    return _dyEqualizer;
}

DYMediaInfo *DYMediaPlayer::mediaInfo() const
{
    return _dyMediaInfo;
}

VLCCoreInteraction *DYMediaPlayer::rotate() const
{
    return m_pRotate;
}

void DYMediaPlayer::fastForward()
{
    if(length() <= 0)
        return;
    const float tp = 5000.0f / length();
    const float cp = libvlc_media_player_get_position(_vlcMediaPlayer);
    if(cp + tp <= 1.0f) {
        libvlc_media_player_set_position(_vlcMediaPlayer, cp + tp);
        emit positionChanged(cp + tp);
    }
    else
    {
        libvlc_media_player_set_position(_vlcMediaPlayer, 1.0f);
        emit positionChanged(1.0f);
    }
}

void DYMediaPlayer::fastRewind()
{
    if(length() <= 0)
        return;
    const float tp = 5000.0f / length();
    const float cp = libvlc_media_player_get_position(_vlcMediaPlayer);
    if(cp - tp >= 0.0f) {
        libvlc_media_player_set_position(_vlcMediaPlayer, cp - tp);
        emit positionChanged(cp - tp);
    }
    else {
        libvlc_media_player_set_position(_vlcMediaPlayer, 0.0f);
        emit positionChanged(0.0f);
    }
}

void DYMediaPlayer::parse()
{
    _dyMediaInfo->parse(core());
}

void DYMediaPlayer::play()
{
    if (!_vlcMediaPlayer)
        return;

    if (_dyVideoWidget) {
        _curWId = _dyVideoWidget->request();
    } else {
        _curWId = 0;
    }

    /* Get our media instance to use our window */
    if (_curWId) {
#if defined(Q_OS_WIN32)
        libvlc_media_player_set_hwnd(_vlcMediaPlayer, (void *)_curWId);
#elif defined(Q_OS_DARWIN)
        libvlc_media_player_set_nsobject(_digMediaPlayer, (void *)_curWId);
#elif defined(Q_OS_UNIX)
        libvlc_media_player_set_xwindow(_digMediaPlayer, _curWId);
#endif
    }

    libvlc_media_player_play(_vlcMediaPlayer);
}

void DYMediaPlayer::togglePause()
{
    if (!_vlcMediaPlayer)
        return;

    if (libvlc_media_player_can_pause(_vlcMediaPlayer))
        libvlc_media_player_pause(_vlcMediaPlayer);
}

void DYMediaPlayer::stop()
{
    if (!_vlcMediaPlayer)
        return;
    if(_curWId == 0)
        return;

    if (_dyVideoWidget)
        _dyVideoWidget->release();

    _curWId = 0;
#if 0
    const DYVlc::State &sta = this->state();
    if(sta == DYVlc::Playing || sta == DYVlc::Paused || sta == DYVlc::Buffering)
        libvlc_media_player_stop(_vlcMediaPlayer);
#else
    libvlc_media_player_stop(_vlcMediaPlayer);
#endif
}

void DYMediaPlayer::setPosition(int time)
{
    if(length() <= 0)
        return;

    const DYVlc::State &sta = this->state();

    if (!(sta == DYVlc::Buffering
          || sta == DYVlc::Playing
          || sta == DYVlc::Paused))
        return;

    float position = (float)time / length();
    libvlc_media_player_set_position(_vlcMediaPlayer, position);

    emit positionChanged(position);
}

bool DYMediaPlayer::playNext()
{
    MEDIAINFO m = LibData->getnextmedia(false);
    if(m.title.isEmpty())
        return false;
    if(LibData->b_isConCPT) {
        if(LibData->checkMediaisAirPlay(m)) {
            OpenFileDialog->setPlayType(OPenFile::CONNECTION);
            OpenFileDialog->sendNetworkInfo(m.filepath);
        } else {
            LibData->clearPlayinfo();
            OpenFileDialog->openMediaNotSupAirPlay(m.filepath);
        }
        return true;
    } else {
        if(m.mediaType == 1)
            OpenFileDialog->setPlayType(OPenFile::Video);
        else if(m.mediaType == 2)
            OpenFileDialog->setPlayType(OPenFile::Music);
        else return false;
        OpenFileDialog->playFile(m.filepath);
        LibData->addRecent(m.filepath);
        return true;
    }
}

bool DYMediaPlayer::playPre()
{
    MEDIAINFO m = LibData->getprevmedia(false);
    if(m.title.isEmpty())
        return false;
    if(LibData->b_isConCPT) {
        if(LibData->checkMediaisAirPlay(m)) {
            OpenFileDialog->setPlayType(OPenFile::CONNECTION);
            OpenFileDialog->sendNetworkInfo(m.filepath);
        } else {
            LibData->clearPlayinfo();
            OpenFileDialog->openMediaNotSupAirPlay(m.filepath);
        }
        return true;
    } else {
        if(m.mediaType == 1)
            OpenFileDialog->setPlayType(OPenFile::Video);
        else if(m.mediaType == 2)
            OpenFileDialog->setPlayType(OPenFile::Music);
        else return false;
        OpenFileDialog->playFile(m.filepath);
        LibData->addRecent(m.filepath);
        return true;
    }
}

void DYMediaPlayer::setVideoWidget(DYVideoFrame *widget)
{
    _dyVideoWidget = widget;
}

static void libvlc_callback(const libvlc_event_t *event, void *data)
{
    static bool isParsed = false;
    DYMediaPlayer *core = static_cast<DYMediaPlayer *>(data);
    switch (event->type) {
    case libvlc_MediaPlayerMediaChanged:
        isParsed = false;
        emit core->mediaChanged(event->u.media_player_media_changed.new_media);
        break;
    case libvlc_MediaPlayerNothingSpecial:
        emit core->nothingSpecial();
        break;
    case libvlc_MediaPlayerOpening:
        emit core->opening();
        break;
    case libvlc_MediaPlayerBuffering:
        emit core->buffering(event->u.media_player_buffering.new_cache);
        emit core->buffering(qRound(event->u.media_player_buffering.new_cache));
        break;
    case libvlc_MediaPlayerPlaying:
        emit core->playing();
        if(!isParsed)
        {
            isParsed = true;
            core->parse();
        }
        break;
    case libvlc_MediaPlayerPaused:
        emit core->paused();
        break;
    case libvlc_MediaPlayerStopped:
        emit core->stopped();
        break;
    case libvlc_MediaPlayerForward:
        emit core->forward();
        break;
    case libvlc_MediaPlayerBackward:
        emit core->backward();
        break;
    case libvlc_MediaPlayerEndReached:
        emit core->end();
        break;
    case libvlc_MediaPlayerEncounteredError:
        emit core->error();
        break;
    case libvlc_MediaPlayerTimeChanged:
        emit core->timeChanged(event->u.media_player_time_changed.new_time);
        break;
    case libvlc_MediaPlayerPositionChanged:
        emit core->positionChanged(event->u.media_player_position_changed.new_position);
        break;
    case libvlc_MediaPlayerSeekableChanged:
        emit core->seekableChanged(event->u.media_player_seekable_changed.new_seekable);
        break;
    case libvlc_MediaPlayerPausableChanged:
        emit core->pausableChanged(event->u.media_player_pausable_changed.new_pausable);
        break;
    case libvlc_MediaPlayerTitleChanged:
        emit core->titleChanged(event->u.media_player_title_changed.new_title);
        break;
    case libvlc_MediaPlayerSnapshotTaken:
        emit core->snapshotTaken(event->u.media_player_snapshot_taken.psz_filename);
        break;
    case libvlc_MediaPlayerLengthChanged:
        emit core->lengthChanged(event->u.media_player_length_changed.new_length);
        break;
    case libvlc_MediaPlayerVout: //视频
    {
        const int count = event->u.media_player_vout.new_count;
        if(count > 0) { // 0 播放结束
            emit core->vout(count);
        }
        break;
    }
    case libvlc_MediaPlayerMuted:
    {
        core->audio()->muteChanged(true);
        break;
    }
    case libvlc_MediaPlayerUnmuted:
    {
        core->audio()->muteChanged(false);
        break;
    }
    case libvlc_MediaPlayerAudioVolume:
    {
        const float volume = event->u.media_player_audio_volume.volume;
        core->audio()->volumeChanged(volume * 100);
        break;
    }
    default:
        break;
    }

    if (event->type >= libvlc_MediaPlayerNothingSpecial
            && event->type <= libvlc_MediaPlayerEncounteredError) {
        emit core->stateChanged();
    }
}

void DYMediaPlayer::vclConnections(bool isCreate)
{
    if(_vlcMediaPlayer == NULL) return;
    QList<libvlc_event_e> list;
    list << libvlc_MediaPlayerMediaChanged
         << libvlc_MediaPlayerNothingSpecial
         << libvlc_MediaPlayerOpening
         << libvlc_MediaPlayerBuffering
         << libvlc_MediaPlayerPlaying
         << libvlc_MediaPlayerPaused
         << libvlc_MediaPlayerStopped
         << libvlc_MediaPlayerForward
         << libvlc_MediaPlayerBackward
         << libvlc_MediaPlayerEndReached
         << libvlc_MediaPlayerEncounteredError
         << libvlc_MediaPlayerTimeChanged
         << libvlc_MediaPlayerPositionChanged
         << libvlc_MediaPlayerSeekableChanged
         << libvlc_MediaPlayerPausableChanged
         << libvlc_MediaPlayerTitleChanged
         << libvlc_MediaPlayerSnapshotTaken
         << libvlc_MediaPlayerLengthChanged
         << libvlc_MediaPlayerVout
         << libvlc_MediaPlayerMuted
         << libvlc_MediaPlayerUnmuted
         << libvlc_MediaPlayerAudioVolume;

    libvlc_event_manager_t *eventManager = libvlc_media_player_event_manager(_vlcMediaPlayer);

    foreach (const libvlc_event_e &event, list)
    {
        if(isCreate)
            libvlc_event_attach(eventManager, event, libvlc_callback, this);
        else
            libvlc_event_detach(eventManager, event, libvlc_callback, this);
    }
}
