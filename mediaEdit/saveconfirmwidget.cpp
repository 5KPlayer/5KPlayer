#include "saveconfirmwidget.h"
#include <QLineEdit>
#include <QDateTime>
#include <QProgressBar>

#include <QDesktopServices>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QProcess>
#include <QTimer>

#include "dylabel.h"
#include "svgbutton.h"
#include "globalarg.h"
#include "openfile.h"
#include "dycheckbox.h"
#include "systemfunc.h"

SaveConfirmWidget::SaveConfirmWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    Init();
}

SaveConfirmWidget::~SaveConfirmWidget()
{
    if(_saveThread && _saveThread->isRunning())
    {
        _saveThread->endSaving();
        _saveThread->wait();
    }
    _saveThread = NULL;

}

void SaveConfirmWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _openFolder->updateUI(scaleRatio);
    upUI();
}

void SaveConfirmWidget::setSaveInfo(CutParams params)
{
    _params = params;
    initParamsInfo();
}

void SaveConfirmWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}

void SaveConfirmWidget::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(13 * _scaleRatio);
    _curTitleLabel->setFont(font);
    _destinNameLabel->setFont(font);
    _startLabel->setFont(font);
    _endLabel->setFont(font);
    _durationLabel->setFont(font);
    _curTimeLabel->setFont(font);
    _fpsLabel->setFont(font);
    _remainderLabel->setFont(font);

    _hardLabel->setFont(font);

    _curTitle->setFont(font);
    _destinName->setFont(font);
    _startTime->setFont(font);
    _endTime->setFont(font);
    _durationTime->setFont(font);
    _curTime->setFont(font);
    _remainderTime->setFont(font);
    _fpsCount->setFont(font);
    _pleaseLabel->setFont(font);

    font.setPixelSize(12 * _scaleRatio);
    _hardNvida->setFont(font);
    _hardAMD->setFont(font);
    _hardIntel->setFont(font);
    _hardCPU->setFont(font);

    font.setPixelSize(19 * _scaleRatio);
    _cancelBtn->setFont(font);

    int labelWidth = 110*_scaleRatio;
    int labelHeight = 20*_scaleRatio;

    int leftSpace   = 20 * _scaleRatio;
    int rigSpace    = 150* _scaleRatio;

    _curTitleLabel->setGeometry(leftSpace,  12*_scaleRatio,   240*_scaleRatio,labelHeight);
    _curTitle->setGeometry(leftSpace,       34*_scaleRatio,   240*_scaleRatio,labelHeight);
    _destinNameLabel->setGeometry(leftSpace,66*_scaleRatio,   240*_scaleRatio,labelHeight);
    _destinName->setGeometry(leftSpace,     88*_scaleRatio,   240*_scaleRatio,labelHeight);

    _startLabel->setGeometry(leftSpace,150*_scaleRatio,   labelWidth,labelHeight);
    _startTime->setGeometry(leftSpace, 172*_scaleRatio,  labelWidth,labelHeight);

    _endLabel->setGeometry(rigSpace,   150*_scaleRatio,   labelWidth,labelHeight);
    _endTime->setGeometry(rigSpace,    172*_scaleRatio,  labelWidth, labelHeight);

    _durationLabel->setGeometry(leftSpace, 204*_scaleRatio,labelWidth,labelHeight);
    _durationTime->setGeometry(leftSpace,  226*_scaleRatio,labelWidth,labelHeight);

    _curTimeLabel->setGeometry(rigSpace,   204*_scaleRatio,labelWidth,labelHeight);
    _curTime->setGeometry(rigSpace,        226*_scaleRatio,labelWidth,labelHeight);

    _fpsLabel->setGeometry(leftSpace,        258*_scaleRatio,labelWidth,labelHeight);
    _fpsCount->setGeometry(leftSpace,        280*_scaleRatio,labelWidth,labelHeight);

    _remainderLabel->setGeometry(rigSpace,   258*_scaleRatio,labelWidth,labelHeight);
    _remainderTime->setGeometry(rigSpace,    280*_scaleRatio,labelWidth,labelHeight);

    _hardLabel->setGeometry(leftSpace,348*_scaleRatio,labelWidth,labelHeight);
    int hardLabW = 48*_scaleRatio;
    _hardNvida->setGeometry(36*_scaleRatio, 372*_scaleRatio,hardLabW,labelHeight);
    _hardAMD->setGeometry(88*_scaleRatio,   372*_scaleRatio,hardLabW,labelHeight);
    _hardIntel->setGeometry(140*_scaleRatio,372*_scaleRatio,hardLabW,labelHeight);
    _hardCPU->setGeometry(192*_scaleRatio,  372*_scaleRatio,hardLabW,labelHeight);

    _pleaseLabel->setGeometry(leftSpace,428*_scaleRatio,labelWidth,labelHeight);
    _progress->setGeometry(leftSpace,452*_scaleRatio,240*_scaleRatio,8*_scaleRatio);

    _openFolder->setGeometry(leftSpace,500*_scaleRatio,260*_scaleRatio,20*_scaleRatio);

    _saveLine->setGeometry(4*_scaleRatio,550*_scaleRatio,this->width(),1*_scaleRatio);

    int saveY = _saveLine->y() + (634*_scaleRatio - 550*_scaleRatio -  36*_scaleRatio)/2;
    if(this->height() > 634*_scaleRatio) {
        saveY = _saveLine->y() + (this->height() - _saveLine->y() - 36*_scaleRatio)/2;
    }

    _cancelBtn->setGeometry(60*_scaleRatio,saveY,160*_scaleRatio,36*_scaleRatio);
}

void SaveConfirmWidget::openCutPath()
{
    QString filePath = getCorrectPath(_openCutPath, _desFileName);
    my_openpathClick(filePath);
}

void SaveConfirmWidget::onSaveFinished()
{
    _timeOutCount = 0;
    _time->stop();
    emit cancled();

    //转码结束
    if(_openFolder->isChecked())
        openCutPath();
    if(_saveThread) {
        _saveThread->deleteLater();
        _saveThread = NULL;
    }
}

void SaveConfirmWidget::onSaveProgress(int current, int total, int fps)
{
    int percent = (float)current / (float)total * 100;
    _progress->setValue(percent);
    _fpsCount->setText(QString::number(fps));
}

void SaveConfirmWidget::onOpenBtnChange(bool check)
{
    Global->setOpenCutFolder(check);
}

void SaveConfirmWidget::Init()
{
    _scaleRatio = Global->_screenRatio;
    initLabel(&_curTitleLabel,  Lang("EDIT/curtitle"));
    initLabel(&_destinNameLabel,Lang("EDIT/desname"));
    initLabel(&_startLabel,     Lang("EDIT/startime"));
    initLabel(&_endLabel,       Lang("EDIT/endtime"));
    initLabel(&_durationLabel,  Lang("EDIT/duration"));
    initLabel(&_curTimeLabel,   Lang("EDIT/curtime"));
    initLabel(&_fpsLabel,       "FPS:");
    initLabel(&_remainderLabel, Lang("EDIT/remtime"));
    initLabel(&_hardLabel,      Lang("EDIT/hard"));
    initLabel(&_pleaseLabel,    Lang("EDIT/wait"));

    initLabel(&_curTitle,       "",true);
    initLabel(&_destinName,     "",true);
    initLabel(&_startTime,      "00:00:00",true);
    initLabel(&_endTime,        "00:00:00",true);
    initLabel(&_durationTime,   "00:00:00",true);
    initLabel(&_curTime,        "00:00:00",true);
    initLabel(&_remainderTime,  "00:00:00",true);
    initLabel(&_fpsCount,       "0",true);

    initHardLabel(&_hardNvida,"Nvidia", Global->hasEncHardware(CardNvidia,"h264"));
    initHardLabel(&_hardAMD,  "AMD",    Global->hasEncHardware(CardAMD,"h264"));
    initHardLabel(&_hardIntel,"Intel",  Global->hasEncHardware(CardIntel,"h264"));
    bool cpuAct = true;
    if(Global->hasEncHardware(CardNvidia,"h264")
       || Global->hasEncHardware(CardAMD,"h264")
       || Global->hasEncHardware(CardIntel,"h264")) {
        cpuAct = false;
    }
    initHardLabel(&_hardCPU,"CPU",cpuAct);

    _openFolder = new DYCheckBox(this,Lang("EDIT/open"));
    _openFolder->setTextColor("#8d8d9a");
    _openFolder->setChecked(Global->getOpenCutFolder());
    connect(_openFolder,SIGNAL(checkChange(bool)),SLOT(onOpenBtnChange(bool)));

    _saveLine = new QLabel(this);
    _saveLine->setStyleSheet("background-color: rgb(53, 53, 53)");

    _timeOutCount   = 0;
    _time           = new QTimer(this);
    _time->setInterval(500);
    connect(_time,SIGNAL(timeout()),SLOT(timeOutSlt()));
    _startEditime   = new QTimer(this);
    _startEditime->setInterval(1000);
    _startEditime->setSingleShot(true);
    connect(_startEditime,&QTimer::timeout,[&]{
        _time->start();
        saveCutFile();
    });

    _saveThread = NULL;

    _cancelBtn = new DYLabel(this,true);
    _cancelBtn->setBackClolr("rgb(240,72,74)","rgb(248,113,107)","rgb(120,37,36)");
    _cancelBtn->setText(Lang("Main/cancel"));
    _cancelBtn->setAlignment(Qt::AlignCenter);
    //点击取消
    connect(_cancelBtn,&DYLabel::clicked,[&]{
        if(_saveThread && _saveThread->isRunning())
        {
            _saveThread->endSaving();
            _saveThread->wait();
            _saveThread->deleteLater();
            _saveThread = NULL;
        }

        if(_startEditime->isActive())
            _startEditime->stop();
        _timeOutCount = 0;
        _time->stop();
        emit cancled();
    });

    _progress = new QProgressBar(this);
    _progress->setValue(0);
    _progress->setTextVisible(false);

    QString path = Global->editVideopath();
    QDir dir(QDir::toNativeSeparators(path));
    if(!dir.exists())
    {
        dir.mkdir(path);
    }
#ifdef Q_OS_WIN
    path.replace("/", "\\");
#else
    path.replace("\\", "/");
#endif
    _openCutPath = path;

    _scaleRatio = Global->_screenRatio;
    updateUI(_scaleRatio);
}

void SaveConfirmWidget::initLabel(QLabel **label, QString text,bool line)
{
    (*label) = new QLabel(text, this);
    (*label)->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    if(line) {
        (*label)->setStyleSheet("QLabel{ \
                                color:rgb(141,141,144); \
                                border-bottom:1px solid #8d8d90; \
                                }");
    } else {
        (*label)->setStyleSheet("color:rgb(141,141,144)");
    }
}

void SaveConfirmWidget::initHardLabel(QLabel **label, QString text, bool action)
{
    (*label) = new QLabel(text, this);
    (*label)->setAlignment(Qt::AlignCenter);
    QString style = "";
    if(action) {
        style = QString("border-radius: %1px;background-color:rgb(71,204,116);"
                        "color:rgb(214,224,224);").arg(4*_scaleRatio);
    } else {
        style = QString("border-radius: %1px;background-color:rgb(94,94,98);"
                        "color:rgb(214,224,224);").arg(4*_scaleRatio);
    }
    (*label)->setStyleSheet(style);
}

void SaveConfirmWidget::initParamsInfo()
{
    QTime start,end,duration;
    int startSec = _params.startTime / 1000;
    int endSec = _params.endTime / 1000;
    int durationSec = (_params.endTime - _params.startTime) / 1000 / _params.speed;
    start.setHMS(startSec / 3600,startSec % 3600 / 60,startSec % 60);
    end.setHMS(endSec / 3600,endSec % 3600 / 60,endSec % 60);
    duration.setHMS(durationSec / 3600,durationSec % 3600 / 60,durationSec % 60);
    QString startTime = start.toString("HH:mm:ss");
    QString endTime = end.toString("HH:mm:ss");
    QString durationTime = duration.toString("HH:mm:ss");

    _startTime->setText(startTime);
    _endTime->setText(endTime);
    _durationTime->setText(durationTime);
    QFileInfo file(_params.filePath);
    _desFileName  = file.fileName();
    QString outpath = getCorrectPath(_openCutPath, _desFileName);

    int count = 0;
    while(QFileInfo(outpath).exists()) {
        _desFileName = QString(file.completeBaseName()+"_%1." + file.suffix()).arg(count);
        count++;
        outpath = getCorrectPath(_openCutPath, _desFileName);
    }

    setLabelText(_curTitle,file.fileName());
    setLabelText(_destinName,_desFileName);
    _curTime->setText("00:00:00");
    _remainderTime->setText("00:00:00");
    _fpsCount->setText("0");
    _progress->setValue(0);
    _timeOutCount = 0;
    _startEditime->start();
}

void SaveConfirmWidget::saveCutFile()
{
    _params.outFilePath = getCorrectPath(_openCutPath, _desFileName);

    if(_saveThread == NULL)
        _saveThread = new EditSaveThread(_params,this);
    if(!_saveThread->isRunning())
        _saveThread->start();
    connect(_saveThread,SIGNAL(saveProgressAndFPS(int,int,int)),SLOT(onSaveProgress(int,int,int)));
    connect(_saveThread,SIGNAL(saveDone()),SLOT(onSaveFinished()));
}

void SaveConfirmWidget::setLabelText(QLabel *label, QString text)
{
    int nameSize   = text.size();
    int nameLength = label->fontMetrics().width(text);
    if(label->width() < nameLength) {
       QString showtext = "";
       for(int i=0;i<nameSize;i++) {
           showtext = text.left(i).append("...");
           int showWidth = label->fontMetrics().width(showtext);
           if(showWidth > label->width()) {
               showtext = text.left(i-1).append("...");
               break;
           }
       }
       label->setText(showtext);
       label->setToolTip(text);
    } else {
       label->setText(text);
    }
}

QString SaveConfirmWidget::getCorrectPath(QString path, QString fileName)
{
    QString filePath = "";
#ifdef Q_OS_WIN
    filePath = path + "\\" + fileName;
#else
    filePath = path + "/" + fileName;
#endif
    return filePath;
}

void SaveConfirmWidget::timeOutSlt()
{
    _timeOutCount++;
    QString curTime = Global->secondToTime(_timeOutCount/2);
    _curTime->setText(curTime);
    // 每500ms完成的进度
    double complete = (double)_progress->value() / _timeOutCount;
    double desPro   = (double)(100-_progress->value()) / complete;

    QString desTime = Global->secondToTime(desPro/2);
    _remainderTime->setText(desTime);

    int wait = _timeOutCount % 4;
    QString waitStr = Lang("EDIT/wait");
    switch (wait) {
    case 1:
        waitStr.append(".");
        break;
    case 2:
        waitStr.append("..");
        break;
    case 3:
        waitStr.append("...");
        break;
    default:
        break;
    }
    _pleaseLabel->setText(waitStr);
}
