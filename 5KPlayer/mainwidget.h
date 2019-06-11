#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDialog>
#include "updateinterface.h"

class TransEventWidget;
class HeadWidget;
class OpenModeWidget;
class LibraryWidget;
class PlayControlWidget;
class SvgButton;
class VirtualLineWidget;
class CPauseWidget;
class PlayMusicWidget;
class OpenningInfoWidget;
class AirPlay;
class QTimer;

class MainWidget : public QDialog, public UpdateInterface
{
    Q_OBJECT
    friend class PlayerMainWidget;
public:
    explicit MainWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~MainWidget(){}

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void showControl();//显示播放控制界面
    void resetControl(const QPoint &pos);//重置控制界面

    bool isPlayVideo();//是否处于等比缩放
    bool isPlayMusic();//

    int headHeight() const;//Head高度
    void initMediaplayerSignals();//初始化MediaPlayer的信号槽链接

    bool airplayStarted();
    bool startAirPlay();
    void stopAirPlay();

    void releaseMouse();

protected:
    bool eventFilter(QObject *obj, QEvent *e);
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

private slots:
    void showMainWidget();   //显示5KPlayer页面
    void showLibraryWidget();//显示Library页面
    void showOpenModeWidget();//显示默认状态
    void changeVolumeSlider(int);//改变声音进度条
    void showMuteBtn(const bool &);//改变mute图标的显示与否
    void openWInitClick(QString item);// lib面板未初始化时点击music或者youtube按钮的事件

    //VLC状态处理
    void vlcOpening();
    void vlcPlaying();
    void vlcPaused();
    void vlcStoped();
    void vlcEnded();
    void vlcVout(const int &); //vout 视频流
    void stopVlc(const bool &async = true);//主动停止VLC
    void changeRatio();

    //Airplay相关的槽
    void airplayMirrorStart(int state);       //airplay状态
    void airplayRecordFinish(const QString &);//airplay录屏结束

    void airplayAudioStart();    //audio开始
    void airplayAudioPause();    //audio暂停
    void airplayAudioVolume(const int &); //audio音量
    void airplayAudioDestroy();  //audio结束
    void airplayAudioPlay(int start, int end);    //audio进度
    void airplayAudioImage(const QByteArray &);   //audio封面
    void airplayAudioMetadata(const QString &);//audio相关信息

    void airplayVideoStart(const QString &);//
    void airplayVideoSeek(const double &);
    void airplayVideoControl(const QString &);

    //播放器与播放器的连接
    void joinConnectMode();//连接模式
    void closeConnectMode();//关闭连接模式
    void handleUDPInfo(const QString &msg);

signals:
    void ItemInitClick(QString ItemName);

private:
    void init();//初始化界面

    int chooseBestAudio();//优先选择声轨

    //显示 openning状态界面
    void showOpenStateWidget();
    void hideOpenStateWidget();

    void showPlayControl();
    void hidePlayControl();

private:
    //标题栏
    HeadWidget *_head;

    //5Kplayer界面
    QWidget *_playerW;//播放界面
    CPauseWidget *_pauseW;//暂停时 显示的界面
    OpenModeWidget *_openW;//打开方式界面
    PlayControlWidget *_playControl;//播放控制
    SvgButton *_closePlay;//关闭正在播放

    //Library界面
    LibraryWidget *_libW;

    //VLC 处于openning状态阻塞
    OpenningInfoWidget *_openSateW;
    PlayMusicWidget    *p_musicW;

    int _margin;//边距
    int _spacing;//间距

    bool _playEnded;//当前播放是否结束

    VirtualLineWidget *_vrW;//虚线界面

    // 控制栏按比例显示位置
    bool _isPressed;  //鼠标是否按下
    QPoint _pressPos; //按下的坐标
    qreal _xRatio;
    qreal _yRatio;

    //大小重置后设置位置
    bool _isResetControl;
    QPoint _resetPos;

    bool _initVolume;//第一次启动  设置初始音量

    bool b_handleVlc;//是否处理vlc信号

    bool b_playingState;//暂停->播放 会出现bug

    int i_curPostion;//用于正在播放时，连接其他电脑

    AirPlay *p_airPlay;
    bool b_airplayMusic;//是否在播放airplay - Music
    bool b_airplayVideo;//是否在播放airplay - Video
    quint16 i_timer;//进度值
    QTimer *p_timer;//同步airplay-music进度
};

#endif // MAINWIDGET_H
