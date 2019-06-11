#include "editrightwidget.h"
#include "playermainwidget.h"
#include <QDebug>
#include "svgbutton.h"
#include <QProcess>
#include <QApplication>
#include <QScrollBar>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include "dylabel.h"
#include "dypixlabel.h"
#include "dycheckbox.h"
#include "spinbox.h"
#include "cslider.h"
#include "cslider3.h"
#include "globalarg.h"
#include "openfile.h"
#include "librarydata.h"
#include "systemfunc.h"
#include "transeventscrollarea.h"
#include "transeventwidget.h"
#include "saveconfirmwidget.h"
#include "doublespinbox.h"
#include "dymediaplayer.h"

EditRightWidget::EditRightWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    this->setStyleSheet("background-color: rgb(22, 22, 26)");
    this->setMouseTracking(true);

    _scaleRatio     = Global->_screenRatio;
    _startTime      = 0;
    _endTime        = 0;
    i_curState      = 0;
    _showPlay       = false;

    _pHeadWidget    = new QWidget(this);
    _pBottonLab     = new QLabel(this);
    _pRightLab      = new QLabel(this);
    _pHeadWidget->setMouseTracking(true);
    _pBottonLab->setMouseTracking(true);
    _pRightLab->setMouseTracking(true);

    _area    = new TransEventScrollArea(this);
    _area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _area->setFrameShape(QFrame::NoFrame);

    _scrollW     = new TransEventWidget(_area);
    _area->setWidget(_scrollW);

    _scrollW->setStyleSheet("background:rgb(22, 22, 26)");
    _pSetWidget  = new QWidget(_scrollW);
    _pSaveWidget = new SaveConfirmWidget(_scrollW);

    _rotateText     = new QLabel(_pSetWidget);
    _rotateValue    = new QLineEdit(_pSetWidget);

    _leftRotateBtn     = new DYLabel(_pSetWidget,true);
    _rightRotateBtn    = new DYLabel(_pSetWidget,true);
    _leftRotateBtn->setText(Lang("EDIT/lrotate"));
    _rightRotateBtn->setText(Lang("EDIT/rrotate"));
    _leftRotateBtn->setAlignment(Qt::AlignCenter);
    _rightRotateBtn->setAlignment(Qt::AlignCenter);

    _horFilpBtn = new DYLabel(_pSetWidget,true);
    _horFilpBtn->setActPressSta(true);
    _horFilpBtn->setText(Lang("EDIT/hflip"));
    _horFilpBtn->setAlignment(Qt::AlignCenter);

    _verFilpBtn = new DYLabel(_pSetWidget,true);
    _verFilpBtn->setActPressSta(true);
    _verFilpBtn->setText(Lang("EDIT/vflip"));
    _verFilpBtn->setAlignment(Qt::AlignCenter);

    QString textColor = "color:rgb(141,141,144)";

    _rotateText->setStyleSheet(textColor);
    _rotateText->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _rotateText->setText(Lang("EDIT/rotate"));

    _rotateValue->setValidator(new QIntValidator(0, 359, this));
    _rotateValue->setContextMenuPolicy(Qt::NoContextMenu);
    _rotateValue->setAlignment(Qt::AlignCenter);
    _rotateValue->setEnabled(false);
    _rotateValue->setText("0");


    _resetRotate    = new DYPixLabel(":/res/png/fanhui",Lang("EDIT/recinit"),_pSetWidget);
    _resetRotate->setFontUnderline(true);
    _resetRotate->setMouseHead(true);

    QString rotate = Lang("EDIT/recinit");
    int width = _resetRotate->fontMetrics().width(rotate) /  _scaleRatio;
    _resetRotate->initSize(width+28,20);

    _rotateLine     = new QLabel(_pSetWidget);
    _rotateLine->setStyleSheet("background-color: rgb(53, 53, 53)");


    _playSpeedLab   = new QLabel(_pSetWidget);
    _speedLab       = new QLabel(_pSetWidget);
    _playVolumeLab  = new QLabel(_pSetWidget);
    _playVolumeVal  = new QLabel(_pSetWidget);
    _volumeDelayLab = new QLabel(_pSetWidget);
    _delayUnitLab   = new QLabel(_pSetWidget);
    _volumeDelayValue   = new SpinBox(_pSetWidget);
    _playSpeedValue     = new DoubleSpinBox(_pSetWidget);
    _playvolumeSlider   = new CSlider3(_pSetWidget);
    _playVolumeEdit     = new QLineEdit(_pSetWidget);

    _playSpeedSlider   = new CSlider3(_pSetWidget);
    _playSpeedSlider->initSize(130, 32);
    _playSpeedSlider->setRange(0.25, 2.0);
    _playSpeedSlider->setPageStep(0.05);
    _playSpeedSlider->setValue(1.0);
    _playSpeedSlider->setLineOrText(true);

    _playvolumeSlider->initSize(130, 16);//100 4
    _playvolumeSlider->setPageStep(1);
    _playvolumeSlider->setRange(0,100);
    _playvolumeSlider->setValue(100);

    _playVolumeEdit->setValidator(new QIntValidator(0, 100, _pSetWidget));
    _playVolumeEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _playVolumeEdit->setAlignment(Qt::AlignCenter);
    _playVolumeEdit->setText("100");

    _volumeDelayValue->setMinimum(0);
    _volumeDelayValue->setMaximum(9900);
    _volumeDelayValue->setSingleStep(100);
    _volumeDelayValue->setAlignment(Qt::AlignCenter);
    _volumeDelayValue->setStyleSheet("background-color: rgb(243,243,243);"
                                     "color:rgb(46,46,50)");


    _playSpeedLab->setStyleSheet(textColor);
    _playSpeedLab->setText(Lang("EDIT/playspeed"));
    _playSpeedLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    _playSpeedValue->setRange(0.25,2.0);
    _playSpeedValue->setSingleStep(0.05);
    _playSpeedValue->setValue(1);
    _playSpeedValue->setAlignment(Qt::AlignCenter);
    _playSpeedValue->setStyleSheet("background-color: rgb(243,243,243);"
                                     "color:rgb(46,46,50)");

    _speedLab->setStyleSheet(textColor);
    _speedLab->setText("X");
    _speedLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    _playVolumeLab->setStyleSheet(textColor);
    _playVolumeLab->setText(Lang("EDIT/volume"));
    _playVolumeLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    _playVolumeVal->setStyleSheet(textColor);
    _playVolumeVal->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _playVolumeVal->setText("%");

    _volumeDelayLab->setStyleSheet(textColor);
    _volumeDelayLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _volumeDelayLab->setText(Lang("EDIT/voiceDey"));

    _delayUnitLab->setStyleSheet(textColor);
    _delayUnitLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _delayUnitLab->setText(Lang("EDIT/unit"));


    _hueLab         = new QLabel(_pSetWidget);
    _hueBW          = new DYCheckBox(_pSetWidget,Lang("EDIT/blackorwhite"));
    _saturationLab  = new QLabel(_pSetWidget);
    _contrastLab    = new QLabel(_pSetWidget);
    _brightnessLab  = new QLabel(_pSetWidget);
    _resetHue       = new DYPixLabel(":/res/png/fanhui",Lang("EDIT/recinit"),_pSetWidget);
    _resetHue->setFontUnderline(true);
    _resetHue->setMouseHead(true);
    _resetHue->initSize(width+28,20);

    _saturationSlider   = new CSlider3(_pSetWidget);
    _saturationSlider->initSize(130, 32);
    _saturationSlider->setRange(0, 3);
    _saturationSlider->setPageStep(0.05);
    _saturationSlider->setValue(1.0);
    _saturationSlider->setLineOrText(true);

    _contrastSlider     = new CSlider3(_pSetWidget);
    _contrastSlider->initSize(130, 32);
    _contrastSlider->setRange(0, 2);
    _contrastSlider->setPageStep(0.05);
    _contrastSlider->setValue(1.0);
    _contrastSlider->setLineOrText(true);

    _brightnessSlider   = new CSlider3(_pSetWidget);
    _brightnessSlider->initSize(130, 32);
    _brightnessSlider->setRange(0, 2.0);
    _brightnessSlider->setPageStep(0.05);
    _brightnessSlider->setValue(1);
    _brightnessSlider->setLineOrText(true);

    _hueBW->setTextColor("#8d8d9a");
    _hueBW->setTextFontBlod(true);

    _hueLab->setStyleSheet(textColor);
    _hueLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _hueLab->setText(Lang("EDIT/color"));
    _saturationLab->setStyleSheet(textColor);
    _saturationLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _saturationLab->setText(Lang("Videotuner/Saturation"));
    _contrastLab->setStyleSheet(textColor);
    _contrastLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _contrastLab->setText(Lang("Videotuner/contrast"));
    _brightnessLab->setStyleSheet(textColor);
    _brightnessLab->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    _brightnessLab->setText(Lang("Videotuner/Brightness"));

    _playLine   = new QLabel(_pSetWidget);
    _playLine->setStyleSheet("background-color: rgb(52, 52, 55)");

    _saveBtn        = new DYLabel(_pSetWidget,true);
    _saveBtn->setBackClolr("rgb(0,140,220)","rgb(51,191,242)","rgb(0,175,239)");
    _saveBtn->setText(Lang("EDIT/save"));
    _saveBtn->setAlignment(Qt::AlignCenter);

    _showPreBtn     = new DYLabel(_pSetWidget,true);
    _showPreBtn->setText(Lang("EDIT/showpre"));
    _showPreBtn->setAlignment(Qt::AlignCenter);

    //关闭按钮
    _closeButton = new SvgButton(_pHeadWidget);
    _closeButton->initSize(26,22);
    _closeButton->setImagePrefixName("back");
#ifdef Q_OS_WIN
    _zoomButton   = new SvgButton(_pHeadWidget);
    _zoomButton->initSize(26,22);
    _zoomButton->setImagePrefixName("zoom");
#endif

    connect(_playSpeedSlider,SIGNAL(valueChanged(double)),SLOT(sliderValueChange(double)));
    connect(_playvolumeSlider,SIGNAL(valueChanged(double)),SLOT(sliderValueChange(double)));
    connect(_saturationSlider,SIGNAL(valueChanged(double)),SLOT(sliderValueChange(double)));
    connect(_contrastSlider,SIGNAL(valueChanged(double)),SLOT(sliderValueChange(double)));
    connect(_brightnessSlider,SIGNAL(valueChanged(double)),SLOT(sliderValueChange(double)));

    connect(_playSpeedValue,SIGNAL(valueChanged(double)),SLOT(speedSpinChange(double)));
    connect(_playVolumeEdit,SIGNAL(textChanged(QString)),SLOT(volumeEditChange(QString)));

    connect(_leftRotateBtn,SIGNAL(clicked()),SLOT(rotateLeftBtnClick()));
    connect(_rightRotateBtn,SIGNAL(clicked()),SLOT(rotateRightBtnClick()));
    connect(_horFilpBtn,SIGNAL(clicked()),SLOT(hvflipBtnClick()));
    connect(_verFilpBtn,SIGNAL(clicked()),SLOT(hvflipBtnClick()));
    connect(_resetRotate,SIGNAL(clicked()),SLOT(rotateInitBtnClick()));

    connect(_volumeDelayValue,SIGNAL(valueChanged(int)),SLOT(audioDelayChange(int)));
    connect(_hueBW,SIGNAL(checkChange(bool)),SLOT(hueBWChkClick(bool)));
    connect(_resetHue,SIGNAL(clicked()),SLOT(hurInitBtnClick()));
    connect(_saveBtn,SIGNAL(clicked()),SLOT(saveBtnClick()));
    connect(_showPreBtn,SIGNAL(clicked()),SLOT(showPreBtnClick()));
    connect(_pSaveWidget,SIGNAL(cancled()),SLOT(cancelBtnClick()));
    connect(MediaPlayer, SIGNAL(positionChanged(float)), SLOT(showPrePosInfo(float)));

    //点击关闭按钮,发送窗口关闭信号给主窗口
    connect(_closeButton,&SvgButton::clicked,[&]{
        if(PMW->isFullScreen() || PMW->isMaximized())
            PMW->showNormaled();
        emit closeEditWindow();
    });
#ifdef Q_OS_WIN
    connect(_zoomButton,&SvgButton::clicked,[&]{
        if(PMW->showMax())
            PMW->showNormaled();
        else
            PMW->showFullScreened();
        emit upWidgetGeometry();
    });
#endif
    _posTimer = new QTimer(this);
    _posTimer->setInterval(100);
    _posTimer->setSingleShot(true);
    connect(_posTimer,&QTimer::timeout,[=]{
        //MediaPlayer->audio()->toggleVolunMute();
        MediaPlayer->togglePause();
    });
    updateUI(_scaleRatio);
}

EditRightWidget::~EditRightWidget()
{

}

void EditRightWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _pSaveWidget->updateUI(scaleRatio);
    _closeButton->updateUI(scaleRatio);
#ifdef Q_OS_WIN
    _zoomButton->updateUI(scaleRatio);
#endif
    _resetRotate->updateUI(scaleRatio);
    _playSpeedSlider->updateUI(scaleRatio);
    _playvolumeSlider->updateUI(scaleRatio);
    _hueBW->updateUI(scaleRatio);
    _saturationSlider->updateUI(scaleRatio);
    _contrastSlider->updateUI(scaleRatio);
    _brightnessSlider->updateUI(scaleRatio);
    _resetHue->updateUI(scaleRatio);
    _rotateValue->setStyleSheet(QString("border-style:outset;border-width: 0px; border-radius: %1px;"
                                "background-color: rgb(243,243,243);color:rgb(141,141,144)").arg(4*_scaleRatio));

    _playVolumeEdit->setStyleSheet(QString("border-style:outset;border-width: 0px; border-radius: %1px;"
                                           "background-color: rgb(243,243,243);color:rgb(141,141,144)").arg(4*_scaleRatio));
    _area->verticalScrollBar()->setStyleSheet(QString(Global->scrollBarSty)
                                               .arg(4  * _scaleRatio).arg(20 * _scaleRatio));

    upUI();
}

CutParams EditRightWidget::getCutParams()
{
    CutParams param;
    param.filePath      = LibData->getcurmediaPath();
    param.startTime     = _startTime;
    param.endTime       = _endTime;

    param.adelay        = _volumeDelayValue->value();
    param.volume        = _playvolumeSlider->getValue() / 100;
    param.speed         = _playSpeedSlider->getValue();
    param.rotate        = _rotateValue->text().toInt();
    param.bitRate       = LibData->getCurVideoStream().bitRate;
    param.resolution    = LibData->getCurVideoStream().resolution;
    int flip = 0;
    if(_horFilpBtn->getPressSta()) {
        flip |= 1;
    }
    if(_verFilpBtn->getPressSta()) {
        flip |=2;
    }
    param.flip          = flip;
    param.blackWhite    = _hueBW->isChecked();
    // 若指定了黑白，则饱和度参数为0
    if(param.blackWhite) {
        param.saturation = 0;
        param.brightness = 0;
        param.contrast   = 1;
    } else {
        param.saturation = _saturationSlider->getValue();
        param.brightness = Vlc2ffmpegBrightness(_brightnessSlider->getValue());
        param.contrast   = _contrastSlider->getValue();
    }
    return param;
}

void EditRightWidget::startAminWidget(int state)
{
    int endValue;
    i_curState = state;
    if(state == 1) {
        endValue = _pSetWidget->width();
    } else if(state == 0) {
        endValue = 0;
    }

    //动画
    QPropertyAnimation * startAnim = new QPropertyAnimation(this);
    startAnim->setTargetObject(_area->horizontalScrollBar());
    startAnim->setPropertyName("value");
    startAnim->setDuration(300);
    startAnim->setEndValue(endValue);
    startAnim->setEasingCurve(QEasingCurve::OutCubic);
    startAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void EditRightWidget::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(13*_scaleRatio);
    _rotateText->setFont(font);
    _rotateValue->setFont(font);
    _leftRotateBtn->setFont(font);
    _rightRotateBtn->setFont(font);
    _horFilpBtn->setFont(font);
    _verFilpBtn->setFont(font);
    _playSpeedLab->setFont(font);
    _playSpeedValue->setFont(font);
    _speedLab->setFont(font);
    _playVolumeLab->setFont(font);
    _playVolumeVal->setFont(font);
    _playVolumeEdit->setFont(font);
    _volumeDelayLab->setFont(font);
    _volumeDelayValue->setFont(font);
    _delayUnitLab->setFont(font);
    _hueLab->setFont(font);
    _saturationLab->setFont(font);
    _contrastLab->setFont(font);
    _brightnessLab->setFont(font);

    font.setPixelSize(18*_scaleRatio);
    _saveBtn->setFont(font);
    _showPreBtn->setFont(font);

    _pHeadWidget->setGeometry(0,0*_scaleRatio,this->width()-4*_scaleRatio,42*_scaleRatio);
    _area->setGeometry(0,42*_scaleRatio,
                       this->width()-4*_scaleRatio,this->height() - 46*_scaleRatio);

    int scroolH = 634*_scaleRatio;
    if((this->height()-46*_scaleRatio) >= scroolH) {
        scroolH = this->height()-46*_scaleRatio;
    }
    _scrollW->setGeometry(0,0,_area->width()*2,scroolH);
    _area->horizontalScrollBar()->setValue(0);
    _pSetWidget->setGeometry(0,0,_area->width(),scroolH);
    _pSaveWidget->setGeometry(_area->width(),0,_area->width(),scroolH);

    if(i_curState == 1) {
        _area->horizontalScrollBar()->setValue(_area->width());
    }
    _pBottonLab->setGeometry(0,this->height() - 4*_scaleRatio,
                             this->width(),4*_scaleRatio);
    _pRightLab->setGeometry(this->width()-4*_scaleRatio,0,4*_scaleRatio,this->height());
    const int labH      = 20 * _scaleRatio;
    const int leftSpace = 42 * _scaleRatio;
    _rotateText->setGeometry(46*_scaleRatio,10*_scaleRatio,88*_scaleRatio,labH);
    _rotateValue->setGeometry(145*_scaleRatio,10*_scaleRatio,48*_scaleRatio,labH);

    _leftRotateBtn->setGeometry(46*_scaleRatio,40*_scaleRatio,88*_scaleRatio,labH);
    _rightRotateBtn->setGeometry(145*_scaleRatio,40*_scaleRatio,88*_scaleRatio,labH);

    _horFilpBtn->setGeometry(46*_scaleRatio,68*_scaleRatio,88*_scaleRatio,labH);
    _verFilpBtn->setGeometry(145*_scaleRatio,68*_scaleRatio,88*_scaleRatio,labH);

    _resetRotate->move((280*_scaleRatio - _resetRotate->width())/2,100*_scaleRatio);

    _rotateLine->setGeometry(0,148*_scaleRatio,this->width(),1*_scaleRatio);

    _playSpeedLab->setGeometry(leftSpace,170*_scaleRatio,100*_scaleRatio,labH);
    _playSpeedSlider->move(leftSpace,190*_scaleRatio);
    _playSpeedValue->setGeometry(180*_scaleRatio,196*_scaleRatio,52*_scaleRatio,labH);
    _speedLab->setGeometry(236*_scaleRatio,196*_scaleRatio,40*_scaleRatio,labH);

    _playVolumeLab->setGeometry(leftSpace,230*_scaleRatio,80*_scaleRatio,labH);
    _playvolumeSlider->move(leftSpace,250*_scaleRatio);
    _playVolumeEdit->setGeometry(180*_scaleRatio,248*_scaleRatio,52*_scaleRatio,labH);
    _playVolumeVal->setGeometry(236*_scaleRatio,248*_scaleRatio,labH,labH);

    _volumeDelayLab->setGeometry(leftSpace,290*_scaleRatio,160*_scaleRatio,labH);
    _volumeDelayValue->setGeometry(180*_scaleRatio,290*_scaleRatio,52*_scaleRatio,labH);
    _delayUnitLab->setGeometry(236*_scaleRatio,290*_scaleRatio,40*_scaleRatio,labH);


    _hueLab->setGeometry(leftSpace,330*_scaleRatio,80*_scaleRatio,labH);
    _hueBW->setGeometry(124*_scaleRatio,330*_scaleRatio,140*_scaleRatio,labH);

    _saturationLab->setGeometry(leftSpace,370*_scaleRatio,80*_scaleRatio,32*_scaleRatio);
    _saturationSlider->move(124*_scaleRatio,370*_scaleRatio);
    _contrastLab->setGeometry(leftSpace,410*_scaleRatio,80*_scaleRatio,32*_scaleRatio);
    _contrastSlider->move(124*_scaleRatio,410*_scaleRatio);
    _brightnessLab->setGeometry(leftSpace,450*_scaleRatio,80*_scaleRatio,32*_scaleRatio);
    _brightnessSlider->move(124*_scaleRatio,450*_scaleRatio);

    _resetHue->move((280*_scaleRatio - _resetHue->width())/2,500*_scaleRatio);
    _playLine->setGeometry(0,550*_scaleRatio,this->width(),1*_scaleRatio);

    int saveY = _playLine->y() + (634*_scaleRatio - 550*_scaleRatio -  36*_scaleRatio)/2;
    if(this->height() > 680*_scaleRatio) {
        saveY = _playLine->y() + (this->height() - _playLine->y() - (46 + 36)*_scaleRatio)/2;
    }
    _saveBtn->setGeometry(150*_scaleRatio,saveY,120*_scaleRatio,32*_scaleRatio);
    _showPreBtn->setGeometry(10*_scaleRatio,saveY,120*_scaleRatio,32*_scaleRatio);

    //关闭按钮右上角---始终处在最顶层
    _closeButton->move(250*_scaleRatio ,0);
#ifdef Q_OS_WIN
    _zoomButton->move(224*_scaleRatio,0);
#endif

    _pHeadWidget->raise();
}

double EditRightWidget::Vlc2ffmpegBrightness(double value)
{
    // vlc 范围0-2 ffmpeg -1 - 1
    double ffmpegBrg = value - 1.0;
    return ffmpegBrg;
}

void EditRightWidget::sliderValueChange(double value)
{
    CSlider3 * slider = qobject_cast<CSlider3 *>(this->sender());
    if(slider == _playSpeedSlider) {
        _playSpeedValue->setValue(value);
        MediaPlayer->setSpeed(value);
    } else if(slider == _playvolumeSlider) {
        _playVolumeEdit->setText(QString::number(value));
        MediaPlayer->audio()->setVolums(value);
    } else if(slider == _brightnessSlider) {
        MediaPlayer->video()->setBrightness(value);
    } else if(slider == _contrastSlider) {
        MediaPlayer->video()->setContrast(value);
    } else if(slider == _saturationSlider) {
        MediaPlayer->video()->setSaturation(value);
    }
}

void EditRightWidget::speedSpinChange(double value)
{
    _playSpeedSlider->setValue(value);
}

void EditRightWidget::volumeEditChange(QString value)
{
    _playvolumeSlider->setValue(value.toInt());
}

void EditRightWidget::rotateLeftBtnClick()
{
    int curRotate = _rotateValue->text().toInt();
    if(curRotate - 90 >= 0) {
        curRotate -= 90;
    } else {
        curRotate = curRotate + 270;
    }
    MediaPlayer->rotate()->editRotate(curRotate);
    _rotateValue->setText(QString::number(curRotate));
}

void EditRightWidget::rotateRightBtnClick()
{
    int curRotate = _rotateValue->text().toInt();
    if(curRotate + 90 < 360) {
        curRotate += 90;
    } else {
        curRotate = curRotate - 270;
    }
    MediaPlayer->rotate()->editRotate(curRotate);
    _rotateValue->setText(QString::number(curRotate));
}

void EditRightWidget::hvflipBtnClick()
{
    int flip = 0;
    if(_horFilpBtn->getPressSta()) {
        flip |= 1;
    }
    if(_verFilpBtn->getPressSta()) {
        flip |=2;
    }
    MediaPlayer->rotate()->editFlip(flip);
}

void EditRightWidget::rotateInitBtnClick()
{
    _rotateValue->setText(QString::number(0));
    _horFilpBtn->setPressSta(false);
    _verFilpBtn->setPressSta(false);
    MediaPlayer->rotate()->editInitTrans();
}

void EditRightWidget::audioDelayChange(int value)
{
    // vlc的声音延迟时间需要增加1000倍
    int vlcTime = value * 1000;
    MediaPlayer->audio()->setDelay(vlcTime);
}

void EditRightWidget::hueBWChkClick(bool check)
{
    _saturationSlider->setSliderEnable(check);
    _contrastSlider->setSliderEnable(check);
    _brightnessSlider->setSliderEnable(check);
    if(check) {
        MediaPlayer->video()->setSaturation(0);
        MediaPlayer->video()->setContrast(1.0);
        MediaPlayer->video()->setBrightness(1.0);
    } else {
        MediaPlayer->video()->setContrast(_contrastSlider->getValue());
        MediaPlayer->video()->setBrightness(_brightnessSlider->getValue());
        MediaPlayer->video()->setSaturation(_saturationSlider->getValue());
    }
}

void EditRightWidget::hurInitBtnClick()
{
    _playSpeedSlider->setValue(1);
    _playSpeedValue->setValue(1);
    _playvolumeSlider->setValue(100);
    _playVolumeEdit->setText("100");
    _volumeDelayValue->setValue(0);
    _hueBW->setChecked(false);
    _saturationSlider->setValue(1);
    _contrastSlider->setValue(1);
    _brightnessSlider->setValue(1);

    MediaPlayer->setSpeed(1);
    MediaPlayer->audio()->setDelay(0);
    MediaPlayer->video()->setContrast(1);
    MediaPlayer->video()->setBrightness(1);
    MediaPlayer->video()->setSaturation(1);
}

void EditRightWidget::saveBtnClick()
{
    stopShowPre();
    _pSaveWidget->setSaveInfo(getCutParams());
    startAminWidget(1);
}

void EditRightWidget::showPreBtnClick()
{
    _showPlay = !_showPlay;
    // 开始预览，检查静音改为有声音，停止预览，打开静音
    if(_showPlay) {
        MediaPlayer->setPosition(_startTime);
        if(MediaPlayer->audio()->isMute()) {
            MediaPlayer->audio()->toggleVolunMute();
        }
        _showPreBtn->setText(Lang("EDIT/stoppre"));
    } else {
        if(!MediaPlayer->audio()->isMute()) {
            MediaPlayer->audio()->toggleVolunMute();
        }
        _showPreBtn->setText(Lang("EDIT/showpre"));
    }

    MediaPlayer->togglePause();
}

void EditRightWidget::showPrePosInfo(float pos)
{
    int64_t curTime = MediaPlayer->length() * pos;
    if(_endTime < curTime) {
        stopShowPre();
    }
}

void EditRightWidget::cancelBtnClick()
{
    startAminWidget(0);
}

void EditRightWidget::selectPtsChange(double start, double end)
{
    if(_startTime == start && _endTime == end) {
        return;
    }
    // 非初始化
    if(_startTime  != 0 || _endTime != 0) {
        if(MediaPlayer->state() != DYVlc::Playing) {
            if(!_posTimer->isActive()) {
                if(!MediaPlayer->audio()->isMute())
                    MediaPlayer->audio()->toggleVolunMute();
                MediaPlayer->togglePause();
                _posTimer->start();
            }
        }
        if(_startTime != start) {
            MediaPlayer->setPosition(start);
        } else if(_endTime != end) {
            if(!_showPlay)
                MediaPlayer->setPosition(end);
        }
    }

    _startTime  = start;
    _endTime    = end;
}

void EditRightWidget::stopShowPre()
{
    if(_showPlay) {
        _showPlay = !_showPlay;
        if(!MediaPlayer->audio()->isMute()) {
            MediaPlayer->audio()->toggleVolunMute();
        }
        MediaPlayer->togglePause();
        _showPreBtn->setText(Lang("EDIT/showpre"));
    }
}

void EditRightWidget::resizeEvent(QResizeEvent *event)
{
    upUI();
}
