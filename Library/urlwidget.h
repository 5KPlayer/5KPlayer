#ifndef URLWIDGET_H
#define URLWIDGET_H

#include "transeventwidget.h"
#include "transeventlabel.h"
#include "updateinterface.h"
#include "dylabel.h"
#include "dystructu.h"
#include "svgbutton.h"
#include "downbutton.h"
#include <QFile>
#include <QNetworkReply>


/*!
 * \brief The URLWidget class
 * Youtube的解析后显示的媒体列表中的子项的界面
 */
class URLWidget : public TransEventWidget ,public UpdateInterface
{
    Q_OBJECT
public:
    explicit URLWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setDataSoure(const MEDIAINFO &data, bool isDoneBtn = false);
    MEDIAINFO getDataSoure();

    void setIsSelect(bool isSel);
    bool getIsDown() {return _isDown;}
    void setDownState(bool state);
    void setDownProcess(double process);
    void setMediaSubtitle(QStringList subList);     // 设置媒体字幕信息

signals:
    void deleteClick(MEDIAINFO);            // 删除url媒体
    void showDetail(MEDIAINFO);             // 显示详细信息
    void downUrl(MEDIAINFO);                // 下载
    void stopDown(MEDIAINFO);               // 停止下载

private:
    void Init();
    void InitLanguage();
    void upUI();
    void uptitleName();
    // 更新显示
    void upDatadisplay();
    void setAnalyPanl();
    void setDataPanl();
    QString matchAudioforVideo();       // 根据当前的video的format信息匹配audio

    QString getvideoformat(const QString &formatids);
    QString getaudioformat(const QString &formatids);

    QString checkFormatIds(const QString &formats);  // 检查当前的formatId的音频和视频是否匹配,如果不匹配，则下载的视频不能合并成需要的格式

protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

public slots:
    void delClick();
    void detailedClick();
    void downClick();
    void clickstopDown();
    void clickReAnaly();

    void upProcessInfo(const QString &objectName, double process);
private:
    TransEventLabel*    _ico;
    TransEventLabel*    _title;
    TransEventLabel*    _name;
    TransEventLabel*    _resolution;
    TransEventLabel*    _resinfo;
    TransEventLabel*    _fileSize;
    TransEventLabel*    _size;
    SvgButton      *    _detailed;
    SvgButton      *    _down;
    SvgButton      *    _delBtn;
    DownButton     *    _downPro;
    TransEventLabel*    _downProSize;
    QLabel         *    _line;
    QLabel         *    _definition;
    DYLabel*            _reAnalyze;

    double              _scaleRatio;
    bool                _isSelect;
    bool                _isDown;
    bool                _downfinish;

    MEDIAINFO           _wdgData;
    int                 _curSelect;
    QList<_StreamInfo>  _audioFormat;
    QList<_StreamInfo>  _vidioFormat;
    _StreamInfo         _curVideoFormat;
    bool                _isAnaly;

};

#endif // URLWIDGET_H
