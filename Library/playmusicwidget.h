#ifndef PLAYMUSICWIDGET_H
#define PLAYMUSICWIDGET_H

#include <QWidget>
#include "updateinterface.h"
#include "transeventwidget.h"
#include "transeventlabel.h"
#include "transeventicoclk.h"
#include "transeventico.h"
#include <QMovie>
#include "dylabel.h"
#include "dystructu.h"

class PlayMusicWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit PlayMusicWidget(QWidget *parent = 0);

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void setPlayMedia(const MEDIAINFO &media, bool isAirPlay);
    void setPlayImage(const QByteArray &imageData);
    void setPlayTitle(const QString &titleName);
    void setLiveInfo(const QString &imagePath, const QString &title);
    void upWidgtet();

private:
    void Init();
    void upUI();
    void upfavoritesbtn();

public slots:
    void favoritesClick();

protected:
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;

private:
    TransEventIco*      _airPlayFlag;   // airplay图标
    TransEventLabel*    _musicTitle;    // 音乐标题
    TransEventIcoClk*   _favorites;     // 收藏按钮

    TransEventLabel*    _musiclog;      // 音乐封面面板
    TransEventLabel*    _musicInfo;     // 音乐信息面板

    TransEventLabel*    _musicIco;      // 音乐的封面
    QMovie*             _musicMovie;    // 音乐旋转
    MEDIAINFO           _mediaInfo;
    double              _scaleRatio;
    QByteArray          _imageData;     // 封面图片信息
    QString             _imagePath;     // 封面图片路径
    QString             _titleData;     // 音乐title信息
};

#endif // PLAYMUSICWIDGET_H
