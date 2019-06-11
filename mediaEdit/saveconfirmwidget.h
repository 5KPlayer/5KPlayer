#ifndef SAVECONFIRMWIDGET_H
#define SAVECONFIRMWIDGET_H

#include "updateinterface.h"
#include "editrightwidget.h"
#include "editsavethread.h"
class TransEventLabel;
class QLineEdit;
class DYLabel;
class QProgressBar;
class SvgButton;
class QProcess;
class CDialog;

class SaveConfirmWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SaveConfirmWidget(QWidget *parent = 0);
    ~SaveConfirmWidget();
    void updateUI(const qreal &scaleRatio);
    void updateLanguage(){}

    void setSaveInfo(CutParams params);

protected:
    void resizeEvent(QResizeEvent *);
private:
    void Init();
    void initLabel(QLabel** label,QString text = "",bool line = false);
    void initHardLabel(QLabel** label, QString text,bool action);
    void upUI();
    void initParamsInfo();
    void saveCutFile();
    void setLabelText(QLabel* label,QString text);
    QString getCorrectPath(QString path,QString fileName);

signals:
    void cancled();
public slots:
    void timeOutSlt();
    void openCutPath();
    void onSaveFinished();
    void onSaveProgress(int current,int total,int fps);
    void onOpenBtnChange(bool check);

private:
    QLabel *_curTitleLabel;
    QLabel *_curTitle;     // 当前文件名

    QLabel *_destinNameLabel;
    QLabel *_destinName;     // 开始时间

    QLabel *_startLabel;
    QLabel *_startTime;     // 开始时间

    QLabel *_endLabel;
    QLabel *_endTime;      // 结束时间

    QLabel *_durationLabel;
    QLabel *_durationTime; // 文件长度

    QLabel *_curTimeLabel;
    QLabel *_curTime;      // 当前时间

    QLabel *_remainderLabel;
    QLabel *_remainderTime; // 剩余时间

    QLabel *_fpsLabel;
    QLabel *_fpsCount;      // Fps

    QLabel *_hardLabel;     // 硬件
    QLabel *_hardNvida;
    QLabel *_hardAMD;
    QLabel *_hardIntel;
    QLabel *_hardCPU;

    QLabel *_pleaseLabel;
    QProgressBar *_progress;

    DYCheckBox*   _openFolder;

    QLabel          *_saveLine;

    DYLabel *_cancelBtn;

    QTimer*     _time;
    QTimer*     _startEditime;
    int         _timeOutCount;

    double      _scaleRatio;
    QString     _openCutPath;
    QString     _desFileName;

    CutParams _params;
    QProcess *proc;

    EditSaveThread *_saveThread;

};

#endif // SAVECONFIRMWIDGET_H
