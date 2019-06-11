#ifndef OPENFILE_H
#define OPENFILE_H

#include <QObject>
#include <QSize>
#include "singleton.h"

class QDialog;
class TcpServer;

#define OpenFileDialog OPenFile::getInstance()

class OPenFile : public QObject, public Singleton<OPenFile>
{
    Q_OBJECT
    friend class Singleton<OPenFile>;
    friend class QSharedPointer<OPenFile>;

public:
    enum PlayType {
        Unknown, Video, Music, NetWork, Live, //包含了airplay-video模式,都是直接播放地址
        DVD, DVDMirror ,AIRPLAY_MUSIC, //只做显示切换,无交互
        CONNECTION, //连接模式,连接其他电脑
    };

    //重置
    void resetType() {
        _curType = Unknown;
    }

    //设置播放类型
    void setPlayType(const PlayType &t);

    //当前类型
    PlayType curPlayType() const {
        return _curType;
    }

    //播放指定文件
    void playFile(const QString &file);

    void sendNetworkInfo(const QString &file, const QSize &dar=QSize(0,0), const QString &radio="");

    bool closeDialog();
    void updateDialog2();

    bool isFile();
    bool isOpenDialog() const;

    void addMediaDialog(const QStringList &pathList);      // 添加媒体列表的dialog
    int  mediaExitDialog();                         // 双击播放媒体不存在的dialog
    int  supDecodecDialog();                        // 检测支持的硬件加速的dialog
    int  mediaDelDialog();                          // 删除媒体确认的dialog
    int  urlExitDialog();                           // 解析视频已经存在于列表的dialog
    int  ytbDeploy();                               // ytb脚本不存在的dialog

    bool isOpenSysDialog() const {return b_systemDialog;}

private:
    Q_DISABLE_COPY(OPenFile)
    explicit OPenFile(QObject *parent = 0);
    ~OPenFile(){}

public slots:
    void openFile(const QString &file);
    void openLocalSocketFile(const QString &file);//双击资源管理器 播放的文件

    void openVideoDialog();//视频文件对话框
    void openDvdDialog();  //DVD对话框
    QString openDvdFileDialog();
    void openMusicDialog();//音乐文件对话框
    void openRadioDialog();//radio对话框
    void openAirPlayDialog();//Airplay对话框
    void openVideoTSDialog();    //目录对话框
    void openSubtitleFileDialog();//字幕文件对话框

    void openSettingsDialog();    //设置对话框
    void openTrackSyncDialog();   //轨道延时对话框
    void openVideoTunerDialog();  //打开视频调谐器对话框
    void openMediaInfoDialog();   //MediaInfo
    void openHWAccelDialog();     //打开硬件加速模块
    void openMediaNotSupAirPlay(const QString &file);//打开airplay不支持播放的dialog

    void openAboutDialog();   //about
    void openRegisterDialog();//注册
    void openUpdateDialog();  //升级界面
    void openAutoUpdateDialog(QByteArray data);
    void openRelationDialog(); //关联界面

    void openTooltipDialog(const QString &msg); //提示对话框

    void openHelpUrl();
    void openConvertUrl();
    void openSendEmail();
    void openFreeDownload();

    void openUrl(const QString &url);

    QString getFileName(const QString caption, const QString &path, const QString &filter);
    QStringList getFileNames(const QString caption, const QString &path, const QString &filter);
    QString getDir(const QString caption, const QString &path);

private:
    PlayType _curType;

    TcpServer *p_httpServer;

    QList<QDialog *>dialogs;

    bool b_systemDialog;
};

#endif // OPENFILE_H
