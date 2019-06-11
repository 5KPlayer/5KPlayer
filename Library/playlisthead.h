#ifndef PLAYLISTHEAD_H
#define PLAYLISTHEAD_H

#include <QWidget>
#include <QMenu>
#include <QActionGroup>
#include "transeventwidget.h"
#include "transeventlabel.h"
#include "svgbutton.h"
#include "updateinterface.h"
#include <QLineEdit>
class PlayListHead : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit PlayListHead(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
    void isfavorites(bool state);
    void menuSortEnable(bool state);
    void upaddbtn(bool state);
    void clearSearchText();     // 清空Search的值，在切换item或者显示模式将清空
    void upMenuBtn(bool state); // 显示menu
signals:
    void listModemenuclk(QAction*);
    void arragemenuclk(QAction*);
    void addClick();
    void delClick();
    void playModeClick();
    void searchName(QString);
public slots:
    void menushow();
    void updelbtn(bool state);
    //void upplaymode();
    void EditChange(QString);
    void searchdelClick();
    void searchClick();
private:
    void Init();
    //void upPlayModeBtn();
    void upUI();
    void InitLanguage();
protected:
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void resizeEvent(QResizeEvent *);
private:
    SvgButton* _addbtn;
    SvgButton* _delbtn;
    //SvgButton* _playMode;
    SvgButton* _menu;

    TransEventLabel* _line;

    QSvgWidget*_searchwidget;
    SvgButton* _searchbtn;
    SvgButton* _searchdelbtn;
    QLineEdit* _searchEdit;

    double _scaleRatio;
    QActionGroup *_arrageGourp;
    QActionGroup *_listModeGourp;
    QMenu* _listMode;
    QAction* _Action[5];
    int _playListMode;
    bool _isFavorites;
};

#endif // PLAYLISTHEAD_H
