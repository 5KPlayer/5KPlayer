#ifndef PLAYERMAINWIDGET_H
#define PLAYERMAINWIDGET_H

#include <QWidget>
#include <QProcess>
#include "singleton.h"
#include "updateinterface.h"
#include "dyvideoframe.h"

#define REGION_COUNT 9

class QTimer;
class MainWidget;
class QScreen;
class QSystemTrayIcon;
class MainWidgetProxy;
class PlayControlWidget;
class QMenu;

#define PMW  PlayerMainWidget::getInstance()

class PlayerMainWidget : public UpdateInterfaceWidget, public DYVideoFrame, public Singleton<PlayerMainWidget>
{
    Q_OBJECT
    friend class MainWidget;
    friend class Singleton<PlayerMainWidget>;
    friend class QSharedPointer<PlayerMainWidget>;

public:
    //九宫格
    enum WidgetRegion
    {
        Top = 0,
        TopRight,
        Right,
        RightBottom,
        Bottom,
        LeftBottom,
        Left,
        LeftTop,
        Inner,
        Unknown
    };

    //更新语言
    void updateLanguage();

    //更新界面
    void updateUI(const qreal &scaleRatio);

    //显示主界面
    void showControl(bool b);

    WId request();  //获取播放窗口的 句柄
    void release(); //释放窗口

    void onTop() const; //置顶
    void offTop() const;//取消置顶

    //停止vlc播放
    void stopVlc(const bool &async);

    void showPlayControl();//显示外部控制条
    void hidePlayControl();//隐藏外部控制条

    void rotate();//界面宽高旋转

    bool showMax() const;//当前界面是否最大化 或者 全屏显示

    int headHeight() const;//head高度

    // 开启/关闭 连接模式
    void startConnectedMode();
    void closeConnectedMode();
    void playUrl(const QString &);

    QSize minimumSize() const;
    int  minimumWidth() const;
    int  minimumHeight() const;

    void initMediaplayer(const QString &file); //初始化mediaPlayer

    void languageChange(const QString &language);

    //播放上一首 下一首  jingyin
    void playPre();
    void playNext();
    void toggleMute();
    void togglePause(const bool &b);

    void moveWidget(const QPoint &p);
    void moveWidget(const int &x, const int &y);
    void setWidgetSize(const int &w, const int &h);
    void setWidgetGeometry(const QRect &rect);
    void setWidgetGeometry(const int &x, const int &y,
                           const int &w, const int &h);
#ifdef Q_OS_MACX
    void initSysMenu();
#endif
    //airplay接口
    bool airplayStarted();
    bool startAirplay();
    void closeAirplay();

    //Dialog移动时  跟随移动
    void dialogPressedMove(const QPoint &point);
    void dialogMoved(const QPoint &point);
    void dialogReleaseMove();

    void changePlayModeImage();

    void releaseMouse();

    QWidget *mainWidget() const {return (QWidget *)p_mainWidget;}

    bool isInit() const {return b_init;}

    void activateWidget();

    QRect originalGeometry() const {return m_normalRect;}

    QScreen *curScreen() const;

public slots:
    //重新计算所在的屏幕
    void recount(const QPoint &point);
    void screenChanged(QObject *obj);

    void showCenter(bool b = true);//居中显示
    void showNormalCenter();
    void showCurrentCenter(const int &w, const int &h, qreal ratio = 0);

    void stayAlwaysOnTop();
    void stayNeverOnTop();
    void stayPlayingOnTop();

    void setPopDialog(const bool &b = true);
    void setTimerState(const bool &isStart = true);
    void showMainWidget();//开启定时器
    void hideMainWidget();//定时隐藏

    void ytbUpgradeTimer();     // ytb更新的定时器
    void mainUpgradeTimer();    // 5kPlayer更新的定时器
    //显示正常尺寸（防止全屏下不切换大小，正常显示后为以前的尺寸，比例不适合）
    void showNormaled();
    void showFullScreened();
    //显示出来   隐藏 最下面  不活动
    void showOut();

    //根据配置文件关闭窗口
    void closeWithConfig(bool checkConfig = false);

protected:
    virtual bool eventFilter(QObject *, QEvent *);
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
    Q_DISABLE_COPY(PlayerMainWidget)
    PlayerMainWidget(QWidget *parent = 0);
    ~PlayerMainWidget(){}

    void sizeChanged(const int &w, const int &h);

    //设置边缘宽度
    void setBorderWidth();

    //根据鼠标判断当前所在区域
    WidgetRegion hitTest(const QPoint &pos);

    //更新界面
    void resizeGeometry(const QPoint &newPos, const int &directX, const int &directY);

    // 鼠标从边框快速移到窗体内子控件上，可能会造成鼠标样式未改变，这里使用计时器监控
    void startCursorTimer();
    void stopCursorTimer();

    //根据区域设置鼠标样式
    void setCursorSharpe(const PlayerMainWidget::WidgetRegion &regin);

    //计算最佳大小
    void bestSize(int &w, int &h, const qreal &ratio);
    //计算初始化最佳大小
    void bestInitSize(int &w, int &h, const qreal &ratio);

    //创建系统托盘
    void createTrayIcon();

    //界面更新
    void changeWidgets();

    //关闭
    void startClose();

    //检测坐标  重新设置 第三个参数：全屏时 是否重置大小
    void checkResetGeometry(int w, int h, const bool &b_full_reset);

    //检测界面是否在当前屏幕内
    void checkGeometry();

    bool init();

    void autoAssociation();//自动关联

    void autoCheckUpMain(); // 自动检查更新主程序

    void normalToFull();

private:
    MainWidget *p_mainWidget;
    QWidget *p_videoWidget; //播放界面
    PlayControlWidget *p_controlWidget;//外部控制栏


    qreal _ratio;//屏幕间的缩放比例

    // 鼠标是否按下
    bool b_mousePressed;

    // 记录鼠标按下时所点击的区域
    WidgetRegion _regionPressed;

    // 九宫格，对应9个区域
    QRect _regions[REGION_COUNT];

    // 记录按下的坐标
    QPoint m_pressedPoint;
    QPoint m_originPosGlobal;

    // 拖拽前窗体位置和大小
    QRect m_originRect;

    //定时器ID
    int i_cursorTimerId;

    //外部控制条
    bool b_isControlPressed;//是否按下
    double dx_posRatio;
    double dy_posRatio;

    //是否启动定时器
    QTimer *p_hideTimer;
    bool b_startTimer;
    bool b_popDialog;

    //ytb自动更新的定时器
    QTimer *p_ytbupTimer;

    //5kplayer自动更新检查定时器
    QTimer *p_mainUpTimer;
    //系统托盘
    QSystemTrayIcon *p_trayIcon;
    QMenu*           _trayMenu = NULL;

    //关闭
    bool b_close;
    bool b_isStartUpMain;

    //ADD: 如果全屏状态下不变化大小,记住大小
    QRect m_normalRect;

    bool b_init;
};

#endif // PLAYERMAINWIDGET_H
