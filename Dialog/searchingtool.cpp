#include <QListWidget>
#include "searchingtool.h"
#include "globalarg.h"
#include <QDebug>
#include <QApplication>
#include <QScrollBar>
SearchingTool::SearchingTool(QWidget *parent) :
    QWidget(parent)
  , m_lstItems(NULL)
  , m_maxNum(10)
{
    _scaleRatio = Global->_screenRatio;
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setMouseTracking(true);
    qApp->installEventFilter(this);
}

void SearchingTool::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
}

void SearchingTool::showUp(QWidget *focusWidget, QStringList strs)
{
    if(strs.isEmpty())
    {
        this->hide();
        return;
    }
    m_focusWidget = focusWidget;
    m_strs = strs;
    int itemWidth = focusWidget->width();
    int itemHeight = focusWidget->height();
    QPoint globalPos = focusWidget->mapToGlobal(QPoint(0,0));
    if(!m_lstItems)
    {
        m_lstItems = new QListWidget(this);
        m_lstItems->move(0,0);
        m_lstItems->setMouseTracking(true);
        m_lstItems->setFocusPolicy(Qt::StrongFocus);
        m_lstItems->installEventFilter(this);
        m_lstItems->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        connect(m_lstItems, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClicked(QListWidgetItem*)));
    }
    m_lstItems->clear();
    m_lstItems->installEventFilter(this);
    m_lstItems->setStyleSheet(QString("QListWidget {"
                                      "border: 0px;"
                                      "background-color: white;"
                                      "color: #656770;"
                                      "outline:0px;"
                                      "}"
                                      "QListWidget::item {"
                                      "color: #656770;"
                                      "    background-color: white;"
                                      "    padding-left: %1px;"
                                      "}"
                                      "QListWidget::item:selected {"
                                      "                      color: #656770;"
                                      "background-color: white;"
                                      "padding-left: %1px;"
                                      "icon:none;"
                                      "}"
                                      "QListWidget::item:hover {"
                                      "color: white;"
                                      "background-color: #3399FF;"
                                      "padding-left: %1px;}")
                              .arg(6*_scaleRatio));

    m_lstItems->verticalScrollBar()->resize(7*_scaleRatio, this->height());
    m_lstItems->verticalScrollBar()->setStyleSheet(QString(// 设置垂直滚动条基本样式
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
                                                    .arg(6  * _scaleRatio));
    m_lstItems->setSelectionMode(QListWidget::NoSelection);
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    for (int i = 0; i < strs.size(); ++i)
    {
        QListWidgetItem *item = new QListWidgetItem(m_lstItems);
        item->setText(strs[i]);
        item->setToolTip(strs[i]);
        item->setFont(font);
        item->setSizeHint(QSize(itemWidth, itemHeight));
    }
    m_lstItems->resize(itemWidth, qMin(strs.size(), m_maxNum) * itemHeight);
    QPoint pos = globalPos + QPoint(0, itemHeight - 1);
    this->resize(itemWidth , qMin(strs.size(), m_maxNum) * itemHeight + 6*_scaleRatio);
    this->move(pos);
    this->show();
}

void SearchingTool::setWidget(QWidget *focusWidget, QStringList strs)
{
    m_focusWidget = focusWidget;
    m_strs = strs;
    this->hide();
}

bool SearchingTool::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == QEvent::MouseButtonPress)
    {
        if(obj == m_focusWidget || obj == m_lstItems)
        {
            this->showUp(m_focusWidget, m_strs);
        }
        else
        {
            this->hide();
        }
    }
    if(obj == m_focusWidget && e->type() == QEvent::FocusOut)
    {
        this->hide();
    }

    return false;
}

void SearchingTool::onItemClicked(QListWidgetItem *item)
{
    emit doClicked(item->text());
}
