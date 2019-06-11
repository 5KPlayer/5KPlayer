#include "listroots.h"
#include "globalarg.h"
ListRoots::ListRoots(bool show, QWidget *parent)
    : TransEventWidget(parent)
{
    _isShowBtn = show;
    Init();
}

void ListRoots::setRootText(const QString &name, const QString &color)
{
    _name->setText(name);
    setStyleSheet(QString("background-color: %1;").arg(color));
}

void ListRoots::setBtnNameShow(bool isShow)
{
    _isShow = isShow;
    if(_isShow)
        _showBtn->setText(_show);
    else
        _showBtn->setText(_hide);
}

void ListRoots::updateLanguage()
{
    upUI();
    _show = Lge->getLangageValue("ListRoot/Show");
    _hide = Lge->getLangageValue("ListRoot/Hide");
    if(_isShow)
        _showBtn->setText(_show);
    else
        _showBtn->setText(_hide);
}

void ListRoots::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void ListRoots::Init()
{
    _name = new QLabel(this);
    _showBtn = new DYLabel(this);
    _show = Lge->getLangageValue("ListRoot/Show");
    _hide = Lge->getLangageValue("ListRoot/Hide");
    _isShow = false;
    _showBtn->setText(_hide);
    _showBtn->hide();
    _name->setStyleSheet("color: rgb(255, 255, 255);");
    _showBtn->setStyleSheet("color: rgb(255, 255, 255);");
    connect(_showBtn,SIGNAL(clicked()),SIGNAL(clickShow()));
    _scaleRatio = 1.0;
}

void ListRoots::upUI()
{
    QFont font;
    font.setPixelSize(this->height()*2/5);
    font.setFamily(Global->getFontFamily());
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setLetterSpacing(QFont::AbsoluteSpacing,1);
    _name->setFont(font);
    _showBtn->setFont(font);
    _name->setGeometry(4*_scaleRatio,(35-30)*_scaleRatio/2,120*_scaleRatio,30*_scaleRatio);
    _showBtn->setGeometry(this->width()-45*_scaleRatio,_name->y(),45*_scaleRatio,30*_scaleRatio);
}

void ListRoots::resizeEvent(QResizeEvent *)
{
    upUI();
}

void ListRoots::enterEvent(QEvent *)
{
    if(_isShowBtn)
        _showBtn->show();
}

void ListRoots::leaveEvent(QEvent *)
{
    _showBtn->hide();
}
