#ifndef EDITRIGHTWIDGET_H
#define EDITRIGHTWIDGET_H

#include <QWidget>
#include "updateinterface.h"
#include "dystructu.h"

class SvgButton;
class DYLabel;
class QLabel;
class QLineEdit;
class DYCheckBox;
class SpinBox;
class DoubleSpinBox;
class CSlider;
class CSlider3;
class DYPixLabel;
class QScrollBar;
class TransEventScrollArea;
class TransEventWidget;
class QPropertyAnimation;
class SaveConfirmWidget;

class EditRightWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit EditRightWidget(QWidget *parent = 0);
    ~EditRightWidget();
    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);
    CutParams getCutParams();   //获取剪切参数

private:
    void startAminWidget(int state);
    void upUI();
    double Vlc2ffmpegBrightness(double value); // VLC2ffmpeg亮度
signals:
    void closeEditWindow();
    void upWidgetGeometry();
private slots:
    void sliderValueChange(double value);

    void speedSpinChange(double value);
    void volumeEditChange(QString value);
    void rotateLeftBtnClick();
    void rotateRightBtnClick();
    void hvflipBtnClick();
    void rotateInitBtnClick();

    void audioDelayChange(int value);
    void hueBWChkClick(bool check);
    void hurInitBtnClick();

    void saveBtnClick();
    void showPreBtnClick();
    void showPrePosInfo(float pos);
    void cancelBtnClick();
public slots:
    void selectPtsChange(double start,double end);
    void stopShowPre();     // 在做解码操作时，停止预览
protected:
    void resizeEvent(QResizeEvent *event);
private:
    double _scaleRatio;    //缩放比例
    SvgButton *_closeButton;    //关闭按钮
    SvgButton *_zoomButton;     //最大化按钮

    QWidget*                _pHeadWidget;   // head界面
    QLabel*                 _pBottonLab;    // 下方的一个拉伸块
    QLabel*                 _pRightLab;    // 右方的一个拉伸块

    TransEventScrollArea *  _area;          // 滚动区域
    TransEventWidget *      _scrollW;       // Set滚动主界面
    QWidget*                _pSetWidget;    // set界面
    SaveConfirmWidget*      _pSaveWidget;   // save界面

    QLabel*     _rotateText;
    QLineEdit*  _rotateValue;

    DYLabel*    _leftRotateBtn;
    DYLabel*    _rightRotateBtn;

    DYLabel*    _horFilpBtn;
    DYLabel*    _verFilpBtn;

    DYPixLabel* _resetRotate;
    QLabel*     _rotateLine;

    DoubleSpinBox* _playSpeedValue;
    QLabel*     _playSpeedLab;
    CSlider3*   _playSpeedSlider;
    QLabel*     _speedLab;
    QLabel*     _playVolumeLab;
    CSlider3*   _playvolumeSlider;//音量条
    QLineEdit*  _playVolumeEdit;
    QLabel*     _playVolumeVal;
    QLabel*     _volumeDelayLab;
    SpinBox*    _volumeDelayValue;
    QLabel*     _delayUnitLab;

    QLabel*     _hueLab;
    DYCheckBox* _hueBW;
    QLabel*     _saturationLab;
    CSlider3*   _saturationSlider;
    QLabel*     _contrastLab;
    CSlider3*   _contrastSlider;
    QLabel*     _brightnessLab;
    CSlider3*   _brightnessSlider;
    DYPixLabel* _resetHue;
    QLabel*     _playLine;

    DYLabel*    _saveBtn;
    DYLabel*    _showPreBtn;

    int64_t     _startTime;
    int64_t     _endTime;
    QTimer*     _posTimer;
    bool        _showPlay;  // 预览标记

    int     i_curState;
};

#endif // EDITRIGHTWIDGET_H
