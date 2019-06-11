#ifndef GLOBALARG_H
#define GLOBALARG_H

#include "singleton.h"
#include <QStandardPaths>
#include <QObject>
#include <QVersionNumber>

#define MIME_TYPE "customWidget/widget"
#define MEDIA_TYPE "text/uri-list"

#define Global CGlobal::getInstance()

class CGlobal : public QObject, public Singleton<CGlobal>
{
    friend class Singleton<CGlobal>;
    friend class QSharedPointer<CGlobal>;

private:
    Q_DISABLE_COPY(CGlobal)
    explicit CGlobal(QObject *parent = Q_NULLPTR);
    ~CGlobal(){}

public:
    enum General_Stay    {Stay_Always, Stay_Playing, Stay_Never, Stay_Unknown};
    enum General_Close   {Close_Mini, Close_Exit, Close_Unknown};
    enum General_Login   {Login_Yes, Login_No, Login_Unknown};
    enum General_Update  {Update_Never, Update_Always, Update_Day, Update_Week, Update_Month, Update_Unknown};

    enum Download_Format     {Format_MP4, Format_WebM, Format_FLV, Format_3gp, Format_Unknown};
    enum Download_Resolution {R_1080P, R_720P, R_640P, R_480P, R_Unknown};

    enum Subtitle_FontSize   {Smaller, Small, Normal, Large, Larger, Unknown};

    enum LibraryMode {IcoMode,ListMode,Mode_Unknown};

    //旋转 交换视频的宽高 / 宽高比
    void rotate();

    //VOLUME
    void setVolume(const int &v);
    int volume();

    // library显示模式
    void setLibraryMode(const LibraryMode &index);
    LibraryMode getLibraryMode();
    //开机启动
    void openBootUp();
    bool isOpenBootUp();
    void closeBootUp();

    // dvd打开模式 UDF=1;  ISO9600=2; Try UDF then ISO = 0
    void setDvdOpenType(const int dvdType);
    int  dvdOpenType();

    void setAutoAssociation(const bool &isAuto);
    bool autoAssociation();

    // 硬件加速
    void setUseHardware(const bool &use);
    int useHardware();

    void setUserHwType(const int &index);    // hw类型
    int  userHwType();

    void setHwSupport(QString name,bool isSupport);    // 记录支持的类型
    int  getHwSupport();
    bool isHwSupport(QString name);         // 查询是否支持

    // 硬件信息
    void setCpuHwInfo(QStringList cpu);
    QStringList CpuHwInfo();
    void setGpuHwInfo(QStringList gpu);
    QStringList GpuHwInfo();

    QString gpuInfos();
    void setgpuInfos(QString jsonStr);

    void setCuvid(const bool &isOpen);
    bool cuvid();

    //live面板历史记录显示
    void setLiveRecentShow(const bool &isShow);
    bool liveRecentShow();

    //多语言的设置
    void setLanguage(const QString &language);
    QString getLanguage();

    // 地区
    void    setLocalArea(const QString &area);
    QString getLocalArea();
    void    setLocalAreaTime(const QString &date);
    QString getLocalAreaTime();
    // 软件版本
    QVersionNumber getCurVersion();
    QVersionNumber getWebVersion();
    void setWebVersion(QVersionNumber webVersion);

    QString getFontFamily();

    // 播放结束后播放模式
    void setPlayMode(const int &mode);
    int  getPlayMode();

    // UUID
    QString getUuid();

    //是否是今天第一次启动
    bool todayOnce(const QString &);

    // Ytb更新的最后版本的Last_Modified
    void setYtbLastModified(const QString &lastDate);
    QString getYtbLastModified();

    // 主程序更新标记
    void setMainUpFlag(const bool &upfinish);
    bool getMainUpFlag();

    // Ytb下载完成但未解压的标记
    void setYtbdownFinish(const bool &finish);
    bool getYtbdownFinish();
    // ytb更新的最后检查时间
    void setYtbLastCheckDate(const QString &lastDate);
    QString getYtbLastCheckDate();

    //外置控制条
    void setShowOutControl(const bool &);
    bool showOutControl();

    //General
    void setStayOnTop(const General_Stay &index);
    General_Stay stayOnTop();

    void setCloseType(const General_Close &index);
    General_Close closeType();

    void setOpenLogin(const General_Login &);
    General_Login openLogin();

    // Update模式，每次/天/周/月
    void setCheckUpdateMode(const General_Update &index);
    General_Update checkUpdateMode();

    void setSnapShotPath(const QString &path);
    QString snapShotPath();

    void setConvertPath(const QString &path);
    QString convertPath();

    //AirPlay
    void setMirrorRecordPath(const QString &path);
    QString mirrorRecordPath();

    // ad目录
    QString adFloderPath();
    QString adFloderTemp();

    void    setGoogleRate(int rate);
    int     getGoogleRate();

    void setAirplayStarted(const bool &isOpen);
    bool airplayStarted();

    //Downloader
    void setDownloadFormat(const Download_Format &index);
    Download_Format downloadFormat();

    void setYtbUpGradeWeb(const QString &web);
    QString getYtbUpGradeWeb();

    void setMainUpGradeWeb(const QString &web);
    QString getMainUpGradeWeb();

    void setDownloadResolution(const Download_Resolution &index);
    Download_Resolution downloadResolution();

    void setOpenProxy(const bool &isOpen);
    bool openProxy();

    void setProxyUrl(const QString &path);
    QString proxyUrl();

    void setDownloadPath(const QString &path);
    QString downloadPath();

    QString tempPath();

    //subtitle
    void setSubtitleEncode(const QString &encode);
    QString subtitleEncode();

    void setSubtitleFont(const QString &font);
    QString subtitleFont();

    void setSubtitleSize(const Subtitle_FontSize &index);
    Subtitle_FontSize subtitleSize();

    void setAutodetectFuzzy(const int &index);
    int autodetectFuzzy();

    // 设置检查的日期
    void    setCheckUpDate(const QString &date);
    QString getCheckUpDate();

    qint64 checkDateForDay();
    qint64 checkDateForWeek();
    qint64 checkDateForMonth();


    //detect

    void setTextColor(const QString &color);
    QString textColor();

    void setTextOpacity(const int &opacity);
    int textOpacity();

    void setBgColor(const QString &color);
    QString bgColor();

    void setBgOpacity(const int &opacity);
    int bgOpacity();

    QString removeSetColon(QString text);

    //DVD 镜像文件后缀格式
    const QStringList& mirrorFiles()const;

    //获取数据目录路径
    QString getAppdataPath() const;

    //注册码
    void setRegistCode(const QString &code);
    QString registCode();
    bool checkRegistCode(const QString &code) const;//检测注册码是否正确

    //
    QString lineEditStyleSheet(const uint &) const;
    QString menuStyleSheet();

    QString localName();
    QString macAddress();

    int winVersion();

    QString liveUrl() const;
    QString liveName()const;
    void setLiveInfo(const QString &url, const QString &name);

    //毫秒转成时间格式
    QString milliToTime(const int &msd) const;
    QString secondToTime(const int &sd) const;

    int bonjourServiceState();

    int checkLastString(QString soure,QString str);
    bool Extract(const QString& in_file_path, const QString& out_file_path); // 解压缩文件

    void    saveShowAdsTime(QDateTime time);        // 广告显示时间
    QString getShowAdsTime();

    void    saveAdsConfigTime(QDateTime time);      // 读取网络配置时间
    QString getAdsConfigTime();

    void    saveAdiniUpTime(QDateTime time);    // 广告配置信息更新时间
    QString getAdiniUpTime();

    void saveNoshowAdNumber();                      // 广告不再显示列表
    void initNoshowAdNumber();
    QList<int> getNoshowAdNumber();
    void addNoShowAdNumber(int number);
public:
    // 广告第一次打开
    bool _advertFirst;

    //视频的宽高比radio
    int _wRatio;
    int _hRatio;
    int _w;
    int _h;

    //初始宽高比
    int _wOriginalRatio;
    int _hOriginalRatio;

    //当前屏幕的缩放比例
    qreal _screenRatio;
    //文字的缩放比例
    qreal _fontRatio;

    //是否开启preview
    bool b_openPreview = false;

    //版本号
    int _version_maj;
    int _version_min;

    //当前路径 (D:/XX/xx)
    QString _cur_path;

    bool _user_ytb;     // ytb正在使用的标记
    bool _down_ytb;     // ytb正在下载的标记

    const QStringList _urlFormat{"MP4","WebM","FLV","3GP"};
    const QStringList _urlResolution{"1080","720","640","480"};
    const QString     _constItemName[11]{"Favorites","FMovies","FMusic","YouTube",\
                                 "yVideos","yMusic","PlayLists","pMovies",\
                                 "pMusic","pAirRecord","nListNew"};

    QHash<QString, QString> languageList() const {return _languageMap;}

    void openLanguageUrl(const QString &key);
    QString getLanguageUrl(const QString &key);

    QString getuserYtbPaht();

private:
    int ivalue(const QString &key) const;
    QString getCustomPath(QStandardPaths::StandardLocation type, const QString &subPath) const;

    void initLanguageList();
    QString getValidLanguage(const QString &l);
    QString getVerWeb(QString web);

    void initUrls();

private:
    int i_volume = -1;

    bool i_useHw = false;
    int i_HwType= -1;
    int i_hwSupport = -1;
    int i_dxva2Codec = -1;
    int i_qscCodec   = -1;
    int i_cudaCodec  = -1;

    int i_googleRate = -1;
    int i_win_version     = -1;
    int b_showOutControlW = -1;  //是否显示外置控制条
    QStringList _mirrorFile; //镜像文件

    QString     _adsShowTime        = "";
    QString     _adsConfigTime      = "";
    QString     _adiniUpTime        = "";

    QVersionNumber _webVersion;

    General_Stay _stayOnTop  = Stay_Unknown;
    General_Close _closeType = Close_Unknown;
    LibraryMode _libraryMode = Mode_Unknown;
    General_Login _openLogin = Login_Unknown;
    General_Update _checkUpdates = Update_Unknown;
    QString _snapShotPath = "";
    QString _convertPath  = "";
    QString _mirrorRecordPath = "";
    Download_Format _downloadFormat = Format_Unknown;
    Download_Resolution _downloadResolution = R_Unknown;

    int i_autodetectFuzzy   = -1;
    Subtitle_FontSize _subtitleSize = Unknown;
    QString _textColor      = "";
    int i_textOpacity       = -1;
    QString _bgColor        = "";
    int i_bgOpacity         = -1;

    int i_dvdOpenType       = 0;

    QString _liveUrl  ="";
    QString _liveName ="";
    QString _upDate = "";

    QHash<QString, QString> _languageMap;
    QHash<QString, QString> _urlHash;
    QList<int>              _noShowAdNumbers;
};

#endif // GLOBALARG_H
