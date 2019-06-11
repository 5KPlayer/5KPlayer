#ifndef DYENUMS_H
#define DYENUMS_H
/*!
 * \brief The DYVlc class
 * 一些相关的枚举
 */
#include <QObject>

class DYVlc : public QObject
{
    Q_OBJECT

    Q_ENUMS(LogLevel)
public:
    /*!
        \brief Dummy enums constructor.

        This constructor creates a new dummy enums object.

        \param parent object parent (QObject *)
    */
    explicit DYVlc(QObject *parent = 0);
    // Enums
    /*!
        \enum LogLevel
        \brief libVLC log levels
        \Log等级
    */
    enum LogLevel {
        DebugLevel = 0,
        NoticeLevel = 2,
        WarningLevel = 3,
        ErrorLevel = 4,
        DisabledLevel = 5
    };
    /*!
        \enum ActionsType
        \brief Actions types identifiers
        动作类型标示
    */
    enum ActionsType {
        AudioTrack,
        Subtitles,
        VideoTrack,
        Other
    };
    /*!
        \enum AudioChannel
        \brief Audio channel options
        声道
    */
    enum AudioChannel {
        AudioChannelError = -1,
        Stereo = 1,
        RStereo = 2,
        Left = 3,
        Right = 4,
        Dolbys = 5
    };
    /*!
        \enum PlaybackMode
        \brief Playlist playback mode
        播放列表播放模式
    */
    enum PlaybackMode {
        DefaultPlayback,
        Loop,
        Repeat
    };
    /*!
        \enum State
        \brief VLC-Qt playback states
        媒体状态
    */
    enum State {
        Idle,
        Opening,
        Buffering,
        Playing,
        Paused,
        Stopped,
        Ended,
        Error
    };

    /*!
        \enum Scale
        \brief Supported scales
        缩放比例
    */
    enum Scale {
        NoScale,
        S_1_05,
        S_1_1,
        S_1_2,
        S_1_3,
        S_1_4,
        S_1_5,
        S_1_6,
        S_1_7,
        S_1_8,
        S_1_9,
        S_2_0
    };
    enum video_orientation_t
    {
        ORIENT_TOP_LEFT = 0, /**< Top line represents top, left column left. */
        ORIENT_TOP_RIGHT, /**< Flipped horizontally */
        ORIENT_BOTTOM_LEFT, /**< Flipped vertically */
        ORIENT_BOTTOM_RIGHT, /**< Rotated 180 degrees */
        ORIENT_LEFT_TOP, /**< Transposed */
        ORIENT_LEFT_BOTTOM, /**< Rotated 90 degrees clockwise */
        ORIENT_RIGHT_TOP, /**< Rotated 90 degrees anti-clockwise */
        ORIENT_RIGHT_BOTTOM, /**< Anti-transposed */

        ORIENT_NORMAL      = ORIENT_TOP_LEFT,
        ORIENT_TRANSPOSED  = ORIENT_LEFT_TOP,
        ORIENT_ANTI_TRANSPOSED = ORIENT_RIGHT_BOTTOM,
        ORIENT_HFLIPPED    = ORIENT_TOP_RIGHT,
        ORIENT_VFLIPPED    = ORIENT_BOTTOM_LEFT,
        ORIENT_ROTATED_180 = ORIENT_BOTTOM_RIGHT,
        ORIENT_ROTATED_270 = ORIENT_LEFT_BOTTOM,
        ORIENT_ROTATED_90  = ORIENT_RIGHT_TOP,
    } video_orientation_t;
    // 视频翻转
    enum video_transform_t
    {
        TRANSFORM_IDENTITY       = ORIENT_NORMAL,
        TRANSFORM_HFLIP          = ORIENT_HFLIPPED,
        TRANSFORM_VFLIP          = ORIENT_VFLIPPED,
        TRANSFORM_R90            = ORIENT_ROTATED_90,
        TRANSFORM_R180           = ORIENT_ROTATED_180,
        TRANSFORM_R270           = ORIENT_ROTATED_270,
        TRANSFORM_TRANSPOSE      = ORIENT_TRANSPOSED,
        TRANSFORM_ANTI_TRANSPOSE = ORIENT_ANTI_TRANSPOSED
    };

    /*!
        \enum VideoCodec
        \brief Video codecs list
    */
    enum VideoCodec {
        NoVideo,
        MPEG2Video,
        MPEG4Video,
        H264,
        Theora
    };

    /*!
        \enum VideoOutput
        \brief Video outputs list
    */
    enum VideoOutput {
#if defined(Q_OS_LINUX)
        X11,
        XVideo,
        GLX,
#elif defined(Q_OS_WIN32)
        DirectX,
        Direct3D,
        OpenGL,
#endif
        DefaultVout
    };

    /*!
        \enum Meta
        \brief Supported meta types

    */
    enum Meta {
        Title,
        Artist,
        Genre,
        Copyright,
        Album,
        TrackNumber,
        Description,
        Rating,
        Date,
        Setting,
        URL,
        Language,
        NowPlaying,
        Publisher,
        EncodedBy,
        ArtworkURL,
        TrackID
    };
    /*!
        \enum Mux
        \brief Recording output file mux
    */
    enum Mux {
        TS,
        PS,
        MP4,
        OGG,
        AVI
    };
    /*!
        \enum AudioCodec
        \brief Audio codecs list
    */
    enum AudioCodec {
        NoAudio,
        MPEG2Audio,
        MP3,
        MPEG4Audio,
        Vorbis,
        Flac
    };
    /*!
        \enum AudioOutput
        \brief Audio outputs list
    */
    enum AudioOutput {
        DefaultAout
    };

    /*!
        \enum Deinterlacing
        \brief Supported deinterlacing modes
    */
    enum Deinterlacing {
        Disabled,
        Discard,
        Blend,
        Mean,
        Bob,
        Linear,
        X,
        Yadif,
        Yadif2x,
        Phospor,
        IVTC
    };

    /*!
        \enum FillMode
        \brief Supported fill modes (QML only)
    */
    enum FillMode {
        PreserveAspectFit = Qt::KeepAspectRatio,
        PreserveAspectCrop = Qt::KeepAspectRatioByExpanding,
        Stretch = Qt::IgnoreAspectRatio
    };
    /*!
        \brief Scale floats
        \return scale floats (QList<float>)
    */
    static QList<float> scale();
    /*!
        \brief Aspect and crop ratios strings
        \return ratios strings (QStringList)
    */
    static QStringList ratio();
    /*!
     * \brief transform strings
     * \return transform strings
     */
    static QStringList transform();
    /*!
        \brief Deinterlacing modes strings
        \return deinterlacing strings (QStringList)
    */
    static QStringList deinterlacing();
};

#endif // DYENUMS_H
