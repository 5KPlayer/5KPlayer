#ifndef LISTNODES_H
#define LISTNODES_H

#include <QWidget>
#include <QLabel>
#include "transeventwidget.h"
#include "transeventico.h"
#include "dystructu.h"
#include "dylabel.h"
#include "updateinterface.h"
#include "svgbutton.h"
class ListNodes : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit ListNodes(bool isdelete = false,QWidget *parent = 0);

    void setFontColor(QString color);
    void setNodeData(QString icoPath,QString name,int count);
    void UpdataNum(int count);
    int  getRedNum();

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
    void setNumRed(bool state);
    bool getNumRed(){return _numRed;}
    void setNodeText(QString name);
    void setSelect(bool select);
    QString getNodeItem() {return _itemName;}
private:
    void Init();
    void upUI();
    QList<MEDIAINFO> dropAddMedia();
signals:
    void deleteNode(QString);
private slots:
    void clickDel();

protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void dragEnterEvent(QDragEnterEvent *);//拖动进入事件
    void dragLeaveEvent(QDragLeaveEvent *);//拖动离开事件
    void dropEvent(QDropEvent *);          //放下事件
private:
    TransEventIco* _ico;
    QLabel* _name;
    QLabel* _count;
    SvgButton* _delbtn;

    QString _itemName;
    bool _delFlag;
    bool _numRed;
    bool _isSelect;
    double _scaleRatio;
};

#endif // LISTNODES_H
