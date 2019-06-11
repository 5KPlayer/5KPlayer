#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QObject>
#include "singleton.h"

class QSettings;

#define Config Configure::getInstance()

class Configure : public QObject, public Singleton<Configure>
{
    Q_OBJECT
    friend class Singleton<Configure>;
    friend class QSharedPointer<Configure>;

public:
    QString getUserVideoFormat();
    QString getUserAudioFormat();

    //返回固定的格式
    QString getFixedVideoFormat() const {return _videoFormat;}
    QString getFixedAudioFormat() const {return _audioFormat;}

    //返回可变的格式
    QString getVarVideoFormat() const {return _localSaveVideoFormat;}
    QString getVarAudioFormat() const {return _localSaveAudioFormat;}

    void    saveUserVideoFormat();
    void    saveUserAudioFormat();

    void    addVideoFormat(const QString &videoFormat);
    void    addAudioFormat(const QString &audioFormat);

    QVariant getConfigVar(const QString &Key);
    void     setConfigVar(const QString &Key, const QVariant &value);

private:
    Q_DISABLE_COPY(Configure)
    explicit Configure(QObject *parent = 0);

    QString getConfigPath() const;

    QSettings *p_set;

    //可变的
    QString _localSaveVideoFormat;
    QString _localSaveAudioFormat;

    //固定的
    const QString _videoFormat = "*.3gp *.asf  *.avi *.dat  *.divx *.f4v *.flv  *.h264 "
                                 "*.m2t *.m2ts *.m4v *.mkv  *.mov  *.mp4 *.mpeg *.mpg "
                                 "*.mts *.ogm  *.rm  *.rmvb *.ogv  *.ts  *.tp   *.vc1 "
                                 "*.vob *.webm *.wmv *.xvid *.yuv";

    const QString _audioFormat = "*.aac *.ac3 *.ape *.flac  *.m4a *.m4b *.mid *.midi "
                                 "*.mka *.mp3 *.mpa *.ogg   *.pcm *.ra  *.ram *.wav "
                                 "*.wma";
};

#endif // CONFIGURE_H
