#ifndef LISTSVIDEO_H
#define LISTSVIDEO_H

#include "dystructu.h"
#include <QMenu>
#include <QProgressBar>
#include "updateinterface.h"
#include "transeventwidget.h"
class TransEventLabelBtn;
class TransEventLabel;
class TransEventIcoClk;
class TransEventIco;
class DYLabel;
class ListsVideo : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit ListsVideo(QWidget *parent = 0);
    explicit ListsVideo(bool isListMode = true,QWidget *parent = 0);

    void setDataSoure(const MEDIAINFO &data);
    MEDIAINFO getDataSoure();

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setIsSelect(bool select,bool ctrldown);
    bool getIsSelect() {return _mediaInfo.isSelect;}
    void setplayFlag(bool state);
    bool getSupAirPlay();
    void setConvertProValue(double proValur);
private:
    void Init();
    void upVideoInfo();
    void upFavoritesbtn();
    void upUI();
    void InitLanguage();
    bool fileNotExist(QString filepath);            // 检查文件是否存在，不存在，弹出提示框，返回false

public slots:
    void ActionMP4Click(bool);
    void ActionMP3Click(bool);
    void ActionAACClick(bool);
    void favoritesClick();
    void convertClick();
    void openpathClick();
    void converthide();
    void upplayflag(const MEDIAINFO &mediainfo);
    void airplayConvert(bool);

    void upProcess(QString objName,double time);
    void upAirPlayCon(bool con);

protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private:

    TransEventLabel* _videoIco;
    TransEventIco*   _titleIco;
    TransEventLabel* _titleName;
    TransEventLabel* _airPlaySup;
    TransEventLabel* _details;
    TransEventLabel* _path;
    TransEventLabel* _playFlag;
    TransEventLabel* _Line;
    TransEventLabel* _icoLine;
    TransEventLabel* _icoLine2;
    TransEventIcoClk* _favoritesbtn;
    TransEventIcoClk* _openpathbtn;
    DYLabel*    _convertbtn;
    QMovie*     _playMovie;
    QMenu*      _Menu;
    QAction*    _MP3;
    QAction*    _AAC;
    QAction*    _MP4;
    QProgressBar*       _convertBar;
    bool                _ListMode;
    MEDIAINFO           _mediaInfo;
    STREAMINFO          _streamInfo;

    double              _scaleRatio;

    TransEventLabel*    _airNoSup;
    TransEventLabel*    _airNoSupTitle;
    TransEventLabel*    _airNoSupConvert;
    TransEventLabel*    _airNoSupIco;
    TransEventLabel*    _airNoSupfixName;
    TransEventLabel*    _airSupcodec;
    TransEventLabel*    _airSupfixName;
    TransEventLabel*    _airSupcodecIco;
    TransEventLabel*    _airSupfixNameIco;
    TransEventLabelBtn* _videoConvert;

};

#endif // LISTSVIDEO_H
