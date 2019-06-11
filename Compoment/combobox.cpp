#include "combobox.h"
#include <QListView>
#include <QScrollBar>
#include <QWheelEvent>
#include "globalarg.h"

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
    this->setView(new QListView());
}

void ComboBox::setFont(const QFont &font)
{
    this->QComboBox::setFont(font);
    this->view()->setFont(font);
}

void ComboBox::upStyleSheet(const qreal &ratio)
{
    //设置滚动条样式
    this->view()->verticalScrollBar()->setStyleSheet(verticalStyle(ratio));

    //设置当前样式
    this->setStyleSheet(style(ratio));
}

void ComboBox::setViewWidth(const uint &w)
{
    this->view()->setMinimumWidth(w);
}

const QString ComboBox::style(const qreal &ratio) const
{
    return QString("QComboBox{background-color: white;border: 1px solid gray;border-radius: %1px;}\n"
                   "QComboBox::drop-down{subcontrol-origin: padding;subcontrol-position: top right;"
                   "width: %2px;border-left-width: 0px;border-left-color: darkgray;border-left-style: solid;"
                   "border-top-right-radius: %1px;border-bottom-right-radius: %1px;}\n"
                   "QComboBox::down-arrow{image: url(:/res/png/sanjiao_1.png);}\n"
                   "QComboBox QAbstractItemView{background: white;selection-background-color: rgb(51,153,255);outline:0px}\n"
                   "QComboBox QAbstractItemView::item{min-height: %3px;}")
            .arg(4  * ratio)
            .arg(16 * ratio)
            .arg(28 * ratio);
}

void ComboBox::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

const QString ComboBox::verticalStyle(const qreal &ratio, const QString &color)
{
    return QString(// 设置垂直滚动条基本样式
                   "QScrollBar:vertical{width:%1px;padding-top:0px;padding-bottom:0px;"
                   "margin:0px,0px,0px,0px;background: %2;}"
                   // 滚动条
                   "QScrollBar::handle:vertical{width:%1px;background:rgba(0,0,0,25%);}"
                   // 鼠标放到滚动条上
                   "QScrollBar::handle:vertical:hover{width:%1px;background:rgba(0,0,0,50%);}"
                   // 当滚动条滚动的时候，上面的部分和下面的部分
                   "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:rgba(0,0,0,10%);}"
                   // 设置上箭头
                   "QScrollBar::sub-line:vertical{height:0px;width:0px;subcontrol-position:top;}"
                   // 设置下箭头
                   "QScrollBar::add-line:vertical{height:0px;width:0px;subcontrol-position:bottom;}")
            .arg(16  * ratio)
            .arg(color);
}
