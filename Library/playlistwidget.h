#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include "dystructu.h"
#include "transeventwidget.h"
#include "updateinterface.h"
#include "youtubewidget.h"
class IcosModeWidget;
class ListVideoWidget;
class ListMusicWidget;
class PlayListHead;
class PlayListWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit PlayListWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    enum ShowWidget{
        IcoWidget = 0,
        ListWidget,
        MusicWidget
    };

public slots:
    void listModeChange(QAction*);
    void arragemenuChange(QAction*act);
    void itemClickSlt(const QString &itemName);
    void addItem();
    void delItem();
    void upmediaWidget(bool bUp,bool bconvert,QString itemName);           // 更新数据并更新列表
    void upresizeWidget();                  // 窗口大小发生改变，需检查是否需要加载数据
    void clearAllWidgetItem();              //当有添加和删除文件时，ico和list需要重新加载
signals:
private:
    void Init();
    void upMaxCount(bool up);
    void upWidget();                        //更新列表，如果当前选中列表信息是界面中的列表，则不更新数据
    void hideAllWidget();                   //隐藏所有显示界面
    void showcurwidget(const QString &itemName);
protected:
    virtual void resizeEvent(QResizeEvent *);
private :
    PlayListHead* _listHead;
    ListVideoWidget* _listModeWidget;
    IcosModeWidget*  _icoModeWidget;
    ListMusicWidget* _musicWidget;
    TransEventWidget* _mediaWidget;
    YoutubeWidget* _youbetu;
    QString _curListItem;
    QList<MEDIAINFO> _mediaList;
    double _scaleRatio;
    QString _prePath;
    bool _music;
    ShowWidget _showWidget;

};

#endif // PLAYLISTWIDGET_H
