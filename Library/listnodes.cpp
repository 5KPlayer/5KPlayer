#include "listnodes.h"
#include <QDebug>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include "globalarg.h"
#include "librarydata.h"
ListNodes::ListNodes(bool isdelete, QWidget *parent)
    : TransEventWidget(parent)
{
    _delFlag    = isdelete;
    _isSelect   = false;
    this->setAcceptDrops(true);
    Init();
}

void ListNodes::setFontColor(QString color)
{
    _name->setStyleSheet(QString("color:%1").arg(color));//("color: rgb(255,255,255);");
}

void ListNodes::setNodeData(QString icoPath, QString name, int count)
{
    _ico->load(icoPath);
    _name->setText(name);
    _itemName = name;
    if(count > 0)
       _count->setText(QString::number(count));
    else
       _count->setText("");
}

void ListNodes::UpdataNum(int count)
{
    _count->setText("");
    if(count > 0) {
        _count->setText(QString::number(count));
    }
    int countWidth = _count->fontMetrics().width(_count->text());
    if(countWidth > _count->width()) {
        _count->setGeometry(this->width()-(20+countWidth)*_scaleRatio,(this->height()-20*_scaleRatio)/2,countWidth,20*_scaleRatio);
    }
}

int ListNodes::getRedNum()
{
    QString numStr = _count->text();
    int     num = 0;
    if(!numStr.isEmpty())
        num = numStr.toInt();
    return num;
}

void ListNodes::updateLanguage()
{
    upUI();
}

void ListNodes::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _delbtn->updateUI(scaleRatio);
    upUI();
}

void ListNodes::Init()
{
    _ico   = new TransEventIco(this);
    _name  = new QLabel(this);
    _count = new QLabel(this);
    _delbtn= new SvgButton(this);

    _name->setStyleSheet("color: rgb(92,94,102);");
    _count->setStyleSheet("color: rgb(92,94,102);");
    _scaleRatio = 1.0;
    _numRed     = false;

    _count->setAlignment(Qt::AlignCenter);
    connect(_delbtn,SIGNAL(clicked()),SLOT(clickDel()));
    _delbtn->initSize(15,15);
    _delbtn->setImagePrefixName("strike_out");
    _delbtn->hide();
    upUI();
}

void ListNodes::upUI()
{
    int InfoH = 11*_scaleRatio;
    int icoH = 20*_scaleRatio;
    _ico->setGeometry(icoH,(this->height()-icoH)/2,icoH,icoH);
    _name->setGeometry(_ico->x()+_ico->width()+5*_scaleRatio,(this->height()-InfoH)/2,90*_scaleRatio,InfoH);
    _delbtn->setGeometry(this->width() - 20*_scaleRatio,(this->height()-_delbtn->height())/2,icoH,icoH);
    int countWidth = _count->fontMetrics().width(_count->text());
    if(countWidth > _count->width()) {
        _count->setGeometry(this->width()-(20+countWidth)*_scaleRatio,(this->height()-20*_scaleRatio)/2,countWidth,20*_scaleRatio);
    } else {
        _count->setGeometry(this->width()-40*_scaleRatio,(this->height()-icoH)/2,20*_scaleRatio,icoH);
    }
    QFont font;
    font.setPixelSize(InfoH);
    font.setFamily(Global->getFontFamily());
    font.setWeight(57);
    font.setLetterSpacing(QFont::AbsoluteSpacing,1);
    _name->setFont(font);
    _count->setFont(font);
}

QList<MEDIAINFO> ListNodes::dropAddMedia()
{
    QList<MEDIAINFO> dropList;
    QList<MEDIAINFO> curList;       // 当前列表存在的媒体
    dropList.clear();
    curList.clear();
    curList = LibData->getMediaforItem(_itemName);
    for(int i=0;i<LibData->selmedialist.count();i++) {
        bool fileExits = false;
        if(LibData->selmedialist.at(i).itemname != _itemName) {
            for(int j=0;j<curList.count();j++) {
                if(QString::compare(curList.at(j).filepath,LibData->selmedialist.at(i).filepath,Qt::CaseInsensitive) == 0) {
                    fileExits = true;
                    break;
                }
            }
            if(!fileExits) {
                MEDIAINFO media = LibData->selmedialist.at(i);
                media.itemname = _itemName;
                media.fileAddTime = LibData->getCurData();
                dropList.append(media);
            }
        }
    }
    return dropList;
}

void ListNodes::setNumRed(bool state)
{
    _numRed = state;
    if(state)
        _count->setStyleSheet(QString("color: rgb(255, 255, 255);background-color: rgb(255, 0, 0);border-radius:%1px").arg(_count->height()/2));
    else
        _count->setStyleSheet(QString("color: rgb(92,94,102);border-radius:%1px").arg(_count->height()/2));
}

void ListNodes::setNodeText(QString name)
{
    _name->setText(name);
}

void ListNodes::setSelect(bool select)
{

    _isSelect = select;
    if(_isSelect) {
        this->setStyleSheet("background-color: rgb(31, 31, 31);");
        setFontColor("rgb(255,255,255)");
    } else {
        this->setStyleSheet("background-color: rgb(14, 14, 15);");
        if(_delFlag) {
            setFontColor("rgb(255,170,51)");
        } else {
            setFontColor("rgb(92,94,102)");
        }
    }
}

void ListNodes::clickDel()
{
    emit deleteNode(_name->text());
}

void ListNodes::resizeEvent(QResizeEvent *)
{
    upUI();
}

void ListNodes::enterEvent(QEvent *)
{
    if(_delFlag)
        _delbtn->show();
    if(!_isSelect)
        this->setStyleSheet("background-color: rgb(51, 51, 56);");
}

void ListNodes::leaveEvent(QEvent *)
{
    if(!_isSelect)
        this->setStyleSheet("background-color: rgb(14, 14, 15);");
     _delbtn->hide();
}

void ListNodes::dragEnterEvent(QDragEnterEvent *e)
{
    if(_delFlag) {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    } else {
        e->setDropAction(Qt::IgnoreAction);
        e->accept();
    }
    if(!_isSelect)
        this->setStyleSheet("background-color: rgb(51, 51, 56);");
}

void ListNodes::dragLeaveEvent(QDragLeaveEvent *)
{
    if(!_isSelect)
        this->setStyleSheet("background-color: rgb(14, 14, 15);");
}

void ListNodes::dropEvent(QDropEvent *e)
{
    if(!e->mimeData()->hasFormat(MIME_TYPE))
        return;
    LibData->addDropMediaList(dropAddMedia());
}

