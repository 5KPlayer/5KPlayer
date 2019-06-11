#include "listnewnode.h"
#include "globalarg.h"
ListNewNode::ListNewNode(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void ListNewNode::updateLanguage()
{
    _newtext = Lge->getLangageValue("NewNode/item");
    _edit->setText(_newtext);
    _text->setText(_newtext);
    upUI();
}

void ListNewNode::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _ico->updateUI(scaleRatio);
    upUI();
}

void ListNewNode::Init()
{
    _newtext = Lge->getLangageValue("NewNode/item");
    _ico = new SvgButton(this);
    _text= new DYLabel(this);
    _edit = new dyLineEdit(this);
    _text->raise();
    _edit->setContextMenuPolicy(Qt::NoContextMenu);
    _edit->setText(_newtext);
    _text->setText(_newtext);
    _text->setStyleSheet("Color:rgb(92,94,102)");
    _text->setAlignment(Qt::AlignVCenter);
    _edit->setStyleSheet("background-color: rgb(255, 255, 255);");
    connect(_text,SIGNAL(clicked()),SLOT(EditShow()));
    connect(_edit,SIGNAL(textChanged(QString)),SLOT(EditChange(QString)));
    connect(_edit,SIGNAL(returnPressed()),SLOT(EditEnter()));
    connect(_edit,SIGNAL(foucsOut()),SLOT(EditFoucsOut()));
    connect(_ico,SIGNAL(clicked()),SLOT(EditShow()));

    isClick = false;
    isEnv = false;
    _scaleRatio = 1.0;
    _ico->initSize(20,20);
    _ico->setImagePrefixName("list");
}

void ListNewNode::upUI()
{
    QFont font;
    font.setPixelSize(this->height()/3);
    font.setFamily(Global->getFontFamily());
    _edit->setFont(font);
    _text->setFont(font);
    _ico->setGeometry(20*_scaleRatio,(this->height()-20*_scaleRatio)/2,20*_scaleRatio,20*_scaleRatio);
    _edit->setGeometry(_ico->x()+_ico->width()+5*_scaleRatio,_ico->y(),_edit->fontMetrics().width(_edit->text())+5*_scaleRatio,20*_scaleRatio);
    _text->setGeometry(_edit->x(),0,this->width()-_ico->x(),this->height());
}

bool ListNewNode::isVailName(QString itemName)
{
    bool isVaile = true;
    for(int i=0;i<11;i++) {
        if(QString::compare(Global->_constItemName[i],itemName,Qt::CaseInsensitive) == 0) {
            isVaile = false;
            break;
        }
    }
    if((itemName== "") || (QString::compare(itemName,_newtext,Qt::CaseInsensitive) == 0)) {
        isVaile = false;
    }
    return isVaile;
}

void ListNewNode::EditShow()
{
    if (isClick){
        isClick = false;
        _text->setText(_newtext);
        _text->setFocus();
        _text->raise();
        emit setmouseig(false);
        if(isVailName(_edit->text())) {
            emit EditChanged(_edit->text());
        }
        _text->setText(_newtext);
        _edit->setText(_newtext);
    } else {
        isClick = true;
        _text->setText("");
        _edit->setFocus();
        _edit->raise();
        emit setmouseig(false);
        _edit->selectAll();
    }
}

void ListNewNode::EditChange(QString s)
{
    _edit->resize(_edit->fontMetrics().width(s)+5,_edit->height());
}

void ListNewNode::EditEnter()
{
    isClick = false;
    QString text = _edit->text();
    emit setmouseig(false);
    if(isVailName(text)) {
        emit EditChanged(text);
    }
    _text->setText(_newtext);
    _edit->setText(_newtext);
    _text->raise();
}

void ListNewNode::EditFoucsOut()
{
    if(!isEnv){
        isClick = false;
        _text->setText(_newtext);
        _text->raise();
        _text->setFocus();
        if(isVailName(_edit->text())) {
            emit EditChanged(_edit->text());
        }
        _text->setText(_newtext);
        _edit->setText(_newtext);
    }
    emit setmouseig(false);
}

void ListNewNode::resizeEvent(QResizeEvent *)
{
    upUI();
}

void ListNewNode::enterEvent(QEvent *)
{
    isEnv = true;
}

void ListNewNode::leaveEvent(QEvent *)
{
    isEnv = false;
    emit setmouseig(true);
}
