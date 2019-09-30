#include "globalarg.h"
#include "configure.h"
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QVariant>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonValue>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSysInfo>
#include <QSettings>
#include <QtNetwork>
#include <Windows.h>
#include "openfile.h"
#include "quazip.h"
#include "quazipfile.h"

CGlobal::CGlobal(QObject *parent) : QObject(parent)
{
    _w = _h = 0;
    _wRatio = _hRatio = 0;
    _wOriginalRatio = _hOriginalRatio = 0;
    _fontRatio = _screenRatio = 1.0;

    _cur_path = "";
    _version_maj = 6;
    _version_min = 1;

    _user_ytb    = false;
    _down_ytb    = false;
    _advertFirst = true;
    _webVersion  = QVersionNumber(_version_maj,_version_min);

    _mirrorFile << "iso" << "bin"
                << "nrg" << "vcd"
                << "cif" << "fcd"
                << "img" << "ccd"
                << "c2d" << "dfi"
                << "tao" << "dao"
                << "cue" << "mds"
                << "bwt" << "cdi"
                << "pdi" << "b5t"
                << "isz" << "lcd"
                << "mdf";
    initLanguageList();
    initNoshowAdNumber();
    initUrls();
}


void CGlobal::rotate()
{
    qSwap(_w, _h);
    qSwap(_wRatio, _hRatio);
}

void CGlobal::setVolume(const int &v)
{
    i_volume = v;
    Config->setConfigVar("Control/Volume", v);
}

int CGlobal::volume()
{
    if(i_volume<0 || i_volume>144) {
        i_volume = ivalue("Control/Volume");
        if(i_volume<0 || i_volume>144)
            i_volume = 12;
        setVolume(i_volume);
    }
    return i_volume;
}

void CGlobal::setLibraryMode(const CGlobal::LibraryMode &index)
{
    _libraryMode = index;
    const int i = index;
    Config->setConfigVar("Player/LibraryMode", i);
}

CGlobal::LibraryMode CGlobal::getLibraryMode()
{
    if(_libraryMode == Mode_Unknown) {
        int i = ivalue("Player/LibraryMode");
        if(i<0 || i>1)
            i = 0;//ico模式
        setLibraryMode(LibraryMode(i));
    }
    return _libraryMode;
}

void CGlobal::openBootUp()
{
    const QString HKEY("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    QSettings set(HKEY, QSettings::NativeFormat);
    //windows注册表 路径为 \  不认/
    QString value = QString("\"%1/5KPlayer.exe\" -auto").arg(_cur_path);
    value.replace("/", "\\");
    set.setValue("5KPlayer", value);
}

bool CGlobal::isOpenBootUp()
{
    const QString HKEY("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    QSettings set(HKEY, QSettings::NativeFormat);
    if(set.contains("5KPlayer"))
        return true;
    return false;
}

void CGlobal::closeBootUp()
{
    const QString HKEY("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    QSettings set(HKEY, QSettings::NativeFormat);
    set.remove("5KPlayer");
}

void CGlobal::setDvdOpenType(const int dvdType)
{
    i_dvdOpenType = dvdType;
}

int CGlobal::dvdOpenType()
{
    return i_dvdOpenType;
}

void CGlobal::setAutoAssociation(const bool &isAuto)
{
    Config->setConfigVar("Setting/AutoAssociation", isAuto ? 1 : 0);
}

bool CGlobal::autoAssociation()
{
    int value = ivalue("Setting/AutoAssociation");
    if(value<0 || value >1) {
        value = 0;
        setAutoAssociation(false);
    }
    return value == 1;
}

void CGlobal::setUseHardware(const bool &use)
{
    Config->setConfigVar("Hardware/UserHw", use ? 1 : 0);
}

int CGlobal::useHardware()
{
    int userHw = -1;
    if(Config->getConfigVar("Hardware/UserHw").isValid()) {
        userHw = Config->getConfigVar("Hardware/UserHw").toInt();
    }
    return userHw;
}

void CGlobal::setUserHwType(const int &index)
{
    i_HwType = index;
    Config->setConfigVar("Hardware/UserHwType", i_HwType);
}

int CGlobal::userHwType()
{
    if(i_HwType < 0 || i_HwType > 2) {
        i_HwType = ivalue("Hardware/UserHwType");
    }

    return i_HwType;
}

void CGlobal::setHwSupport(QString name, bool isSupport)
{
    i_hwSupport = Config->getConfigVar("Hardware/HwSupport").toInt();
    if(name == "NVIDIA") {
        if(isSupport) {
            i_hwSupport |= 4;
        } else {
            i_hwSupport &= 3;   // 011
        }
    } else if(name == "QSV") {
        if(isSupport) {
            i_hwSupport |= 2;
        } else {
            i_hwSupport &= 5;   // 101
        }
    } else if(name == "DXVA2") {
        if(isSupport) {
            i_hwSupport |= 1;
        } else {
            i_hwSupport &= 6;   // 110
        }
    }
    Config->setConfigVar("Hardware/HwSupport", i_hwSupport);
}

int CGlobal::getHwSupport()
{
    if(Config->getConfigVar("Hardware/HwSupport").isValid())
        i_hwSupport = Config->getConfigVar("Hardware/HwSupport").toInt();
    return i_hwSupport;
}

bool CGlobal::isHwSupport(QString name)
{
    i_hwSupport = getHwSupport();
    bool isSupport = false;
    int hwSup = 0;
    if(name == "NVIDIA") {
        hwSup = 4;
    } else if(name == "QSV") {
        hwSup = 2;
    } else if(name == "DXVA2") {
        hwSup = 1;
    }
    isSupport = (i_hwSupport & hwSup) != 0;

    return isSupport;
}

void CGlobal::setCpuHwInfo(QStringList cpu)
{
    Config->setConfigVar("Hardware/Cpu", cpu);
}

QStringList CGlobal::CpuHwInfo()
{
    QStringList cpuinfo;
    cpuinfo.clear();
    if(Config->getConfigVar("Hardware/Cpu").isValid())
        cpuinfo = Config->getConfigVar("Hardware/Cpu").toStringList();
    return cpuinfo;
}

void CGlobal::setGpuHwInfo(QStringList gpu)
{
    Config->setConfigVar("Hardware/Gpu", gpu);
}

QStringList CGlobal::GpuHwInfo()
{
    QStringList gpuinfo;
    gpuinfo.clear();
    if(Config->getConfigVar("Hardware/Gpu").isValid())
        gpuinfo = Config->getConfigVar("Hardware/Gpu").toStringList();
    return gpuinfo;
}

QString CGlobal::gpuInfos()
{
    QString infos = "";
    if(Config->getConfigVar("Hardware/hwInfos").isValid()) {
        infos = Config->getConfigVar("Hardware/hwInfos").toString();
    }
    return infos;
}

void CGlobal::setgpuInfos(QString jsonStr)
{
    Config->setConfigVar("Hardware/hwInfos",jsonStr);
}

void CGlobal::setCuvid(const bool &isOpen)
{
    Config->setConfigVar("Hardware/CUVID_FFMPEG",isOpen);
}

bool CGlobal::cuvid()
{
    bool isOpen = false;
    if(Config->getConfigVar("Hardware/CUVID_FFMPEG").isValid()) {
        isOpen = Config->getConfigVar("Hardware/CUVID_FFMPEG").toBool();
    }
    return isOpen;
}

void CGlobal::setLiveRecentShow(const bool &isShow)
{
    Config->setConfigVar("Live/recentShow",isShow);
}

bool CGlobal::liveRecentShow()
{
    bool isShow = true;
    if(Config->getConfigVar("Live/recentShow").isValid()) {
        isShow = Config->getConfigVar("Live/recentShow").toBool();
    }
    return isShow;
}

void CGlobal::setLanguage(const QString &language)
{
    Config->setConfigVar("SYS/Language", language);
}

QString CGlobal::getLanguage()
{
    QString language("en");
    if(Config->getConfigVar("SYS/Language").isValid()) {
        language = Config->getConfigVar("SYS/Language").toString();
    } else {
        if(QLocale::system().language() == QLocale::Japanese) {
            language = "jp";
        }
    }
    return getValidLanguage(language);
}

void CGlobal::setLocalArea(const QString &area)
{
    Config->setConfigVar("SYS/Area", area);
}

QString CGlobal::getLocalArea()
{
    QString area = "GB";
    if(Config->getConfigVar("SYS/Area").isValid()) {
        area = Config->getConfigVar("SYS/Area").toString();
    }
    return area;
}

void CGlobal::setLocalAreaTime(const QString &date)
{
    Config->setConfigVar("SYS/AreaDate", date);
}

QString CGlobal::getLocalAreaTime()
{
    QString lastAreaTime = "2017-01-01";
    if(Config->getConfigVar("SYS/AreaDate").isValid()) {
        lastAreaTime = Config->getConfigVar("SYS/AreaDate").toString();
    }
    return lastAreaTime;
}

QVersionNumber CGlobal::getCurVersion()
{
    QVersionNumber localVersion(_version_maj,_version_min);
    return localVersion;
}

QVersionNumber CGlobal::getWebVersion()
{
    return _webVersion;
}

void CGlobal::setWebVersion(QVersionNumber webVersion)
{
    _webVersion = webVersion;
}

QString CGlobal::getFontFamily()
{
    QString fontFamily = "Tahoma";
    if(getLanguage() == "jp") {
        //fontFamily = "MS UI Gothic";
        fontFamily = "Microsoft YaHei";
    }
    return fontFamily;
}

void CGlobal::setPlayMode(const int &mode)
{
    Config->setConfigVar("Control/PlayMode",mode);
}

int CGlobal::getPlayMode()
{
    int mode = 1;
    if(Config->getConfigVar("Control/PlayMode").isValid()) {
        mode = ivalue("Control/PlayMode");
    }
    return mode;
}

QString CGlobal::getUuid()
{
    QString uuidStr;
    if(Config->getConfigVar("SYS/Uuid").isValid()) {
        uuidStr = Config->getConfigVar("SYS/Uuid").toString();
    }
    if(uuidStr.isEmpty())
    {
        QUuid uuid = QUuid::createUuid();
        uuidStr = uuid.toString().replace(QRegExp("(\\{)|(-)|(\\})"), "");
        Config->setConfigVar("SYS/Uuid", uuidStr);
    }
    return uuidStr;
}

bool CGlobal::todayOnce(const QString &str)
{
    const QString key = "SYS/bootonce" + str;
    QString cur = QDate::currentDate().toString("yyMMdd");
    QString iniStr;

    if(Config->getConfigVar(key).isValid()) {
        iniStr = Config->getConfigVar(key).toString();
    }
    if(iniStr.isEmpty())
    {
        Config->setConfigVar(key, cur);
        return true;
    }
    else
    {
        return iniStr != cur;
    }
}

void CGlobal::setYtbLastModified(const QString &lastDate)
{
    Config->setConfigVar("Upgrade/YtbLastModified",lastDate);
}

QString CGlobal::getYtbLastModified()
{
    QString lastModified = "2017-01-01";
    if(Config->getConfigVar("Upgrade/YtbLastModified").isValid()) {
        lastModified = Config->getConfigVar("Upgrade/YtbLastModified").toString();
    }
    return lastModified;
}

void CGlobal::setMainUpFlag(const bool &upfinish)
{
    Config->setConfigVar("Upgrade/MainUpFlag",upfinish);
}

bool CGlobal::getMainUpFlag()
{
    bool upFinish = false;
    if(Config->getConfigVar("Upgrade/MainUpFlag").isValid()) {
        upFinish = Config->getConfigVar("Upgrade/MainUpFlag").toBool();
    }
    return upFinish;
}

void CGlobal::setYtbdownFinish(const bool &finish)
{
    Config->setConfigVar("Upgrade/YtbDownFinish",finish);
}

bool CGlobal::getYtbdownFinish()
{
    bool downfinish = false;
    if(Config->getConfigVar("Upgrade/YtbDownFinish").isValid()) {
        downfinish = Config->getConfigVar("Upgrade/YtbDownFinish").toBool();
    }
    return downfinish;
}

void CGlobal::setYtbLastCheckDate(const QString &lastDate)
{
    Config->setConfigVar("Upgrade/YtbLastCheckDate",lastDate);
}

QString CGlobal::getYtbLastCheckDate()
{
    QString lastCheckDate = "2017-01-01";
    if(Config->getConfigVar("Upgrade/YtbLastCheckDate").isValid()) {
        lastCheckDate = Config->getConfigVar("Upgrade/YtbLastCheckDate").toString();
    }
    return lastCheckDate;
}

void CGlobal::setShowOutControl(const bool &isShow)
{
    b_showOutControlW = isShow ? 1 : 0;
    //写入配置表
    Config->setConfigVar("Player/ShowOutControl", b_showOutControlW);
}

bool CGlobal::showOutControl()
{
    if(b_showOutControlW == -1) {
        b_showOutControlW = ivalue("Player/ShowOutControl");
        if(b_showOutControlW<0 || b_showOutControlW>1)
            setShowOutControl(false);
    }
    return b_showOutControlW == 1;
}

void CGlobal::setStayOnTop(const CGlobal::General_Stay &index)
{
    _stayOnTop = index;
    const int i = index;
    Config->setConfigVar("Setting/StayOnTop", i);
}

CGlobal::General_Stay CGlobal::stayOnTop()
{
    if(_stayOnTop == Stay_Unknown) {
        int i = ivalue("Setting/StayOnTop");
        if(i<0 || i>2)
            i = 2; //从不
        setStayOnTop(General_Stay(i));
    }
    return _stayOnTop;
}

void CGlobal::setCloseType(const CGlobal::General_Close &index)
{
    _closeType = index;
    const int i = index;
    Config->setConfigVar("Setting/CloseExit", i);
}

CGlobal::General_Close CGlobal::closeType()
{
    if(_closeType == Close_Unknown) {
        int i = ivalue("Setting/CloseExit");
        if(i<0 || i>1)
            i = 0;//最小化
        setCloseType(General_Close(i));
    }
    return _closeType;
}

void CGlobal::setOpenLogin(const CGlobal::General_Login &index)
{
    _openLogin = index;
    const int i = index;
    Config->setConfigVar("Setting/Login", i);
}

CGlobal::General_Login CGlobal::openLogin()
{
    if(_openLogin == Login_Unknown) {
        int i = ivalue("Setting/Login");
        if(i<0 || i>1)
            i = 0; //Yes
        setOpenLogin(General_Login(i));
    }
    return _openLogin;
}

void CGlobal::setCheckUpdateMode(const CGlobal::General_Update &index)
{
    _checkUpdates = index;
    const int i = index;
    Config->setConfigVar("Setting/Updata", i);
}

CGlobal::General_Update CGlobal::checkUpdateMode()
{
    if(_checkUpdates == Update_Unknown) {
        int i = ivalue("Setting/Updata");
        if(i<0 || i>4)
            i = 2;//默认每天
        setCheckUpdateMode(General_Update(i));
    }
    return _checkUpdates;
}

void CGlobal::setSnapShotPath(const QString &path)
{
    _snapShotPath = path;
    Config->setConfigVar("Setting/SnapShot", path);
}

QString CGlobal::snapShotPath()
{
    if(_snapShotPath.isEmpty()) {
        _snapShotPath = Config->getConfigVar("Setting/SnapShot").toString();
        if(_snapShotPath.isEmpty())
            setSnapShotPath(getCustomPath(QStandardPaths::PicturesLocation, "/Snapshot"));
    }
    return _snapShotPath;
}

void CGlobal::setConvertPath(const QString &path)
{
    _convertPath = path;
    Config->setConfigVar("Setting/Convert", path);
}

QString CGlobal::convertPath()
{
    if(_convertPath.isEmpty()) {
        _convertPath = Config->getConfigVar("Setting/Convert").toString();
        if(_convertPath.isEmpty())
            setConvertPath(getCustomPath(QStandardPaths::MusicLocation, "/Download Music"));
    }
    return _convertPath;
}

void CGlobal::setMirrorRecordPath(const QString &path)
{
    _mirrorRecordPath = path;
    Config->setConfigVar("Setting/AirPath", path);
}

QString CGlobal::mirrorRecordPath()
{
    if(_mirrorRecordPath.isEmpty()) {
        _mirrorRecordPath = Config->getConfigVar("Setting/AirPath").toString();
        if(_mirrorRecordPath.isEmpty())
            setMirrorRecordPath(getCustomPath(QStandardPaths::MoviesLocation, "/Mirror Videos"));
    }
    return _mirrorRecordPath;
}

QString CGlobal::adFloderPath()
{
    QString userADPaht = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return userADPaht;
}

QString CGlobal::adFloderTemp()
{
    QString userADPaht = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    return userADPaht;
}

void CGlobal::setGoogleRate(int rate)
{
    i_googleRate = rate;
    Config->setConfigVar("AD/Google", rate);
}

int CGlobal::getGoogleRate()
{
    i_googleRate = ivalue("AD/Google");
    if(i_googleRate < 0 || i_googleRate > 100)
        setGoogleRate(20);
    return i_googleRate;
}

void CGlobal::setAirplayStarted(const bool &isOpen)
{
    Config->setConfigVar("Setting/AirplayState", isOpen ? 1 : 0);
}

bool CGlobal::airplayStarted()
{
    int value = ivalue("Setting/AirplayState");
    if(value<0 || value>1) {
        value = 1; //默认开启
        setAirplayStarted(false);
    }
    return value == 1;
}

void CGlobal::setDownloadFormat(const CGlobal::Download_Format &index)
{
    _downloadFormat = index;
    const int i = index;
    Config->setConfigVar("Setting/Format", i);
}

CGlobal::Download_Format CGlobal::downloadFormat()
{
    if(_downloadFormat == Format_Unknown) {
        int i = ivalue("Setting/Format");
        if(i<0 || i>3)
            i = 0; //MP4
        setDownloadFormat(Download_Format(i));
    }
    return _downloadFormat;
}

void CGlobal::setYtbUpGradeWeb(const QString &web)
{
    Config->setConfigVar("UpGradeWeb/ytb",web);
}

QString CGlobal::getYtbUpGradeWeb()
{
    QString ytbweb = "http://dl1.5kplayer.com/youtube/upgradeYoutube.config";
    if(Config->getConfigVar("UpGradeWeb/ytb").isValid())
        ytbweb = Config->getConfigVar("UpGradeWeb/ytb").toString();
    return ytbweb;
}

void CGlobal::setMainUpGradeWeb(const QString &web)
{
    Config->setConfigVar("UpGradeWeb/main",web);
}

QString CGlobal::getMainUpGradeWeb()
{
    QString mainweb = "http://www.5kplayer.com/upgrade/windows/upgrade5KPlayer.config";
    if(Config->getConfigVar("UpGradeWeb/main").isValid())
        mainweb = Config->getConfigVar("UpGradeWeb/main").toString();
    return mainweb;
}

void CGlobal::setDownloadResolution(const CGlobal::Download_Resolution &index)
{
    _downloadResolution = index;
    const int i = index;
    Config->setConfigVar("Setting/Resolution", i);
}

CGlobal::Download_Resolution CGlobal::downloadResolution()
{
    if(_downloadResolution == R_Unknown) {
        int i = ivalue("Setting/Resolution");
        if(i<0 || i>3)
            i = 0; //1080P
        setDownloadResolution(Download_Resolution(i));
    }
    return _downloadResolution;
}

void CGlobal::setOpenProxy(const bool &isOpen)
{
    Config->setConfigVar("Setting/proxy", isOpen ? 1 : 0);
}

bool CGlobal::openProxy()
{
    int value = ivalue("Setting/proxy");
    if(value<0 || value>1) {
        value = 0;
        setOpenProxy(false);
    }
    return value == 1;
}

void CGlobal::setProxyUrl(const QString &path)
{
    Config->setConfigVar("Setting/ProxyText", path);
}

QString CGlobal::proxyUrl()
{
    QString str = Config->getConfigVar("Setting/ProxyText").toString();
    if(str.isEmpty()) {
        str = "https://127.0.0.1:1080";
        setProxyUrl(str);
    }
    return str;
}

void CGlobal::setDownloadPath(const QString &path)
{
    Config->setConfigVar("Setting/downPath", path);
}

QString CGlobal::downloadPath()
{
    QString str = Config->getConfigVar("Setting/downPath").toString();
    if(str.isEmpty()) {
        str = getCustomPath(QStandardPaths::MoviesLocation, "/Download Videos");
        setDownloadPath(str);
    }
    return str;
}

QString CGlobal::tempPath()
{
    QString str = getCustomPath(QStandardPaths::TempLocation, "");
    return str;
}

void CGlobal::setSubtitleEncode(const QString &encode)
{
    Config->setConfigVar("Setting/Encode", encode);
}

QString CGlobal::subtitleEncode()
{
    QString str = Config->getConfigVar("Setting/Encode").toString();
    if(str.isEmpty()) {
        str = "utf-8";
        setSubtitleEncode(str);
    }
    return str;
}

void CGlobal::setSubtitleFont(const QString &font)
{
    Config->setConfigVar("Setting/Font", font);
}

QString CGlobal::subtitleFont()
{
    QString str = Config->getConfigVar("Setting/Font").toString();
    if(str.isEmpty()) {
        str = "Arial";
        setSubtitleFont(str);
    }
    return str;
}

void CGlobal::setSubtitleSize(const CGlobal::Subtitle_FontSize &index)
{
    _subtitleSize = index;
    const int i = index;
    Config->setConfigVar("Setting/FontSize", i);
}

CGlobal::Subtitle_FontSize CGlobal::subtitleSize()
{
    if(_subtitleSize == Unknown) {
        int i = ivalue("Setting/FontSize");
        if(i<0 || i>4)
            i = 2; //normal
        setSubtitleSize(Subtitle_FontSize(i));
    }
    return _subtitleSize;
}

void CGlobal::setAutodetectFuzzy(const int &index)
{
    i_autodetectFuzzy = index;
    Config->setConfigVar("Setting/AutodetectFuzzy", index);
}

int CGlobal::autodetectFuzzy()
{
    if(i_autodetectFuzzy<0 || i_autodetectFuzzy>4) {
        i_autodetectFuzzy = ivalue("Setting/AutodetectFuzzy");
        if(i_autodetectFuzzy<0 || i_autodetectFuzzy>4)
            setAutodetectFuzzy(2);
    }
    return i_autodetectFuzzy;
}

void CGlobal::setCheckUpDate(const QString &date)
{
    _upDate = date;

    Config->setConfigVar("Upgrade/CheckUpdate", date);
}

QString CGlobal::getCheckUpDate()
{
    if(_upDate.isEmpty()) {
        _upDate = Config->getConfigVar("Upgrade/CheckUpdate").toString();
        if(_upDate.isEmpty())
            setCheckUpDate("2017-04-01");
    }
    return _upDate;
}

qint64 CGlobal::checkDateForDay()
{
    const QDateTime curDTime = QDateTime::currentDateTime();
    const QDateTime preDTime = QDateTime::fromString(getCheckUpDate(), "yyyy-MM-dd");

    return preDTime.daysTo(curDTime);
}

qint64 CGlobal::checkDateForWeek()
{
    const QDateTime curDTime = QDateTime::currentDateTime();
    const QDateTime preDTime = QDateTime::fromString(getCheckUpDate(), "yyyy-MM-dd");

    if(curDTime.date().year() == preDTime.date().year())
        return curDTime.date().weekNumber() - preDTime.date().weekNumber();
    if(curDTime.date().year() > preDTime.date().year())
        return 1;
    return -1;
}

qint64 CGlobal::checkDateForMonth()
{
    const QDateTime curDTime = QDateTime::currentDateTime();
    const QDateTime preDTime = QDateTime::fromString(getCheckUpDate(), "yyyy-MM-dd");

    if(curDTime.date().year() == preDTime.date().year())
        return curDTime.date().month() - preDTime.date().month();
    if(curDTime.date().year() > preDTime.date().year())
        return 1;
    return -1;
}

#include <QColor>
void CGlobal::setTextColor(const QString &color)
{
    _textColor = color;

    Config->setConfigVar("Setting/TextColor", color);
}

QString CGlobal::textColor()
{
    if(_textColor.isEmpty()) {
        _textColor = Config->getConfigVar("Setting/TextColor").toString();
        if(_textColor.isEmpty())
            setTextColor("white");
    }
    return _textColor;
}

void CGlobal::setTextOpacity(const int &opacity)
{
    i_textOpacity = opacity;
    Config->setConfigVar("Setting/TextOpacity", opacity);
}

int CGlobal::textOpacity()
{
    if(i_textOpacity < 0 || i_textOpacity > 255) {
        i_textOpacity = ivalue("Setting/TextOpacity");
        if(i_textOpacity < 0 || i_textOpacity > 255)
            setTextOpacity(255);
    }
    return i_textOpacity;
}

void CGlobal::setBgColor(const QString &color)
{
    _bgColor = color;

    Config->setConfigVar("Setting/BgColor", color);
}

QString CGlobal::bgColor()
{
    if(_bgColor.isEmpty()) {
        _bgColor = Config->getConfigVar("Setting/BgColor").toString();
        if(_bgColor.isEmpty())
            setBgColor("black");
    }
    return _bgColor;
}

void CGlobal::setBgOpacity(const int &opacity)
{
    i_bgOpacity = opacity;
    Config->setConfigVar("Setting/BgOpacity", opacity);
}

int CGlobal::bgOpacity()
{
    if(i_bgOpacity < 0 || i_bgOpacity > 255) {
        i_bgOpacity = ivalue("Setting/BgOpacity");
        if(i_bgOpacity < 0 || i_bgOpacity > 255)
            setBgOpacity(0);
    }
    return i_bgOpacity;
}

QString CGlobal::removeSetColon(QString text)
{
    QString reText = text;
    if(text.right(1) == ":") {
        reText = text.left(text.size()-1);
    }
    return reText;
}

const QStringList &CGlobal::mirrorFiles() const
{
    return _mirrorFile;
}

QString CGlobal::getAppdataPath() const
{
    //    QStringList environmentList = QProcess::systemEnvironment();
    //    QString appPath("");
    //    foreach (QString environment, environmentList )
    //    {
    //        if (environment.startsWith("APPDATA=", Qt::CaseInsensitive))
    //        {
    //            appPath = environment.mid(QString("APPDATA=").length());
    //            break;
    //        }
    //    }
    const QString appPath = getCustomPath(QStandardPaths::AppDataLocation, "");

    QDir temp(appPath);
    temp.cdUp();

    const QString roamPath = temp.absolutePath().append("/5KPlayer");
    temp.setPath(roamPath);
    if(!temp.exists())
        temp.mkpath("./");

    return roamPath ;
}

void CGlobal::setRegistCode(const QString &code)
{
    Config->setConfigVar("Setting/RegistCode", code);
}

QString CGlobal::registCode()
{
    QString code("");
    QString path = getCustomPath(QStandardPaths::AppDataLocation, "");
    path.remove("/5KPlayer");
    path.append("/5Kplayer/preferences.xml");
    QFile fi(path);
    if(fi.exists()) {
        if(fi.open(QIODevice::ReadOnly)) {
            while(!fi.atEnd()) {
                QByteArray line = fi.readLine().trimmed();
                int index1 = line.indexOf("<RegisterLicense>");
                int index2 = line.indexOf("</RegisterLicense>");
                if(index1 != -1 && index2 != -1) {
                    index1 = index1 + QString("<RegisterLicense>").length();
                    code = line.mid(index1, index2-index1);
                    break;
                }
            }
        }
    }
    if(!code.isEmpty()) {
        fi.close();
        fi.remove();
        setRegistCode(code);
        return code;
    }

    return Config->getConfigVar("Setting/RegistCode").toString();
}

bool CGlobal::checkRegistCode(const QString &code) const
{
    QFile fi(":/res/registCodeList.txt");
    if(!fi.open(QIODevice::ReadOnly)) {
        return false;
    }
    bool ok = false;
    while(!fi.atEnd()) {
        if(fi.readLine().trimmed() == code.toLatin1()) {
            ok = true;
            break;
        }
    }
    fi.close();
    return ok;
}

QString CGlobal::lineEditStyleSheet(const uint &v) const
{
    return QString("border-style:outset; border-radius: %1px;"
                   "border-width: 0px; background-color: white")
            .arg(v);
}

QString CGlobal::menuStyleSheet()
{
    return QString("QMenu {"
                   "background-color: white;" /* sets background of the menu 设置整个菜单区域的背景色*/
                   "border: 0px solid white;"/*整个菜单区域的边框粗细、样式、颜色*/
                   "}"

                   "QMenu::item {"
                   /* sets background of menu item. set this to something non-transparent
                                                     if you want menu color and menu item color to be different */
                   "background-color: transparent;"
                   "padding:%1px %2px;"/*设置菜单项文字上下和左右的内边距，效果就是菜单中的条目左右上下有了间隔*/
                   "margin:0px 0px;"/*设置菜单项的外边距*/
                   "}"

                   "QMenu::item:selected { "/* when user selects item using mouse or keyboard */
                   "background-color: #6b6c6d;"/*这一句是设置菜单项鼠标经过选中的样式*/
                   "}").arg(2*_screenRatio).arg(20*_screenRatio);
}

QString CGlobal::localName()
{
    static QString localName;
    if(localName.isEmpty())
    {
        localName = QHostInfo::localHostName();
    }
    return localName;
}

QString CGlobal::macAddress()
{
    static QString macAddr;
    if(macAddr.isEmpty())
    {
        foreach (QNetworkInterface inter, QNetworkInterface::allInterfaces())
        {
            macAddr = inter.hardwareAddress();
            if(!macAddr.isEmpty())
                break;
        }
    }
    return macAddr;
}

int CGlobal::winVersion()
{
    if(i_win_version == -1)
    {
        switch (QSysInfo::windowsVersion())
        {
        case QSysInfo::WV_WINDOWS10:  i_win_version = 10;break;
        case QSysInfo::WV_WINDOWS8_1: i_win_version = 9; break;
        case QSysInfo::WV_WINDOWS8:   i_win_version = 8; break;
        case QSysInfo::WV_WINDOWS7:   i_win_version = 7; break;
        case QSysInfo::WV_VISTA:      i_win_version = 6; break;
        case QSysInfo::WV_2003:       i_win_version = 5; break;
        case QSysInfo::WV_XP:         i_win_version = 4; break;
        default: i_win_version = -1;   break;
        }
    }
    return i_win_version;
}

QString CGlobal::liveUrl() const
{
    return _liveUrl;
}

QString CGlobal::liveName() const
{
    return _liveName;
}

void CGlobal::setLiveInfo(const QString &url, const QString &name)
{
    _liveUrl  = url;
    _liveName = name;
}

int CGlobal::ivalue(const QString &key) const
{
    const QVariant var = Config->getConfigVar(key);
    if(!var.isValid())
        return -1;
    return var.toInt();
}

QString CGlobal::getCustomPath(QStandardPaths::StandardLocation type, const QString &subPath) const
{
    return QStandardPaths::writableLocation(type).append(subPath);
}

void CGlobal::initLanguageList()
{
    _languageMap.clear();
    _languageMap["en"] = "English";
    _languageMap["jp"] = "日本語";
}

QString CGlobal::getValidLanguage(const QString &l)
{
    if(_languageMap.keys().indexOf(l) != -1)
        return l;
    return "en";
}

QString CGlobal::getVerWeb(QString web)
{
    if(!web.contains("ttsoft=",Qt::CaseInsensitive)) {
        return web;
    }
    QString reg = "-unreg";
    if(checkRegistCode(registCode())) {
        reg = "-freereg";
    }
    QString reWeb = QString(web).arg(getCurVersion().toString());

    return QString(reWeb).append(reg);
}

void CGlobal::initUrls()
{
    _urlHash.clear();
    _urlHash["support_en"] = "mailto:support@5kplayer.com";
    _urlHash["support_jp"] = "mailto:support-jp@5kplayer.com";
    _urlHash["support_zh"] = "mailto:support@5kplayer.com";
#ifdef Q_OS_WIN
    _urlHash["main_up"] = "http://www.5kplayer.com/upgrade/windows/upgrade5KPlayer.config";
    _urlHash["ad_conf"] = "http://dl1.5kplayer.com/5kpads/ad.config";
    _urlHash["ad_ini"]  = "http://dl1.5kplayer.com/5kpads/ads.ini";

    // 主界面广告
    _urlHash["mu_en"] = getVerWeb("https://www.videoproc.com/?ttsoft=5kpwin-%1-mu");
    _urlHash["mu_jp"] = getVerWeb("https://jp.videoproc.com/?ttsoft=5kpwin-%1-mu");
    _urlHash["mu_zh"] = getVerWeb("https://www.videoproc.com/index-zh.htm?ttsoft=5kpwin-%1-mu");

    // 编辑不支持解码广告
    _urlHash["edires_en"] = getVerWeb("https://www.videoproc.com/?ttsoft=5kpwin-%1-edires");
    _urlHash["edires_jp"] = getVerWeb("https://jp.videoproc.com/?ttsoft=5kpwin-%1-edires");
    _urlHash["edires_zh"] = getVerWeb("https://www.videoproc.com/index-zh.htm?ttsoft=5kpwin-%1-edires");

    _urlHash["edifile_en"] = getVerWeb("https://www.videoproc.com/?ttsoft=5kpwin-%1-edifile");
    _urlHash["edifile_jp"] = getVerWeb("https://jp.videoproc.com/?ttsoft=5kpwin-%1-edifile");
    _urlHash["edifile_zh"] = getVerWeb("https://www.videoproc.com/index-zh.htm?ttsoft=5kpwin-%1-edifile");

    // ytbMore
    _urlHash["ytbw_en"] = getVerWeb("https://www.5kplayer.com/youtube-download/online-video-sites-list.htm?ttsoft=5kpwin-%1-ytbw");
    _urlHash["ytbw_jp"] = getVerWeb("https://www.5kplayer.com/youtube-download-jp/online-video-sites-list.htm?ttsoft=5kpwin-%1-ytbw");
    _urlHash["ytbw_zh"] = getVerWeb("https://www.5kplayer.com/youtube-download-zh/online-video-sites-list.htm?ttsoft=5kpwin-%1-ytbw");

    // aboutDialog上的链接
    _urlHash["minfo_en"] = getVerWeb("https://www.5kplayer.com/?ttsoft=5kpwin-%1-minfo");
    _urlHash["minfo_jp"] = getVerWeb("https://www.5kplayer.com/index-jp.htm?ttsoft=5kpwin-%1-minfo");
    _urlHash["minfo_zh"] = getVerWeb("https://www.5kplayer.com/index-zh.htm?ttsoft=5kpwin-%1-minfo");

    // DVD界面上的下载按钮的链接
    _urlHash["ldvd_en"] = getVerWeb("https://www.5kplayer.com/software/rip-dvd.htm?ttsoft=5kpwin-%1-ldvd");
    _urlHash["ldvd_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-jp.htm?ttsoft=5kpwin-%1-ldvd");
    _urlHash["ldvd_zh"] = getVerWeb("https://www.5kplayer.com/software/rip-dvd-zh.htm?ttsoft=5kpwin-%1-ldvd");

    _urlHash["ug_en"] = getVerWeb("https://www.5kplayer.com/user-guide/windows-how-to.htm?ttsoft=5kpwin-%1-ug");
    _urlHash["ug_jp"] = getVerWeb("https://www.5kplayer.com/user-guide/windows-how-to-jp.htm?ttsoft=5kpwin-%1-ug");
    _urlHash["ug_zh"] = getVerWeb("https://www.5kplayer.com/user-guide/windows-how-to.htm?ttsoft=5kpwin-%1-ug");

    //右键convert
    _urlHash["plc_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video.htm?ttsoft=5kpwin-%1-plc");
    _urlHash["plc_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-jp.htm?ttsoft=5kpwin-%1-plc");
    _urlHash["plc_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-zh.htm?ttsoft=5kpwin-%1-plc");

    _urlHash["pac_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video.htm?ttsoft=5kpwin-%1-pac");
    _urlHash["pac_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-jp.htm?ttsoft=5kpwin-%1-pac");
    _urlHash["pac_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-zh.htm?ttsoft=5kpwin-%1-pac");

    _urlHash["pic_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video.htm?ttsoft=5kpwin-%1-pic");
    _urlHash["pic_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-jp.htm?ttsoft=5kpwin-%1-pic");
    _urlHash["pic_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-zh.htm?ttsoft=5kpwin-%1-pic");

    _urlHash["pdc_en"] = getVerWeb("https://www.5kplayer.com/software/rip-dvd.htm?ttsoft=5kpwin-%1-pdc");
    _urlHash["pdc_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-jp.htm?ttsoft=5kpwin-%1-pdc");
    _urlHash["pdc_zh"] = getVerWeb("https://www.5kplayer.com/software/rip-dvd-zh.htm?ttsoft=5kpwin-%1-pdc");

    _urlHash["pvc_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video.htm?ttsoft=5kpwin-%1-pvc");
    _urlHash["pvc_jp"] = getVerWeb("https://www.5kplayer.com/software/youtube-video-convert-jp.htm?ttsoft=5kpwin-%1-pvc");
    _urlHash["pvc_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-zh.htm?ttsoft=5kpwin-%1-pvc");

    _urlHash["run_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video.htm?ttsoft=5kpwin-%1-run");
    _urlHash["run_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-jp.htm?ttsoft=5kpwin-%1-run");
    _urlHash["run_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-zh.htm?ttsoft=5kpwin-%1-run");

    //convert
    _urlHash["ycv_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video.htm?ttsoft=5kpwin-%1-ycv");
    _urlHash["ycv_jp"] = getVerWeb("https://www.5kplayer.com/software/youtube-video-convert-jp.htm?ttsoft=5kpwin-%1-ycv");
    _urlHash["ycv_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-zh.htm?ttsoft=5kpwin-%1-ycv");

    _urlHash["ansb_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video.htm?ttsoft=5kpwin-%1-ansb");
    _urlHash["ansb_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-jp.htm?ttsoft=5kpwin-%1-ansb");
    _urlHash["ansb_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-zh.htm?ttsoft=5kpwin-%1-ansb");

    _urlHash["dlna_en"] = getVerWeb("https://www.5kplayer.com/software/convert-video.htm?ttsoft=5kpwin-%1-dlna");
    _urlHash["dlna_jp"] = getVerWeb("https://www.5kplayer.com/software/youtube-video-convert-jp.htm?ttsoft=5kpwin-%1-dlna");
    _urlHash["dlna_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-video-zh.htm?ttsoft=5kpwin-%1-dlna");
#else
    _urlHash["main_up"] = "http://www.5kplayer.com/upgrade/mac/upgrade5KPlayer.config";
    _urlHash["ad_conf"] = "http://dl1.5kplayer.com/5kp-mac-ads/ad.config";
    _urlHash["ad_ini"]  = "http://dl1.5kplayer.com/5kp-mac-ads/ads.ini";

    // 主界面广告
    _urlHash["mu_en"] = getVerWeb("https://www.videoproc.com/?ttsoft=5kpmac-%1-mu");
    _urlHash["mu_jp"] = getVerWeb("https://jp.videoproc.com/?ttsoft=5kpmac-%1-mu");
    _urlHash["mu_zh"] = getVerWeb("https://www.videoproc.com/index-zh.htm?ttsoft=5kpmac-%1-mu");

    _urlHash["edires_en"] = getVerWeb("https://www.videoproc.com/?ttsoft=5kpmac-%1-edires");
    _urlHash["edires_jp"] = getVerWeb("https://jp.videoproc.com/?ttsoft=5kpmac-%1-edires");
    _urlHash["edires_zh"] = getVerWeb("https://www.videoproc.com/index-zh.htm?ttsoft=5kpmac-%1-edires");

    _urlHash["edifile_en"] = getVerWeb("https://www.videoproc.com/?ttsoft=5kpmac-%1-edifile");
    _urlHash["edifile_jp"] = getVerWeb("https://jp.videoproc.com/?ttsoft=5kpmac-%1-edifile");
    _urlHash["edifile_zh"] = getVerWeb("https://www.videoproc.com/index-zh.htm?ttsoft=5kpmac-%1-edifile");

    // ytbMore
    _urlHash["ytbw_en"] = getVerWeb("https://www.5kplayer.com/youtube-download/online-video-sites-list.htm?ttsoft=5kpmac-%1-ytbw");
    _urlHash["ytbw_jp"] = getVerWeb("https://www.5kplayer.com/youtube-download-jp/online-video-sites-list-mac.htm?ttsoft=5kpmac-%1-ytbw");
    _urlHash["ytbw_zh"] = getVerWeb("https://www.5kplayer.com/youtube-download-zh/online-video-sites-list.htm?ttsoft=5kpmac-%1-ytbw");

    // about 界面的link
    _urlHash["minfo_en"] = getVerWeb("https://www.5kplayer.com/index.htm?ttsoft=5kpmac-%1-minfo");
    _urlHash["minfo_jp"] = getVerWeb("https://www.5kplayer.com/index-jp.htm?ttsoft=5kpmac-%1-minfo");
    _urlHash["minfo_zh"] = getVerWeb("https://www.5kplayer.com/index-zh.htm?ttsoft=5kpmac-%1-minfo");

    _urlHash["ldvd_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-ldvd");
    _urlHash["ldvd_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-ldvd");
    _urlHash["ldvd_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-ldvd");

    _urlHash["ug_en"] = getVerWeb("https://www.5kplayer.com/user-guide/index.htm?ttsoft=5kpmac-%1-ug");
    _urlHash["ug_jp"] = getVerWeb("https://www.5kplayer.com/user-guide/index-jp.htm?ttsoft=5kpmac-%1-ug");
    _urlHash["ug_zh"] = getVerWeb("https://www.5kplayer.com/user-guide/index.htm?ttsoft=5kpmac-%1-ug");

    //右键convert
    _urlHash["plc_en"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-plc");
    _urlHash["plc_jp"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-plc");
    _urlHash["plc_zh"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-plc");

    _urlHash["pac_en"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-pac");
    _urlHash["pac_jp"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-pac");
    _urlHash["pac_zh"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-pac");

    _urlHash["pic_en"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-pic");
    _urlHash["pic_jp"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-pic");
    _urlHash["pic_zh"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-pic");

    _urlHash["pdc_en"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-pdc");
    _urlHash["pdc_jp"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-pdc");
    _urlHash["pdc_zh"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-pdc");

    _urlHash["pvc_en"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-pvc");
    _urlHash["pvc_jp"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-pvc");
    _urlHash["pvc_zh"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-pvc");

    _urlHash["run_en"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-run");
    _urlHash["run_jp"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-run");
    _urlHash["run_zh"]  = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-run");

    //convert
    _urlHash["ycv_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-ycv");
    _urlHash["ycv_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-ycv");
    _urlHash["ycv_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-ycv");

    _urlHash["ansb_en"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac.htm?ttsoft=5kpmac-%1-ansb");
    _urlHash["ansb_jp"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-jp.htm?ttsoft=5kpmac-%1-ansb");
    _urlHash["ansb_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-dvd-video-mac-zh.htm?ttsoft=5kpmac-%1-ansb");

    _urlHash["dlna_en"] = getVerWeb("https://www.5kplayer.com/software/convert-video-mac.htm?ttsoft=5kpmac-%1-dlna");
    _urlHash["dlna_jp"] = getVerWeb("https://www.5kplayer.com/software/youtube-video-convert-mac-jp.htm?ttsoft=5kpmac-%1-dlna");
    _urlHash["dlna_zh"] = getVerWeb("https://www.5kplayer.com/software/convert-video-mac-zh.htm?ttsoft=5kpmac-%1-dlna");
#endif
}

void CGlobal::openLanguageUrl(const QString &key)
{
    QString tKey(key);
    if(!_urlHash.contains(key)) {
        tKey = key + "_" + getLanguage();
    }

    if(_urlHash.contains(tKey)) {
        OpenFileDialog->openUrl(_urlHash.value(tKey));
    }
}

QString CGlobal::getLanguageUrl(const QString &key)
{
    QString url = "";
    if(_urlHash.contains(key)) {
        url =  _urlHash.value(key);
    }
    return url;
}

QString CGlobal::getuserYtbPaht()
{
    QString userYtbPaht = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    userYtbPaht.append("/pack");
    return userYtbPaht;
}

QString CGlobal::milliToTime(const int &msd) const
{
    if(msd <= 0) return "00:00:00";

    const int tSecond = msd / 1000;

    const int hour = tSecond/3600;
    const int minute = tSecond%3600/60;
    const int second = tSecond%3600%60;

    QString hourStr   = QString::number(hour).rightJustified(2, '0');
    QString minuteStr = QString::number(minute).rightJustified(2, '0');
    QString secondStr = QString::number(second).rightJustified(2, '0');
    return QString("%1:%2:%3").arg(hourStr).arg(minuteStr).arg(secondStr);
}

QString CGlobal::secondToTime(const int &sd) const
{
    if(sd <= 0) return "00:00:00";

    const int hour = sd/3600;
    const int minute = sd%3600/60;
    const int second = sd%3600%60;

    QString hourStr   = QString::number(hour).rightJustified(2, '0');
    QString minuteStr = QString::number(minute).rightJustified(2, '0');
    QString secondStr = QString::number(second).rightJustified(2, '0');
    return QString("%1:%2:%3").arg(hourStr).arg(minuteStr).arg(secondStr);
}

int CGlobal::bonjourServiceState()
{
    SC_HANDLE scHandle = OpenSCManager(NULL, NULL, GENERIC_READ);//SC_MANAGER_ALL_ACCESS
    if (scHandle == NULL) {
        return -1;
    }

    LPCTSTR lpServiceName = "Bonjour Service";
    SC_HANDLE schService = OpenService(scHandle, lpServiceName, SERVICE_QUERY_STATUS);
    if(schService == NULL) {
        CloseServiceHandle(scHandle);
        return -2;
    }

    DWORD dwBytesNeeded;
    SERVICE_STATUS_PROCESS ssStatus;
    BOOL b = QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssStatus,
                                  sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded);

    CloseServiceHandle(schService);
    CloseServiceHandle(scHandle);
    if(b == FALSE) {
        return -3;
    }

    int state = -1;
    switch(ssStatus.dwCurrentState)
    {
    case SERVICE_STOPPED:
    case SERVICE_STOP_PENDING:
        state = 0;
        break;
    case SERVICE_PAUSED:
    case SERVICE_PAUSE_PENDING:
        state = 2;
        break;
    case SERVICE_CONTINUE_PENDING:
    case SERVICE_RUNNING:
    case SERVICE_START_PENDING:
        state = 1;
        break;
    }
    return state;
}

int CGlobal::checkLastString(QString soure, QString str)
{
    int i=0;
    int k=0;
    for(;i<soure.size();) {
        k = soure.indexOf(str,i);
        if(k != -1) {
            i = k+1;
        } else {
            break;
        }
    }
    return i;
}

bool CGlobal::Extract(const QString &in_file_path, const QString &out_file_path)
{
    QuaZip archive(in_file_path);
    if (!archive.open(QuaZip::mdUnzip))
        return false;

    QString path = out_file_path;
    if (!path.endsWith("/") && !out_file_path.endsWith("\\"))
        path += "/";

    QDir dir(out_file_path);
    if (!dir.exists())
        dir.mkpath(out_file_path);

    for( bool f = archive.goToFirstFile(); f; f = archive.goToNextFile() )
    {
        QString filePath = archive.getCurrentFileName();
        QuaZipFile zFile(archive.getZipName(), filePath);
        zFile.open(QIODevice::ReadOnly );
        QByteArray ba = zFile.readAll();
        zFile.close();

        int mid = checkLastString(filePath,"/");
        if (mid > 0)
        {
            QString newPath = filePath.mid(0,mid);
            if(!newPath.isEmpty())
                dir.mkpath(newPath);
        }
        if (filePath.endsWith("/"))
        {
            dir.mkpath(filePath);
        } else {
            QFile dstFile(path + filePath);
            if (!dstFile.open(QIODevice::WriteOnly))
                return false;
            dstFile.write(ba);
            dstFile.close();
        }
    }

    return true;
}

void CGlobal::saveShowAdsTime(QDateTime time)
{
    _adsShowTime = time.toString("yyyy-MM-dd hh-mm-ss");
    Config->setConfigVar("AD/ShowTime", _adsShowTime);
}

QString CGlobal::getShowAdsTime()
{
    if(_adsShowTime.isEmpty()) {
        if(Config->getConfigVar("AD/ShowTime").isValid()) {
            _adsShowTime = Config->getConfigVar("AD/ShowTime").toString();
        } else {
            _adsShowTime = "2018-01-01 00-00-00";
        }
    }
    return _adsShowTime;
}

void CGlobal::saveAdsConfigTime(QDateTime time)
{
    _adsConfigTime = time.toString("yyyy-MM-dd hh-mm-ss");
    Config->setConfigVar("AD/ConfigUpTime", _adsConfigTime);
}

QString CGlobal::getAdsConfigTime()
{
    if(_adsConfigTime.isEmpty()) {
        if(Config->getConfigVar("AD/ConfigUpTime").isValid()) {
            _adsConfigTime = Config->getConfigVar("AD/ConfigUpTime").toString();
        } else {
            _adsConfigTime = "2017-01-01 00-00-00";
        }
    }
    return _adsConfigTime;
}

void CGlobal::saveAdiniUpTime(QDateTime time)
{
    _adiniUpTime = time.toString("yyyy-MM-dd hh-mm-ss");
    Config->setConfigVar("AD/IniUpTime", _adiniUpTime);
}

QString CGlobal::getAdiniUpTime()
{
    if(_adiniUpTime.isEmpty()) {
        if(Config->getConfigVar("AD/IniUpTime").isValid()) {
            _adiniUpTime = Config->getConfigVar("AD/IniUpTime").toString();
        } else {
            _adiniUpTime = "2017-01-01 00-00-00";
        }
    }
    return _adiniUpTime;
}

void CGlobal::saveNoshowAdNumber()
{
    QStringList noShowNumbers;
    noShowNumbers.clear();
    for(int i=0;i<_noShowAdNumbers.size();i++) {
        noShowNumbers.append(QString::number(_noShowAdNumbers.at(i)));
    }
    Config->setConfigVar("AD/NoShows", noShowNumbers);
}

void CGlobal::initNoshowAdNumber()
{
    _noShowAdNumbers.clear();
    QStringList noShowList = Config->getConfigVar("AD/NoShows").toStringList();

    for(int i=0;i<noShowList.size();i++) {
        int number = QString(noShowList.at(i)).toInt();
        if(number > 0) {
            _noShowAdNumbers.append(number);
        }
    }
}

QList<int> CGlobal::getNoshowAdNumber()
{
    return _noShowAdNumbers;
}

void CGlobal::addNoShowAdNumber(int number)
{
    // 理论上不会出现重复
    bool isExit = false;
    for(int i=0;i<_noShowAdNumbers.size();i++) {
        if(_noShowAdNumbers.at(i) == number) {
            isExit = true;
            break;
        }
    }
    if(!isExit) {
        _noShowAdNumbers.append(number);
        saveNoshowAdNumber();
    }
}
