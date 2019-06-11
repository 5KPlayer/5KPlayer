#include "openfile.h"
#include "cdialog.h"
#include "cdialog2.h"
#include "radiowidget.h"
#include "playermainwidget.h"
#include "librarydata.h"
#include "globalarg.h"
#include "videotunerdialog.h"
#include <QScreen>

#include "dymediaplayer.h"
#include "dvdplaywidget.h"
#include "mediainfowidget.h"
#include "aboutwidget.h"
#include "synctrackwidget.h"
#include "settingwidget.h"
#include "hardwarewidget.h"
#include "medianosupairplay.h"
#include "updatewidget.h"
#include "language.h"
#include "configure.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "language.h"
#include "siglogdialog.h"
#include "mediaaddwidget.h"
#include "mediaexitdialog.h"
#include "delmediawidget.h"
#include "decodeccheck.h"
#include "urlanalyzeexit.h"
#include "ytbdeploywidget.h"
#include "cmenu.h"
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include <QHostInfo>

#define PMW_RECT PMW->x(),     PMW->y() + PMW->headHeight(), \
    PMW->width(), PMW->height()-PMW->headHeight()

#define DIA_RECT PMW->x() + (PMW->width() - dia.width()*Global->_screenRatio)/2, \
    PMW->y() + PMW->headHeight() /2 + (PMW->height() - dia.height()*Global->_screenRatio)/2

#define P_DIA_RECT PMW->x() + (PMW->width() - p_dia->width()*Global->_screenRatio)/2, \
    PMW->y() + PMW->headHeight() / 2 + (PMW->height() - p_dia->height()*Global->_screenRatio)/2

#define Parent PMW->mainWidget()

int i_count = 0;

class CTmp
{
public:
    CTmp(){
        ++i_count;
        //OpenFileDialog->closeDialog();
        PMW->releaseMouse();
        PMW->setPopDialog(true);
        PMW->showMainWidget();
    }
    ~CTmp(){
        --i_count;
        if(i_count == 0) {
            PMW->setPopDialog(false);
            PMW->showMainWidget();
        }
    }
};

OPenFile::OPenFile(QObject *parent) : QObject(parent)
{
    p_httpServer = NULL;
    b_systemDialog = false;
    resetType();
}

#include <QDateTime>
void OPenFile::openLocalSocketFile(const QString &file)
{
    static QDateTime dt = QDateTime::currentDateTime();

    QDateTime t_dt = QDateTime::currentDateTime();

    const qint64 m = dt.msecsTo(t_dt);
    if(m < 500 && m > 5)
        return;

    dt = t_dt;

    qDebug() << "recv: " << file;

    if(file == "exit_9")
    {
        PMW->closeWithConfig();
        return;
    }

    if(file == "-auto" || file == "/auto" || file == "auto")
        return;

    PMW->showOut();
    LibData->b_doubleClickPlay = true;
    openFile(file);
}

void OPenFile::openFile(const QString &file)
{
    if(!PMW->isInit()) return;
    qDebug() << "openFile: " << file;
    if(file.isEmpty() || file == "-auto" || file == "/auto" ||
            file == "auto" || file == "-show" || file.left(1) == "-")
        return;

    QFileInfo fi(file);
    if(!fi.exists()) {
        QString filelog = "\"" + file + "\" ";
        filelog.append(Lang("RecentFile/info"));
        openTooltipDialog(filelog);
        return;
    }

    PMW->release();
    //判断是不是根目录 如果是  说明是DVD
    if(fi.isDir() && file.length() <= 3 && QFileInfo::exists(file + "VIDEO_TS")) {
        if(LibData->b_isConCPT) {
            openMediaNotSupAirPlay(file);
        } else {
            setPlayType(DVD);
            //清空library播放状态
            LibData->clearPlayinfo();
            MediaPlayer->playFile("dvd:///" + file, false);
        }
        return;
    }

    if(Global->mirrorFiles().contains( fi.suffix().toLower() ))
    {
        if(LibData->b_isConCPT) {
            openMediaNotSupAirPlay(file);
        } else {
            setPlayType(DVDMirror);
            //清空library播放状态
            LibData->clearPlayinfo();
            playFile(file);
        }
    }
    else if(fi.isFile() && LibData->addmedia(file) == true)
    {
        MEDIAINFO info = LibData->getplayinfo();
        if(LibData->b_isConCPT) {
            if(LibData->checkMediaisAirPlay(info)) {
                OpenFileDialog->setPlayType(OPenFile::CONNECTION);
                OpenFileDialog->sendNetworkInfo(file);
            } else {
                LibData->clearPlayinfo();
                openMediaNotSupAirPlay(file);
            }
        } else {
            if(info.mediaType == 1) {//video
                setPlayType(Video);
                LibData->setplayItem("pMovies");
            } else if(info.mediaType == 2) {
                setPlayType(Music);
                LibData->setplayItem("pMusic");
            }

            playFile(file);
            LibData->addRecent(file);
        }
    }
}

void OPenFile::setPlayType(const OPenFile::PlayType &t)
{
    _curType = t;
}

void OPenFile::playFile(const QString &file)
{
    qDebug() << "play File:" << file;
    LibData->disScreenprotection();
    MediaPlayer->playFile(file);
}

//如果开启airplay并且连接到其他电脑 ，给对方发送相关数据，本机设置为响应模式(设置/同步 进度、声音、控制(pause play stop))
void OPenFile::sendNetworkInfo(const QString &file, const QSize &/*dar*/, const QString &radio)
{
    QFile fileinfo(file);
    if(!fileinfo.exists())
        return;
    if(_curType == CONNECTION)
    {
        //httpServer 设置文件
        if(p_httpServer == NULL)
        {
            p_httpServer = new TcpServer(this);
            p_httpServer->listenPort(8866); // 最大 65535
        }
        p_httpServer->setFileName(file);

        QString msg;
        if(radio.isEmpty()) // music
        {
            //msg = QString("{\"port\":\"8866\",\"type\":\"music\"}");
            msg = "bf:8866/type=music";
        }
        else
        {
            /*
            msg = QString("{\"port\":\"8866\",\"type\":\"video\","
                          "\"radio\":\"%1\",\"dar\":\"%2x%3\"}")
                    .arg(radio)
                    .arg(dar.width())
                    .arg(dar.height());
            */
            msg = "bf:8866/type=video&radio=16:9&dar=1920x1080";
        }

        //{"url":"http://192.168.2.42:8866","type":"video","radio":"16:9","dar":"1920X1080"}
        // UDP 发送信息 http://192.168.2.42:8866?type=video&radio=16:9&dar=1920X1080

        // http://192.168.2.42:8866?type=music
        //{"url":"http://192.168.2.42:8866","type":"music"}

        UdpSev->sendMsgToConCPT(msg);

        LibData->disScreenprotection();
        //控制台 DPASUAED
        PMW->startConnectedMode();
        return;
    }
}

bool OPenFile::closeDialog()
{
    bool brt = false;
    if(dialogs.count() > 0)
        brt = true;
    foreach (QDialog *dia, dialogs) {
        dia->reject();
    }
    return brt;
}

void OPenFile::updateDialog2()
{
    foreach (QDialog *d, dialogs) {
        CDialog2 * t_dialog = qobject_cast<CDialog2 *>(d);
        if(t_dialog) {
            t_dialog->updateUI(Global->_screenRatio);
            //t_dialog->initGeometry(PMW_RECT);
            t_dialog->setFocus();
        }
    }
}

bool OPenFile::isOpenDialog() const
{
    if(dialogs.count() > 0)
        return true;
    return false;
}

void OPenFile::addMediaDialog(const QStringList &pathList)
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(false,Parent);

    dialogs << &dia;

    mediaAddWidget *w = new mediaAddWidget(&dia);
    w->setAddfilePath(pathList);
    connect(w,SIGNAL(stopanalyze()),LibData,SIGNAL(stopAnalyzeFile()));
    connect(LibData,SIGNAL(analyzefinish()),w,SLOT(loadfinish()));
    connect(LibData,SIGNAL(upAnalyzeInfo(QList<MEDIAINFO>)),w,SLOT(upLoadWidget(QList<MEDIAINFO>)));
    dia.setNeedSpace(false);
    dia.setContentWidget(w);
    dia.setNoClose(false);
    dia.resize(500,140);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

int OPenFile::mediaExitDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(true,Parent);
    dialogs << &dia;
    MediaExitDialog *w = new MediaExitDialog(&dia);
    dia.setContentWidget(w);
    dia.resize(500,200);
    dia.move(DIA_RECT);
    int res = dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
    return res;
}

int OPenFile::supDecodecDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(true,Parent);
    dialogs << &dia;
    DecodecCheck *w = new DecodecCheck(&dia);
    dia.setContentWidget(w);
    dia.resize(500,200);
    dia.move(DIA_RECT);
    dia.exec();
    int res = w->getSupQsv();

    dialogs.removeOne(&dia);
    obj.clear();
    return res;
}

int OPenFile::mediaDelDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(true,Parent);
    dialogs << &dia;

    DelMediaWidget *w = new DelMediaWidget(&dia);
    dia.setContentWidget(w);
    dia.resize(500,200);
    dia.move(DIA_RECT);
    int res = dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
    return res;
}

int OPenFile::urlExitDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(true,Parent);
    dialogs << &dia;

    UrlAnalyzeExit *w = new UrlAnalyzeExit(&dia);
    dia.setContentWidget(w);
    dia.resize(450,180);
    dia.move(DIA_RECT);
    int res = dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
    return res;
}

int OPenFile::ytbDeploy()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(true,Parent);
    dialogs << &dia;

    YtbDeployWidget *w = new YtbDeployWidget(&dia);
    dia.setContentWidget(w);
    dia.resize(450,180);
    dia.move(DIA_RECT);
    int res = dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
    return res;
}

void OPenFile::openVideoDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    const QString path = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    const QString file = getFileName(Lang("OpenFile/opvideo"), path,
                                     Lang("OpenFile/videoFilt") + "(" +
                                     Config->getUserVideoFormat() + ");;" +
                                     Lang("OpenFile/allfile"));
    if(file.isEmpty())
        return;

    openFile(file);
}

void OPenFile::openMusicDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    const QString path = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    const QString file = getFileName( Lang("OpenFile/opmusic"), path,
                                      Lang("OpenFile/musicFilt") + "(" +
                                      Config->getUserAudioFormat() + ");;" +
                                      Lang("OpenFile/allfile"));
    if(file.isEmpty())
        return;

    openFile(file);
}

void OPenFile::openDvdDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog2 dia(Lang("DVD/title"), Parent);
    dialogs << &dia;

    DVDPlayWidget *w = new DVDPlayWidget;

    dia.setContentWidget(w);
    dia.initGeometry(PMW_RECT);

    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

QString OPenFile::openDvdFileDialog()
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    return getFileName(Lang("OpenFile/dvdtitle"), path, Lang("OpenFile/dvdfile"));
}

void OPenFile::openRadioDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);


    CDialog2 dia(Lang("OpenFile/live"), Parent);
    dialogs << &dia;

    RadioWidget *w = new RadioWidget;

    dia.setContentWidget(w);
    dia.initGeometry(PMW_RECT);

    dia.exec();
    dialogs.removeOne(&dia);
    obj.clear();
}

#include "settingairlaywidget.h"
void OPenFile::openAirPlayDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(false, Parent);
    dialogs << &dia;

    SettingWidget *w = new SettingWidget("AirPlay");

    dia.setContentWidget(w);
    dia.disableMove();
    dia.setNeedSpace(false);
    dia.resize(900,560);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openVideoTSDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    const QString dirStr = getDir(Lang("Menu/OVideoTS"), "");
    if(dirStr.isEmpty())
        return;

    OpenFileDialog->setPlayType(OPenFile::DVD);
    MediaPlayer->playFile("dvd:///" + dirStr, false);
}

void OPenFile::openSubtitleFileDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);
    const QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    const QString file = getFileName(Lang("OpenFile/selSub"), path,
                                     Lang("OpenFile/subFor"));
    if(file.isEmpty())
        return;

    if(Menu->isLoadSubtitle(QDir::toNativeSeparators(file))) {
        // 选择字幕已经载入，选择字幕
        Menu->selectSubtitle(QDir::toNativeSeparators(file));
    } else {
        // 新加入并选择
        Menu->loadSubpath2Subtitle(QDir::toNativeSeparators(file));
    }

    //MediaPlayer->video()->setSubtitleFile(QDir::toNativeSeparators(file));
}

void OPenFile::openSettingsDialog()
{
    if(dialogs.count() > 0) {
        PMW->activateWidget();
        return;
    }
    //防止打开对话框  隐藏定时器还在继续
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);
    PMW->showOut();

    CDialog dia(false, Parent);
    dialogs << &dia;

    SettingWidget *w = new SettingWidget;

    dia.setContentWidget(w);
    dia.disableMove();
    dia.setNeedSpace(false);
    dia.resize(900,560);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openTrackSyncDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(false, Parent);
    dialogs << &dia;
    dia.setTitle(Lang("TrackSyn/title"));

    SyncTrackWidget *w = new SyncTrackWidget;
    dia.setContentWidget(w);
    dia.resize(w->width() + 16, w->height()+40);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openVideoTunerDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(false, Parent);
    dialogs << &dia;
    dia.setTitle(Lang("Videotuner/title"));
    VideoTunerDialog *w = new VideoTunerDialog;
    dia.setContentWidget(w);
    dia.resize(w->width() + 16, w->height()+40);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openMediaInfoDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(false, Parent);
    dialogs << &dia;
    dia.setTitle(Lang("MediaInfo/title"));
    MediaInfoWidget *w = new MediaInfoWidget;
    w->resize(460, 300);
    dia.setContentWidget(w);

    dia.resize(w->width() + 16, w->height()+40);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openHWAccelDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(false, Parent);
    dialogs << &dia;

    SettingWidget *w = new SettingWidget("Headware");

    dia.setContentWidget(w);
    dia.disableMove();
    dia.setNeedSpace(false);
    dia.resize(900,560);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openMediaNotSupAirPlay(const QString &file)
{
    CDialog dia(true);
    MediaNoSupAirPlay *w = new MediaNoSupAirPlay(&dia, file);
    dia.setContentWidget(w);
    dia.resize(500,200);
    dia.move(DIA_RECT);
    dia.exec();
}

void OPenFile::openAboutDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(false, Parent);
    dialogs << &dia;
    dia.setTitle(Lang("about/title"));

    AboutWidget *w = new AboutWidget;
    dia.setContentWidget(w);

    dia.resize(w->width() + 16, w->height()+40);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}
#include "registwidet.h"
void OPenFile::openRegisterDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog dia(false, Parent);
    dialogs << &dia;
    dia.setTitle(Lang("Register/title"));

    RegistWidet *w = new RegistWidet;
    dia.setContentWidget(w);

    dia.resize(460, 456);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openUpdateDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    Global->setMainUpFlag(false);
    CDialog2 dia(Lang("UpDate/title"), Parent);
    dialogs << &dia;

    UpdateWidget *w = new UpdateWidget;

    dia.setContentWidget(w);
    dia.initGeometry(PMW_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openAutoUpdateDialog(QByteArray data)
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    Global->setMainUpFlag(false);
    CDialog2 dia(Lang("UpDate/title"), Parent);
    dialogs << &dia;

    UpdateWidget *w = new UpdateWidget(false);
    w->finishNeedUp(data);
    dia.setContentWidget(w);
    dia.initGeometry(PMW_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

#include "filerelationwidget.h"
void OPenFile::openRelationDialog()
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);

    CDialog2 dia(Lang("Relation/title"), Parent);
    dialogs << &dia;

    FileRelationWidget *w = new FileRelationWidget;

    dia.setContentWidget(w);
    dia.initGeometry(PMW_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openTooltipDialog(const QString &msg)
{
    QSharedPointer<CTmp> obj = QSharedPointer<CTmp>(new CTmp);
    CDialog dia(true,Parent);
    dialogs << &dia;
    SigLogDialog *w = new SigLogDialog(&dia);
    w->setLogInfo(msg);
    dia.setContentWidget(w);
    dia.resize(500,170);
    dia.move(DIA_RECT);
    dia.exec();

    dialogs.removeOne(&dia);
    obj.clear();
}

void OPenFile::openHelpUrl()
{
    Global->openLanguageUrl("help");
}

void OPenFile::openConvertUrl()
{
    QString urlValue;
    switch (OpenFileDialog->curPlayType()) {
    case OPenFile::DVD:
        urlValue = "r_convert_dvd";
        break;
    case OPenFile::DVDMirror:
        urlValue = "r_convert_iso";
        break;
    case OPenFile::Video:
        urlValue = "r_convert_video";
        break;
    case OPenFile::Music:
        urlValue = "r_convert_audio";
        break;
    case OPenFile::Live:
        urlValue = "r_convert_live";
        break;
    default:
        urlValue = "r_convert_main";
        break;
    }
    Global->openLanguageUrl(urlValue);
}

void OPenFile::openSendEmail()
{
    Global->openLanguageUrl("support");
}

void OPenFile::openFreeDownload()
{
    Global->openLanguageUrl("download");
}

void OPenFile::openUrl(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

QString OPenFile::getFileName(const QString caption, const QString &path, const QString &filter)
{
    b_systemDialog = true;
    QString file = QFileDialog::getOpenFileName(NULL, caption, path, filter);
    b_systemDialog = false;
    return file;
}

QStringList OPenFile::getFileNames(const QString caption, const QString &path, const QString &filter)
{
    b_systemDialog = true;
    QStringList list = QFileDialog::getOpenFileNames(NULL, caption, path, filter);
    b_systemDialog = false;

    list.sort();
    return list;
}

QString OPenFile::getDir(const QString caption, const QString &path)
{
    b_systemDialog = true;
    const QString dir = QFileDialog::getExistingDirectory(NULL, caption, path);
    b_systemDialog = false;
    return dir;
}
