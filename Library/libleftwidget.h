#ifndef LIBLEFTWIDGET_H
#define LIBLEFTWIDGET_H

#include <QWidget>
#include <QListWidgetItem>
#include <QListWidget>
#include <QActionGroup>
#include "listnodes.h"
#include "listroots.h"
#include "listnewnode.h"
#include "dystructu.h"
#include "transevenlistwidget.h"
#include "updateinterface.h"
#include "librarydata.h"
class libLeftWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit libLeftWidget(QWidget *parent = 0);

    void InitConstItem(); //初始化固定的WidgetItem
    void InitItem();      //初始化数据库中的widgetitem；
    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
signals:
    void ListClick(QString name);
public slots:
    void itemClick(QListWidgetItem *it);
    void FavoritesClick();
    void YouTubeClick();
    void AddWidgetItem(const QString &name);
    void DelWidgetItem(const QString &name);
    void UpdataList(bool bUp, bool bconvert, QString itemName);
    void itemChange(const QString &itemName);
private:
    void Init();
    void InitLanguage();
    bool checkUpList(QString nodeName,QString itemName,bool bconvert);
protected:
    void resizeEvent(QResizeEvent *event);
private:
    ListNodes* _fMovies;
    ListNodes* _fMusic;
    ListNodes* _yVideos;
    ListNodes* _yMusic;
    ListNodes* _pMovies;
    ListNodes* _pMusic;
    ListNodes* _pAirRecord;
    ListRoots* _fFavorites;
    ListRoots* _yYouTube;
    ListRoots* _pPlayLists;
    ListNewNode*  _nListNew;
    QListWidgetItem* _WidgetItem[11];
    QListWidgetItem * _CurItem;
    QListWidgetItem * _PreItem;
    TransEvenListWidget* _listwidget;
    bool _init;
    int _nItemNum;
    int _curYtbNum;
    QList<QString> _itemList;
    double _scaleRatio;
};

#endif // LIBLEFTWIDGET_H
