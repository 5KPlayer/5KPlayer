#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include <QWidget>
#include <QPoint>
#include "updateinterface.h"
#include "previewthead.h"
#include "snapshotthread.h"

class SvgButton;
class CSlider;
class QLabel;
class Preview;

class PlayControlWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit PlayControlWidget(QWidget *parent = 0);
    ~PlayControlWidget(){}

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void ignoreLeaveEvent();//忽略离开事件
    void handleVlc(const bool &b);        //是否处理vlc相关(是否处于远程连接状态)
    void visibleSnapshotBtn(const bool &);//是否隐藏截图按钮

    bool getMouseIn() const{return b_mouseIn;}

    int getCurTime() const;//获取当前的时间

    void changeAirplayState(const bool &b);//改变airplay样式
    void changViewPoint();
    void changMultiview(QString mult);
    void clearViewPoint();
    bool getViewPointState();
    void upViewPointState();
#ifdef Q_OS_MACX
    void playSliderEnable(bool isMusie);
#endif

public slots:
    void playClicked(const bool &play=true, const bool &b=true);//点击播放按钮

    void setCurTime_t(const int &);
    void setCurTime(const float &, const bool &b=true); //设置当前进度
    void showMuteBtn(const bool &b);//显示静音按钮
    void setCurVolume(const int &, const bool &b=true); //设置当前音量
    void upCurVolume(const int &v);      // 作为控制端只更新volume的值
    void setTotalTime(const int &); //总时间

    void showPlayImage(); //显示播放图片
    void showPauseImage();//显示暂停图片
    void clearInfo();     //清空信息
    void snapshot();      //截图
#ifdef Q_OS_WIN
    void takeScreenShot(QString fileName);  //使用Qt接口提取屏幕图像截屏
#endif

    void hidePreview();

    void updatePlayModeImage();

private slots:
    void changePlayMode();
    void openSnapshotDir();
    void showPreview(QString path, int64_t time, QByteArray data);
    void clickviewPoint();
    void clickRviewPoint();
    void clickEidtBtn();

protected:
    virtual void resizeEvent(QResizeEvent *);

    virtual void paintEvent(QPaintEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

private:
    void InitLanguge();
    bool checkMousePoint(QPoint pos); // 检查当前鼠标位置是否在_previewPos的位置，x y的误差可以为5

private:
    int  i_totalTime;
    bool b_mouseIn;
    bool b_handleVlc;


    SvgButton *_preBtn; //上一个
    SvgButton *_playBtn;//播放/暂停
    SvgButton *_nextBtn;//下一个
    SvgButton *_snapshotBtn;//截图
    SvgButton *_viewPointBtn;//360视频
    SvgButton *_editVideoBtn;//视频编辑
    //SvgButton *_airBtn; //airplay
    SvgButton *_playModeBtn;//播放模式按钮

    //音量相关
    SvgButton *_muteBtn;        //静音
    //SvgButton *_volumeMinBtn;   //最小音量
    CSlider   *_volumeBarSlider;//音量条
    SvgButton *_volumnMaxBtn;   //最大音量
    QLabel    *_curVolLabel;

    //进度相关
    QLabel  *_curTime;   //当前时间
    CSlider *_playSlider;//进度条
    QLabel  *_totalTime; //总时长

    Preview           *p_preview;
    int     _multiview; // 记录当前360视频源模式 0：2D，1：3D左右 2：3D上下，
    int     _viewPoint;

    int _space;//间距
    int _lrMargin;//左右2边间距
    int _tbMargin;//上下2边间距

    //
    PreviewThead    _previewThead;
    SnapshotThread  _snapThread;
    QPoint  _previewPos;  // Preview的鼠标位置
    int64_t _previewTime; // Preview的位置时间
    QString _previewFile; // Preview的文件名字
};

#endif // PLAYCONTROLWIDGET_H
