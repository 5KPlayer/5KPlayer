#ifndef DYMEDIAPLAYER_H
#define DYMEDIAPLAYER_H

#include <QObject>
#include <qwindowdefs.h>
#include "dyenums.h"
#include "singleton.h"

#include "dyvideo.h"
#include "dyaudio.h"
#include "dymedia.h"
#include "dymediainfo.h"
#include "dyequalizer.h"
#include "dyinstance.h"
#include "dyvideoframe.h"
#include "vlccoreinteraction.h"

struct libvlc_event_t;
struct libvlc_media_t;
struct libvlc_media_player_t;

#define MediaPlayer DYMediaPlayer::getInstance()

class DYMediaPlayer : public QObject, public Singleton<DYMediaPlayer>
{
    Q_OBJECT
    friend class Singleton<DYMediaPlayer>;
    friend class QSharedPointer<DYMediaPlayer>;

private:
    Q_DISABLE_COPY(DYMediaPlayer)
    explicit DYMediaPlayer(QObject *p = NULL);
    ~DYMediaPlayer();

public:
    /*!
        \brief Returns libvlc media player object.
        \return libvlc media player (libvlc_media_player_t *)
    */
    libvlc_media_player_t *core() const;
    /*! \brief Set video widget.

        Set video widget to be able to play video inside Qt interface.

        \param widget video widget (VlcVideoDelegate *)
        设置播放器显示面板
    */
    void setVideoWidget(DYVideoFrame *widget);

    /*!
        \brief Get the current movie length (in ms).
        \return the movie length (in ms), or -1 if there is no media (const int)
        得到当前媒体的时长
    */
    int length() const;

    bool hasVout();

    /*!
        \brief Get current player state.
        \return current player state (const DYVlc::State)
        得到当前的状态
    */
    DYVlc::State state() const;

    void playFile(const QString &file, const bool &isFile = true);

    DYMedia* media() const;
    DYVideo* video() const;
    DYAudio* audio() const;
    DYEqualizer *equalizer() const;
    DYMediaInfo *mediaInfo() const;
    VLCCoreInteraction *rotate() const;

    void fastForward(); //快进
    void fastRewind();  //快退

    void parse();

public slots:
    /*!
        \brief Toggles pause of the playback of current media if possible
        暂停/播放
    */
    void togglePause();
    /*!
        \brief Stops playing current media
        停止
    */
    void stop();
    /*! \brief Set the movie time (in ms).

        This has no effect if no media is being played. Not all formats and protocols support this.

        \param time the movie time (in ms) (int)
        设置播放时间
    */
    void setPosition(int time);

    bool playNext();
    bool playPre();

signals:
    /*!
        \brief Signal sent on backward
    */
    void backward();

    /*!
        \brief Signal sent on buffering
        \param buffer buffer status in percent
    */
    void buffering(float buffer);

    /*!
        \brief Signal sent on buffering
        \param buffer buffer status in percent
    */
    void buffering(int buffer);

    /*!
        \brief Signal sent when end reached
    */
    void end();

    /*!
        \brief Signal sent on error
    */
    void error();

    /*!
        \brief Signal sent on forward
    */
    void forward();

    /*!
        \brief Signal sent on length change
        \param length new length
    */
    void lengthChanged(int length);

    /*!
        \brief Signal sent on media change
        \param media new media object
    */
    void mediaChanged(libvlc_media_t *media);

    /*!
        \brief Signal sent nothing special happened
    */
    void nothingSpecial();

    /*!
        \brief Signal sent when opening
    */
    void opening();

    /*!
        \brief Signal sent on pausable change
        \param pausable pausable status
    */
    void pausableChanged(bool pausable);

    /*!
        \brief Signal sent when paused
    */
    void paused();

    /*!
        \brief Signal sent when playing
    */
    void playing();

    /*!
        \brief Signal sent on position change
        \param position new position
    */
    void positionChanged(float position);

    /*!
        \brief Signal sent on seekable change
        \param seekable seekable status
    */
    void seekableChanged(bool seekable);

    /*!
        \brief Signal sent on snapshot taken
        \param filename filename of the snapshot
    */
    void snapshotTaken(const QString &filename);

    /*!
        \brief Signal sent when stopped
    */
    void stopped();

    /*!
        \brief Signal sent on time change
        \param time new time
    */
    void timeChanged(int time);

    /*!
        \brief Signal sent on title change
        \param title new title
    */
    void titleChanged(int title);

    /*!
        \brief Signal sent when video output is available
        \param count number of video outputs available
    */
    void vout(int count);

    /*!
        \brief Signal sent when state of the player changed
    */
    void stateChanged();

private:
    void vclConnections(bool isCreate = true);

    /*!
        \brief Starts playing current media if possible
        播放
    */
    void play();

private:
    void deinit();

    libvlc_media_player_t * _vlcMediaPlayer;

    DYMedia *_dyMedia;
    DYVideo *_dyVideo;
    DYAudio *_dyAudio;
    DYMediaInfo *_dyMediaInfo;
    DYEqualizer *_dyEqualizer;
    VLCCoreInteraction *m_pRotate;

    DYVideoFrame* _dyVideoWidget;
    WId _curWId;

    DYInstance *_instance;
};

#endif // DYMEDIAPLAYER_H
