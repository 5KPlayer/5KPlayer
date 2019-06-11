#ifndef DYSTRUCTU_H
#define DYSTRUCTU_H
#include <QSize>
#include <QByteArray>
#include <QList>
#include <QFont>
#include <QPoint>
#include <QRect>
typedef struct STREAMINFO
{
    QString     streamId;       // 流id或者url视频的formatid
    int64_t     streamType;     // 0视频流，1音频流，2字幕
    int64_t     bitRate;        // 比特率
    QString     ext;            // 后缀名格式
    QString     codec;          // 编/解码器
    QString     language;       // 语言
    QString     description;
    double      fileSize;       //文件大小
    QSize       resolution;     // 分辨率
    QString     aspectRatio;    // 宽高比
    int64_t     rateDen;
    int64_t     rateNum;
    int64_t     Channel;
    int64_t     rate;
    QString     Encoding;
    STREAMINFO() {
        streamId = "";
        streamType = -1;
        bitRate = 0;
        ext   = "";
        codec = "";
        language = "";
        description = "";
        fileSize    = 0;
        resolution = QSize(0, 0);
        aspectRatio = "";
        rateDen = 0;
        rateNum = 0;
        Channel = 0;
        rate = 0;
        Encoding = "";
    }

}_StreamInfo;

typedef struct MEDIAINFO
{
    QString             pin;
    QString             itemname;       // 所属文件夹
    QString             title;          // 文件标题
    QString             ext;            // 文件后缀名
    QString             album;          // 专辑
    QString             artist;         // 艺术家
    QList<_StreamInfo>  streamList;     // 文件详细信息
    QString             filepath;       // 文件路径
    QByteArray          screenshot;     // 文件截图路径
    int64_t             rotate;         // 文件旋转
    int64_t             duration;       // 文件时长
    double              filesize;       // 文件大小
    int64_t             fileAddTime;    // 文件添加时间
    QString             fileSoure;      // 文件源 本地/网络
    int64_t             mediaType;      // 1: video 2: music
    QString             webUrl;         // 网络链接
    QString             videoinfojson;
    QString             formatID;
    QStringList         subtitleList;   // 字幕
    bool                isFavorites;    // 收藏标记
    bool                isSelect;       // 选中标记

    MEDIAINFO()
    {
        pin = "";
        itemname = "";
        title    = "";
        ext      = "";
        album = "";
        artist = "";
        streamList.clear();
        filepath = "";
        screenshot.clear();
        rotate   = 0;
        duration = 0;
        filesize = 0;
        fileAddTime = 0;
        fileSoure= "";
        mediaType= 0;
        webUrl   = "";
        videoinfojson="";
        formatID = "";
        subtitleList.clear();
        isFavorites = false;
        isSelect = false;
    }
}_MEDIAINFO;

// url媒体信息中选中的字幕信息
typedef struct URLMEDIADATA{
    MEDIAINFO       mediainfo;
    QStringList     checkSubtitle;
}_UrlMediaData;

typedef struct DOWNURL
{
    MEDIAINFO downmedia;
    QString   downfile;
    bool      audioFlag;
    double    downProcess;
    bool      error;
    DOWNURL() {
        downfile    = "";
        audioFlag   = false;
        downProcess = 0.0;
        error       = false;
    }
} _downUrl;

typedef struct ANALYURL
{
    QString     analyzePath;
    bool        error;
    QByteArray  jsonByte;
    ANALYURL() {
        analyzePath = "";
        error    = false;
        jsonByte.clear();
    }
} _analyUrl;

typedef struct CONVERTMEDIA
{
    MEDIAINFO media;
    QString   outfilePath;
    QString   conFormat;
    double    proValue;
    CONVERTMEDIA() {
        outfilePath = "";
        conFormat   = "";
        proValue    = 0.0;
    }
} _convertMedia;


typedef struct HWCODECSUP
{
    QString codec;
    int     supHw;  // 1支持，0不支持
}_hwSup;

typedef struct GPUHWINFO
{
    QString GpuName;
    QList<_hwSup> supHWcodec;
} _GpuHWInfo;
typedef struct CPUSWINFO
{
    QString CpuName;
    QString CpuFreq;
    QList<QString> supInstruction;   // 指令集
} _CpuSWInfo;

struct NetInfo{
    QString ipAddr;      //IP地址
    quint16 port;        //端口号
    QString machineName; //机器名称
    NetInfo() {
        ipAddr      = "";
        port        = 0;
        machineName = "";
    }
};
struct AnalysisArgs {
    QString url;
    QString https;
    QString appPath;
    AnalysisArgs() {
        url         = "";
        https       = "";
        appPath     = "";
    }
};

struct DownloadArgs {
    MEDIAINFO   downInfo;
    QString     https;
    QStringList sub;
    QString     appPath;
    QString     ffmpegPath;
    QString     outputPath;
    DownloadArgs() {
        https       = "";
        sub.clear();
        appPath     = "";
        ffmpegPath  = "";
        outputPath  = "";
    }
};


// 图片
struct IcoStruct{
    QString     _fileName;  // 图片名字
    QPoint      _icoPos;    // 图片的位置
    QSize       _icoSize;   // 图片的大小
    QString     _icoUrl;    // 图片跳转的Url
    IcoStruct() {
        _fileName   = "";
        _icoPos.setX(0);
        _icoPos.setY(0);
        _icoSize.setWidth(0);
        _icoSize.setHeight(0);
        _icoUrl     = "";
    }
};
// 文字
struct TextStruct{
    QString     _textGuid;  // 文字编号
    QString     _text;      // 文字内容
    QString     _textColor; // 文字颜色
    QFont       _font;      // 文字字体
    QString     _textUrl;   // 跳转的Url
    QPoint      _textPos;   // 文字位置
    TextStruct() {
        _textGuid   = "";
        _text       = "";
        _textColor  = "#000000";
        _textUrl    = "";
        _textPos.setX(0);
        _textPos.setY(0);
        _font.setBold(false);
        _font.setItalic(false);
        _font.setStrikeOut(false);
        _font.setUnderline(false);
        _font.setPointSize(12);
        _font.setFamily("Tahoma");
    }
};
// 热点区域 带有资源图片时，可以理解成一个按钮
struct HotspotStruct {
    QString     _hotGuid;       // 热点的编码
    QRect       _hotRect;       // 热点的范围
    QString     _hotspotUrl;    // 热点指向的url
    QString     _hotspotNorRes; // 热点显示的资源图片
    QString     _hotspotEnRes;  // 热点鼠标进入时的资源图片
    HotspotStruct() {
        _hotGuid       = "";
        _hotspotNorRes = "";
        _hotspotEnRes  = "";
        _hotspotUrl = "";
    }
};

//广告结构体
struct AdvertStruct {
    QSize                   _adWinSize;     // 广告窗口大小
    bool                    _adSizeAuto;    // 用于广告编辑器，是否自适应大小
    QString                 _adBackColor;   // 广告窗口背景色
    QString                 _adNoshowColor; // 广告左下角不再显示文字颜色
    QString                 _adNoshowText;  // 广告左下角不再显示文字内容
    QString                 _adWinKey;      // 广告关键字
    QString                 _adLanguage;    // 广告语言
    QString                 _adType;        // 广告类型
    bool                    _adCloseHide;   // 广告界面关闭按钮是否显示
    QString                 _adShowType;    // 广告弹出的方式,BtoTop、RtoLeft
    int                     _adShowTime;    // 显示时间 -1为一直显示
    QString                 _adStartDate;   // 广告开始时间
    QString                 _adEndDate;     // 广告开始时间
    IcoStruct               _adBackIco;     // 图片
    QList<TextStruct>       _adTexts;       // 广告文字列表
    QList<HotspotStruct*>   _adHotspots;    // 热点列表
    AdvertStruct() {
        _adWinSize.setHeight(0);
        _adWinSize.setWidth(0);
        _adBackColor    = "#515171";
        _adNoshowColor  = "#ffffff";
        _adNoshowText   = "";
        _adWinKey       = "";
        _adShowTime     = -1;
        _adStartDate    = "2017-1-1";
        _adEndDate     = "2017-1-1";
    }
};

struct adFile{
    QString     url;
    QString     type;
    int         number;
    int         prob;
    QString     language;
    QStringList area;    // 地区
    adFile() {
        url     = "";
        type    = "";
        number  = 100000;
        prob    = 0;
        language= "";
        area.clear();
    }
};

struct adWebFile {
    int interval;
    QString nextUrl;
    bool noadFlag;
    QList<adFile> adfileList;
    adWebFile() {
        interval = 1;
        nextUrl  = "";
        noadFlag = false;
        adfileList.clear();
    }
};

struct adsDvd {
    int interval;   // 时间间隔
};

struct adsiPhone {
    int interval;   // 时间间隔
};

struct convertFormat {
    QString mediaFormat;    // 视频格式
    QString videoFormat;    // 视频流格式
    QString audioFormat;    // 音频流格式
};

struct adsConvert {
    QList<convertFormat>    conMediaList;      // 格式
    QSize   resolution;                        // 分辨率
    double  fileSize;                          // 文件大小
    int     interval;                          // 间隔时间
};

struct adsConfig {
    QString     nextUrl;        // 下次检查地址
    adsDvd      dvdConfig;
    adsiPhone   iPhoneConfig;
    adsConvert  convertConfig;
};

#endif // DYSTRUCTU_H
