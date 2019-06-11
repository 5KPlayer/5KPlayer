#ifndef AIRPLAY_H
#define AIRPLAY_H

//接口地址宏定义
#define FStartServer						"_DF631D50C5927A88CD641CAE6F603FC3"
#define FStopServer   					 	"_F356348146176B56F432CD94CDEBF3B2"
#define FSetProgress 						"_C482149CA08E45F8BF5010B1AC6960F5"
#define FSendEvent 							"_FCF774B929BDC4FA9E272F69CD858FC6"
#define FSetNoBonjourServerFunc 			"_AF3B473FF6124946657AA99B7DF13D74"
#define FSetLoadingFunc 					"_BC42715D686509F0945F0633D9012540"
#define FSetLogFunc 						"_B6CD2EB653C429022A9F7E6F9A466C1C"
#define FSetStartAirPlayFunc 				"_5c3fc1c8d75b37936894e3a47d5a0fd2"
#define FSetVideoPlayFunc 					"_54D6A53FA7DD96284069DD87E4F55FF7"
#define FSetVideoControlFunc 				"_E6701C7BEA72C84AFC05157C33489A76"
#define FSetVideoSeekFunc 					"_184E4F22F62DA06E412616DC50A10273"
#define FSetVideoProgressFunc 				"_EC5B1BF9A861C0B35DD201D6541339F1"
#define FSetAudioPlayFunc 					"_C6D76D6982045590A57D8717EFD9948A"
#define FSetAudioImageFunc 					"_47D7CB1125C540455A896EAF6B7F001D"
#define FSetAudioMetadataFunc 				"_AEA0D6223D7BC4342438ABDA983AD5E9"
#define FSetAudioDestroyFunc 				"_6DE6F92454DAE6362B591A3B857DE11D"
#define FSetAudioVolumeFunc 				"_3A708EC545EE73B362CD53A076993A8A"
#define FSetAudioPauseFunc 					"_2258333E8DA70C1FBC32DF4368708078"
#define FSetAudioStartFunc 					"_E2CBC9AEE1647237B9D7FA85BDF37FB8"
#define FCloseRaop 							"_43ad05e794d962cd1df6aa77ecaed8b1"
#define FSetFindDacpServerNameFunc 			"_0E2D6413BBB43FB6ADF11BAB75A15E17"
#define FSetSystemVolume 					"_C04F9BAD3F28220DF9EAD45E6AD70A0A"
#define FGetSystemVolume 					"_53352F6B55B5BD5ED8B077354A7FBB23"
#define FSetSavePath 						"_883534D48FC118F174BB3D26209EBC3E"
#define FSetScreenMirrorRecordFinishFunc 	"_64929C52C77A1F4EE3445112059922AB"
#define FSetScreenMirrorStartFunc 			"_BE7032C3D32E7FACB2DDEF90FB8F0527"
//#define FLoadConfig 						"_612E4EC3A9CA791E4F2B2A3C0779FE60"
#define FDaapChunk                          "_F42D8778C850D4372E15BE68B884C3CF"
#define FFreeMp3file                        "_1558FA40F496EE9257F08EF6CE1EE5C2"

typedef struct tag_mp3file {
    char *path;
    char *fname;
    char *title;     /* TIT2 */
    char *artist;    /* TPE1 */
    char *album;     /* TALB */
    char *genre;     /* TCON */
    char *comment;   /* COMM */
    char *type;
    char *composer;  /* TCOM */
    char *orchestra; /* TPE2 */
    char *conductor; /* TPE3 */
    char *grouping;  /* TIT1 */
    char *url;       /* daap.songdataurl (asul) */

    int bitrate;
    int samplerate;
    int song_length;
    int file_size;
    int year;        /* TDRC */

    int track;       /* TRCK */
    int total_tracks;

    int disc;        /* TPOS */
    int total_discs;

    int time_added;
    int time_modified;
    int time_played;
    int db_timestamp;

    int bpm;         /* TBPM */

    int got_id3;
    //    unsigned int id;
    unsigned long int	id;
    /* generated fields */
    char* description;		/* long file type */
    int item_kind;		/* song or movie */
    int data_kind;              /* dmap.datakind (asdk) */

    char compilation;
} MP3FILE;

//Airplay播放状态
enum VideoControlCMD
{
    PLAY,
    STOP,
    PAUSE,
};

//Ariplay
typedef int (*f_StartServer)(const char* serverName, const char* macAdd, int port);
typedef void (*f_StopServer)();

typedef void (*f_SetProgress)(double progress, double duration);
typedef void (*f_SetSavePath)(const wchar_t* path);
typedef void (*f_CloseRaop)();
typedef int (*f_SendEvent)(const char* event);

typedef void (*f_NoBonjourServerFunc)();
typedef void (*f_SetNoBonjourServerFunc)(f_NoBonjourServerFunc);

typedef void (*f_FindDacpServerNameFunc)(const char* dacpID, const char* activeRemote);
typedef void (*f_SetFindDacpServerNameFunc)(f_FindDacpServerNameFunc);

//Log
typedef void (*f_LogFunc)(const char* log);
typedef void (*f_SetLogFunc)(f_LogFunc);

//state = 0 表示缓冲结束 1 表示正在缓冲数据
typedef void (*f_LoadingFunc)(int state);
typedef void (*f_SetLoadingFunc)(f_LoadingFunc);

//Video
typedef void (*f_StartAirPlay)();
typedef void (*f_SetStartAirPlayFunc)(f_StartAirPlay);

typedef void (*f_VideoPlay)(const char* url, double startPosition);
typedef void (*f_SetVideoPlayFunc)(f_VideoPlay);

typedef void (*f_VideoControl)(VideoControlCMD cmd);
typedef void (*f_SetVideoControlFunc)(f_VideoControl);

typedef void (*f_VideoSeek)(double position);
typedef void (*f_SetVideoSeekFunc)(f_VideoSeek);

typedef void (*f_VideoProgress)(double* duration, double* cachePosition,
                                double* position, int* playing);
typedef void (*f_SetVideoProgressFunc)(f_VideoProgress);

//Audio
typedef void (*f_SetAudioPlay)(int start, int end);
typedef void (*f_SetAudioPlayFunc)(f_SetAudioPlay);

typedef void (*f_AudioImage)(const char* data, int datalen);
typedef void (*f_SetAudioImageFunc)(f_AudioImage);

typedef void (*f_AudioMetadata)(const char* data, int datalen);
typedef void (*f_SetAudioMetadataFunc)(f_AudioMetadata);

typedef void (*f_AudioDestroy)();
typedef void (*f_SetAudioDestroyFunc)(f_AudioDestroy);

typedef void (*f_AudioVolume)(float volume);
typedef void (*f_SetAudioVolumeFunc)(f_AudioVolume);

typedef void (*f_AudioPause)();
typedef void (*f_SetAudioPauseFunc)(f_AudioPause);

typedef void (*f_AudioStart)();
typedef void (*f_SetAudioStartFunc)(f_AudioStart);

//volume
typedef void (*f_SetSystemVolume)(float volume);
typedef float (*f_GetSystemVolume)(void);


//ScreenMirror
typedef void (*f_SetScreenMirrorStart)(int state);
typedef void (*f_SetScreenMirrorStartFunc)(f_SetScreenMirrorStart);

typedef void (*f_SetScreenMirrorRecordFinish)(const char* filename);
typedef void (*f_SetScreenMirrorRecordFinishFunc)(f_SetScreenMirrorRecordFinish);

//解析音频meta
typedef MP3FILE* (*f_daap_chunk)(const void* data, unsigned int length);
typedef void (*f_free_mp3file)(MP3FILE*);

#include <QObject>

class QLibrary;

class AirPlay : public QObject
{
    Q_OBJECT
public:
    AirPlay(const QString &pcName, const QString &macAddr,
            const uint &port=8000, QObject *parent = Q_NULLPTR);
    ~AirPlay();

    bool airplayStarted();//airplay是否启动
    bool startAirPlay();//启动airplay服务
    void stopAirPlay(); //断开链接
    void stopAirPlayServer();//关闭airplay服务

    void setMirrorRecordPath(const QString &path);//设置录屏文件存放的路径

    QString getAudioInfo(const void* data, unsigned int length) const;

    //nextitem   previtem   playpause   stop   mutetoggle
    void sendMusicControl(const uint &type);

    //loading,   playing,   paused,     stopped
    void sendVideoControl(const uint &type);

    QString _dacpID;
    QString _activeRemote;
    bool    _noBonjourServer;

signals:
    void signal_loading(const int &);

    void signal_startAirPlay();
    void signal_videoPlay(const QString &);
    void signal_videoControl(const QString &);
    void signal_videoSeek(const double &);
    void signal_videoProgress(const double &, const double &, const double &, const int &);

    void signal_audioStart();
    void signal_audioPlay(const int &start, const int &end);
    void signal_audioPause();
    void signal_audioVolume(const int &);
    void signal_audioImage(const QByteArray &data);
    void signal_audioMetadata(const QString &);
    void signal_audioDestroy();

    void signal_screenRecordFinish(const QString &);
    void signal_screenMirrorStart(int);

private:
    void setFuncNULL();
    bool findDacpInfo(QString &ip, quint16 &port) const;

private:
    QString _pcName;
    QString _macAddr;
    uint    _iport;

    QLibrary *p_lib;

    //Airplay接口
    f_StartServer                StartServer;
    f_StopServer                 StopServer;
    f_SetProgress                SetProgress;
    f_SetSavePath                SetSavePath;
    f_SetLoadingFunc             SetLoadingFunc;
    f_SetNoBonjourServerFunc     SetNoBonjourServerFunc;
    f_SetFindDacpServerNameFunc  SetFindDacpServerNameFunc;

    //Video接口
    f_SetStartAirPlayFunc  SetStartAirPlayFunc;
    f_SetVideoPlayFunc     SetVideoPlayFunc;
    f_SetVideoControlFunc  SetVideoControlFunc;
    f_SetVideoSeekFunc     SetVideoSeekFunc;
    f_SetVideoProgressFunc SetVideoProgressFunc;

    //Audio接口
    f_SetAudioPlayFunc     SetAudioPlayFunc;
    f_SetAudioImageFunc    SetAudioImageFunc;
    f_SetAudioMetadataFunc SetAudioMetadataFunc;
    f_SetAudioDestroyFunc  SetAudioDestroyFunc;

    f_SetAudioVolumeFunc SetAudioVolumeFunc;
    f_SetAudioPauseFunc  SetAudioPauseFunc;
    f_SetAudioStartFunc  SetAudioStartFunc;

    //解析audio 的 meta信息
    f_daap_chunk   daap_chunk;
    f_free_mp3file free_mp3file;

    f_SendEvent  SendEvent;
    //f_SetLogFunc SetLogFunc;
    f_CloseRaop  CloseRaop;
    //f_SetSystemVolume SetSystemVolume;
    //f_GetSystemVolume GetSystemVolume;

    //ScreenMirro接口
    f_SetScreenMirrorRecordFinishFunc SetScreenMirrorRecordFinishFunc;
    f_SetScreenMirrorStartFunc        SetScreenMirrorStartFunc;
};

#endif // AIRPLAY_H
