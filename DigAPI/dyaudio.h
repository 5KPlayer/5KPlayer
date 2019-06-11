#ifndef DYAUDIO_H
#define DYAUDIO_H
#include <QObject>
#include "dyenums.h"

class DYMediaPlayer;

class DYAudio : public QObject
{
    Q_OBJECT
public:
    explicit DYAudio(DYMediaPlayer *player);
    ~DYAudio();
public:
    /*!
        \brief Get current mute state.
        \return current mute state (const bool)
        是否静音
    */
    bool isMute() const;
    /*!
        \brief Get current audio track.
        \return the number of current audio track, or -1 if none (const int)
        当前的音频轨道
    */
    int track() const;
    /*!
     * \brief getaudioTrackCount
     * \return 获取音频数量
     */
    int trackCount() const;
    /*!
        \brief Get available audio tracks.
        \return dictionary with available audio tracks (const QMap<int, QString>)
        音频轨道信息
    */
    QMap<int, QString> tracks() const;
    /*!
     * \brief channel
     * \return 当前的声道信息
     */
    DYVlc::AudioChannel channel() const;

    int getDelay();
    void setDelay(const int &);

    int sliderValue() const {return i_sliderValue;}
    int realVolume() const;

public slots:
    /*!
     * \brief setVolums
     * \param vol
     * 设置音量
     */
    void setVolums(int vol);
    /*!
     * \brief setVolunMute
     * \param state
     * 切换静音
     */
    void toggleVolunMute();
    /*!
        \brief Set current audio track.
        \param track new audio track (int)
        设置音轨
    */
    void setTrack(int track);
    /*!
        \brief Set audio channel.
        \param channel new audio channel
        设置声道
    */
    void setChannel(DYVlc::AudioChannel channel);
    void setChannel(const int &channel);

    //增加音量
    void addVolums();
    //减小音量
    void decVolums();

signals:
    /*!
        \brief Signal sent when volume has changed.
        \param volume new volume (float)
    */
    void volumeChangedF(float volume);

    /*!
        \brief Signal sent when volume has changed.
        \param volume new volume (int)
    */
    void volumeChanged(int volume);

    /*!
        \brief Signal sent when mute has changed.
        \param mute new mute state (bool)
    */
    void muteChanged(bool mute);


private:
    //数值 -> vlc的音量
    int valueToVolue(int value) const;
    //vlc的音量 -> 数值
    int volumeToValue() const;

private:
    DYMediaPlayer *m_pParent;
    int i_sliderValue;//当前滑块的值
};

#endif // DYAUDIO_H
