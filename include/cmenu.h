#ifndef CMENU_H
#define CMENU_H

#include <QObject>
#include "singleton.h"
#include "updateinterface.h"

class QMenu;
class QTimer;
class QAction;
class QActionGroup;

#define Menu CMenu::getInstance()

class CMenu : public QObject, public UpdateInterface, public Singleton<CMenu>
{
    Q_OBJECT
    friend class Singleton<CMenu>;
    friend class QSharedPointer<CMenu>;

public:
    bool audioMenuIsEnable();
    bool subtitleMenuIsEnable();
    bool chapterMenuIsEnable();

    void setAudioTrackId(const int&);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void updateStayOnTop(const int &index);
    void upQActionText(QMenu* menu);
    void selectSubtitle(QString subPath);       // 选择传入的字幕文件路径为当前使用字幕
    bool isLoadSubtitle(QString subPath);       // 当前字幕是否已经自动载入
    void loadSubpath2Subtitle(QString subPath); // 手动加入字幕文件

public slots:
    /*!
     * \brief clear: 播放结束时, 主要清理菜单的一些信息,恢复至默认状态,
     *  eg. 清理字幕轨道菜单
     */
    void resetTrackMenu();
    void resetMenu();      //菜单初始化
    void enableVideoMenu();//启用视频菜单
    void enableAudioMenu();//启用音频菜单

    void showSetMenu(const QPoint &pos);     //显示设置菜单
    void showRightMenu(const QPoint &pos);   //显示右键菜单
    void showAudioMenu(const QPoint &pos);   //显示音频轨道菜单
    void showChapterMenu(const QPoint &pos); //显示章节菜单
    void showSubtitleMenu(const QPoint &pos);//显示字幕菜单
    void showAirMenu(const QPoint &pos);     //显示air菜单

    void addRecentFile(const QString &file); //增加最近播放记录
    void clearRecentMenu();                  //清空最近播放的菜单
    void clearRecentFile();                  //清空最近记录
    void muteMenu(const bool &b);            //是否静音

    void initRecentFiles();                  //初始化播放记录

    void hideRegistMenu();//隐藏注册菜单

private slots:
    void showMenu();
    void hideMenu();

    void resetTitleMenu(const int &);

    void resetAirMenu();

Q_SIGNALS:
    void updateShowState();

private:
    Q_DISABLE_COPY(CMenu)
    explicit CMenu(QObject *parent = Q_NULLPTR);
    ~CMenu();

    //清空指定菜单
    void clearMenu(QMenu *);

    //初始化菜单组
    void initMenuGroup();

    //菜单初始化
    void initSubMenu();  //初始化 子 菜单
    void initSetMenu();  //初始化设置菜单
    void initRightMenu();//初始化右键菜单

    void initAudioMenu();   //初始化音轨
    void initStereoMenu();
    void initVideoMenu();   //初始化视频轨道
    void initTitleMenu();   //初始化Title
    void initChapterMenu(); //初始化章节
    void initSubtitleMenu();//初始化字幕
    void initAirPlayMenu(); //初始化airPlay

private:
    //弹出显示的主菜单
    QMenu *_setMenu;    //右上角的设置菜单
    QMenu *_rightMenu;  //主界面的 右键菜单
    QMenu *_videoMenu;  //视频轨道菜单
    QMenu *_audioMenu;  //音频轨道菜单
    QMenu *_subtitleMenu;//字幕菜单
    QMenu *_airMenu;    //air菜单

    //子菜单
    QMenu *_fileMenu;   //打开文件菜单
    QMenu *_recentMenu; //历史记录菜单
    QMenu *_helpMenu;   //帮助菜单
    QMenu *_langMenu;   //语言菜单
    QMenu *_rPlayMenu;  //播放控制菜单
    QMenu *_rStayMenu;  //层次菜单（保持的层次）
    QMenu *_rVideoMenu; //视频
    QMenu *_rAudioMenu; //音频
    QMenu *_rDeinterlaceMenu;//反交错
    QMenu *_rAspectRatio;    //宽高比
    QMenu *_rTransformMenu;  //旋转
    QMenu *_rEqualizerMenu;  //均衡器
    QMenu *_rStereoModeMenu;

    QMenu *_titleMenu;  //title菜单
    QMenu *_chapterMenu;//章节菜单

    //公用Action
    QAction *_setAc         = NULL;      //设置
    QAction *_syncAc        = NULL;     //
    QAction *_openFile_right= NULL;
    QAction *_openFile_set  = NULL;
    QAction *_convert       = NULL;
    QAction *_loadSub       = NULL;
    QAction *_tuner         = NULL;
    QAction *_fullAc        = NULL;     //全屏
    QAction *_snapshotAc    = NULL; //截屏
    QAction *_infoAc        = NULL;     //MediaInfo
    QAction *_clearRecent   = NULL;//清空最近打开的文件
    QAction *_hwaccelAc     = NULL;  //硬件加速
    QAction *_muteAc        = NULL;     //静音
    QAction *_addVolumeAc   = NULL;//增加音量
    QAction *_decVolumeAc   = NULL;//进校音量
    QAction *_alwaysAc;
    QAction *_whplayAc;
    QAction *_neversAc;
    QAction *_registAc      = NULL;//注册
    QAction *_relation      = NULL;  //文件关联

    QActionGroup *_audioMenuGroup;    //audio track
    QActionGroup *_videoMenuGroup;    //video track
    QActionGroup *_airMenuGroup;      //airPlayMenu
    QActionGroup *_titleMenuGroup;    //title
    QActionGroup *_chapterMenuGroup;  //章节
    QActionGroup *_subtitleMenuGroup; //字幕
    QActionGroup *_deinterlaceGroup;  //反交错
    QActionGroup *_aspectRatioGroup;  //宽高比
    QActionGroup *_equalizerMenuGroup;//均衡器
    QActionGroup *_stereoModeMenuGroup;
    QActionGroup *_stayGroup;         //stay
    QActionGroup *_languageGroup;     //语言

    bool b_isInitVideoMenu;
    bool b_isInitMusicMenu;
    QStringList     _recentList;
    QStringList     _folderSubList; // 自动添加目录下的字幕路径
    QStringList     _pathSubList;   // 手动加入字幕的路径

    int _showCount;

    QTimer *p_timer = NULL;
};

#endif // CMENU_H
