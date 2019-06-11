#include "transevenlistwidget.h"
#include <QMouseEvent>
#include <QScrollBar>
TransEvenListWidget::TransEvenListWidget(QWidget *parent)
    : QListWidget(parent)
{
    this->setMouseTracking(true);
    _ctrl_is_down = false;
    _shift_is_down= false;
    _mouse_ignore = true;
    _minselect = -1;
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void TransEvenListWidget::setminselect(const int &selectcount)
{
    if(_minselect==-1 || _minselect>selectcount)
        _minselect = selectcount;
}

void TransEvenListWidget::upDateStyle(qreal ratio)
{
    verticalScrollBar()->setStyleSheet(QString(// 设置垂直滚动条基本样式
                   "QScrollBar:vertical{width:%1px;padding-top:0px;padding-bottom:0px;"
                   "margin:0px,0px,0px,0px;background: rgb(22,22,23);}"
                   // 滚动条
                   "QScrollBar::handle:vertical{width:%1px;background:rgba(51,51,56,75%);}"
                   // 鼠标放到滚动条上
                   "QScrollBar::handle:vertical:hover{width:%1px;background:rgba(51,51,56,75%);}"
                   // 当滚动条滚动的时候，上面的部分和下面的部分
                   "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:rgba(0,0,0,10%);}"
                   // 设置上箭头
                   "QScrollBar::sub-line:vertical{height:0px;width:0px;subcontrol-position:top;}"
                   // 设置下箭头
                   "QScrollBar::add-line:vertical{height:0px;width:0px;subcontrol-position:bottom;}")
            .arg(4  * ratio)
            );
}

void TransEvenListWidget::mousePressEvent(QMouseEvent * e)
{
    QListWidget::mousePressEvent(e);
    if(_mouse_ignore)
        e->ignore();
    else
        e->accept();
}

void TransEvenListWidget::mouseMoveEvent(QMouseEvent *)
{
    // 屏蔽鼠标移动事件的穿透，使鼠标在listWidget中不能拖动
//    QListWidget::mouseMoveEvent(e);
//    e->ignore();
}

void TransEvenListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QListWidget::mouseReleaseEvent(e);
    e->ignore();
}

void TransEvenListWidget::keyPressEvent(QKeyEvent *event)
{
    QListWidget::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_A:
        if(event->modifiers()==Qt::ControlModifier) {
            emit widgetselall();
        }
        break;
    default:
        break;
    }
    switch (event->modifiers()) {
    case Qt::ControlModifier:
        _ctrl_is_down = true;
        break;
    case Qt::ShiftModifier :
        _shift_is_down= true;
        break;
    }
}

void TransEvenListWidget::keyReleaseEvent(QKeyEvent *event)
{
    QListWidget::keyReleaseEvent(event);
    if(event->modifiers()!=Qt::ControlModifier) {
        _ctrl_is_down = false;
    }
    if(event->modifiers()!=Qt::ShiftModifier) {
        _shift_is_down = false;
    }
}

void TransEvenListWidget::enterEvent(QEvent *e)
{
    QListWidget::enterEvent(e);
    e->ignore();
}

void TransEvenListWidget::wheelEvent(QWheelEvent *e)
{
    const int delta = e->angleDelta().y() / 8;
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - delta);

    e->ignore();
}
