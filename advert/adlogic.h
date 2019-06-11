#ifndef ADLOGIC_H
#define ADLOGIC_H

#include <QObject>
#include "singleton.h"
#include "dystructu.h"
#include <QByteArray>

class QNetworkReply;
class QFile;
class GGWidget;
class QTimer;
class AppleNotify;
/*!
    广告的所有验证，下载相关逻辑单例类
*/
#define Ad ADLogic::getInstance()
struct LocalPreference {
    QString adType;
    int     adCount;
    LocalPreference() {
        adType  = "";
        adCount = 1;
    }
};

class ADLogic : public QObject,public Singleton<ADLogic>
{
    Q_OBJECT
    friend class  Singleton<ADLogic>;
    friend class QSharedPointer<ADLogic>;
public:
    explicit ADLogic(QObject *parent = 0);

    void startAdByAuto();                               // 显示广告，根据时间自动显示
    void startAdByEventType(QString type, QString winKey);      // 显示广告，根据事件类型显示
    void languageChange();                              // 语言改变时，需要重新获取广告信息
signals:
private:
    void getLocalArea();                        // 获取本地的地区
    void getLocalAdFile();                      // 获取本地广告信息
    void writeLocalAdFile();                    // 写广告到本地文件
    void getAdFileByWeb();                      // 获取网络上的广告
    void getAdFileByFixedWeb();                 // 使用固定的url获取网络上的广告

    void initAdini();                       // 初始化adsConfig信息
    bool getLocalAdini();                   // 获取本地广告配置信息
    void getWebAdini();                     // 从网络上获取广告配置信息
    void getOldWebAdini();                  // 使用固定地址获取广告配置信息
    void writeLocalAdini();                 // 写广告配置信息到本地文件
    void readWebAdini(QByteArray data);

    bool checkLocalFileIsExit(int fileNumber);  // 检查本地文件是否存在
    QString getAdWinKey(int number);            // 获取广告文件的关键字

    adWebFile analyJsonByByteArray(QByteArray bytes);   // 分析网络广告信息
    QList<adFile> comparedLocalOrWeb();                 // 对比本地和网络文件的不同，返回增加的列表，删除多余的列表
    bool      deleteFolder(QString path);               // 删除文件夹
    bool      checkAdvertData(int number);              // 检查广告时间是否有效
    bool      isShowTimeOut(adFile fileInfo);           // 广告的上次显示时间离这次显示时间已经到了
    void      startShowAd();                            // 完成，开始显示广告

    void      startDownWebFile(const QString& url);
    void      downFinish(const QString& url);
    void      downFail(const QString& url);
    void      clearDownZip();
    bool      comparedSize(QSize tarSize,QSize souSize);        // 比较tarSize是否有一个值大于souSize
    bool      compareFormat(MEDIAINFO media);                   // 比较media的格式是否符合adsConInfo中的格式

    QList<LocalPreference>  analyPreferenceInfo(QByteArray bytes);  // 分析本地偏好信息
    QList<LocalPreference>  getPreferInfoence();                    // 获取本地的偏好信息
    void            savePreference();                               // 点击广告后，记录偏好
    QString         getPrefType();                                  // 根据本地的偏好信息和网络上的广告类型，获取本次显示的广告类型

    QByteArray encodeAes(const QByteArray &key, const QByteArray &data);        // 加密
    QByteArray decodeAes(const QByteArray &key, const QByteArray &data);        // 解密
private slots:
    void httpAreaRead();
    void httpAreaFinish();

    void httpWebReadyRead();
    void httpWebFinished();

    void httpAdsReadyRead();
    void httpAdsFinished();

    void webDownFinish();
    void webDownRead();
    void autoPushTimerOut();
    void adUpConfigOut();
    void openUrlSlt(QString type);
    void appleNotifySlt();
    void playMediaChange(MEDIAINFO mediainfo);
public slots:
private:

    int                     _text = 1;
    adsConfig               _adsConInfo;    // 广告的配置信息
    adWebFile               _localFile;     // 本地文件
    QString                 _adLanguage;
    bool                    _iswebFinish;   // 网络获取成功
    bool                    _useFixedWeb;   // 获取xml信息使用固定url
    bool                    _useOldiniWeb;  // 使用固定地址获取ini标记
    adWebFile               _webFile;       // 网络文件
    QList<adFile>           _downAdList;    // 下载文件列表

    QNetworkReply*          _areaReply  = NULL;
    QByteArray              _adArea;

    QNetworkReply*          _adsConfigReply  = NULL; // 广告信息Reply
    QByteArray              _adsConfigData;

    QNetworkReply*          _handReply  = NULL; // 广告信息Reply
    QByteArray              _adData;

    QNetworkReply*          _downReply  = NULL; // 下载Reply
    QFile*                  _downFile;
    QString                 _downFilePath;
    QString                 _downUrl;

    QTimer*                 _autoPushTimer;      // 广告自动推送的定时器，第一次为5-10分钟之间，下次为间隔时间
    QTimer*                 _adConfigTimer;      // 广告检查定时器

    AppleNotify*            _adApple;            // 检查apple设备
    QString                 _adFolderpath;       // 广告的目录
    GGWidget*               _ggWidget;           // 弹出广告的面板
    QList<LocalPreference>  _localPreference;    // 用户偏好，即用户点击广告类型的记录

    const QByteArray randomKey = "*|/elbiSYw#0i2j&";
};

#endif // ADLOGIC_H
