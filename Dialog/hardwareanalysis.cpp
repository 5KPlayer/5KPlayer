#include "hardwareanalysis.h"

#include "dypixlabel.h"
#include "globalarg.h"


#include <QScrollArea>
#include <QScrollBar>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QAbstractAnimation>
#include <QPropertyAnimation>
#include <QLabel>
#include <QMovie>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>

#include <QDebug>

class OpacityWidget: public QWidget
{
public:
    OpacityWidget(QWidget* parent = 0):
        QWidget(parent),
        m_opacityEffect(NULL),
        m_opacity(0.0)
    {
        m_opacityEffect = new QGraphicsOpacityEffect(this);
        m_opacityEffect->setOpacity(m_opacity);
        this->setGraphicsEffect(m_opacityEffect);
    }

    QGraphicsOpacityEffect *effect() {
        return m_opacityEffect;
    }

    void setOpacity(qreal opacity)
    {
        m_opacity = opacity;
        m_opacityEffect->setOpacity(opacity);
    }

private:
    QGraphicsOpacityEffect *m_opacityEffect;
    qreal m_opacity;
};

CheckItem::CheckItem()
{
   widget = NULL;
   isShow = false;
   isTitle = false;
   type = CardNone;
}

void CheckItem::clear()
{
   if (widget) {
       delete widget;
       widget = NULL;
   }
   isShow = false;
   isTitle = false;
   type = CardNone;
}


static void buildLabel(QLabel *&label,
                       const QString &objectName,
                       QWidget *parent,
                       const QString &color = "#ffffff",
                       int fontSize = 13,
                       Qt::Alignment align = Qt::AlignCenter)
{
    label = new QLabel(parent);
    label->setObjectName(objectName);
    label->setProperty("baseFontSize", fontSize);
    label->setAlignment(align);
    label->setStyleSheet(QString("QLabel{color:%1;"
                                 "background-color: transparent;"
                                 "font-family:\"Tahoma\";}").arg(color));
}

static void buildButton(QPushButton *&btn,
                       const QString &objectName,
                       QWidget *parent,
                       const QString &color = "#ffffff",
                       int fontSize = 13)
{
    btn = new QPushButton(parent);
    btn->setObjectName(objectName);
    btn->setText("");
    btn->setProperty("baseFontSize", fontSize);
    btn->setProperty("baseIconSize", QVariant(QSize(14, 11)));
    btn->setStyleSheet(QString("QPushButton{"
                               "background-color: transparent;"
                               "color:%1;font-family:\"Tahoma\";"
                               "}").arg(color));
}

class TitleWidget: public OpacityWidget
{
public:
    TitleWidget(QWidget *parent = NULL):
        OpacityWidget(parent)
    {
        mWidgetMain = new QWidget(this);
        QHBoxLayout *layoutMain = new QHBoxLayout(this);
        layoutMain->addWidget(mWidgetMain);
        layoutMain->setMargin(0);
        layoutMain->setSpacing(0);
        setLayout(layoutMain);

        buildLabel(mLabTitle, "title", mWidgetMain, "#434652", 14);

        QHBoxLayout *layout = new QHBoxLayout(mWidgetMain);
        layout->addWidget(mLabTitle);
        layout->setMargin(0);
        layout->setSpacing(0);
        mWidgetMain->setLayout(layout);

        setBaseSize(600, 26);
        setMinimumSize(600, 26);
        mWidgetMain->setObjectName("mWidgetMain");
        mWidgetMain->setStyleSheet("QWidget#mWidgetMain{"
                                   "border-image: url(:/res/hardware/title.png);"
                                   "font-family:\"Tahoma\";"
                                   "}");
    }

    void setText(const QString &text)
    {
        mLabTitle->setText(text);
    }

private:
    QWidget *mWidgetMain;
    QLabel *mLabTitle;
};


class ItemWidget: public OpacityWidget
{
public:
    ItemWidget(QWidget *parent = NULL):
        OpacityWidget(parent)
    {
        mWidgetMain = new QWidget(this);
        QHBoxLayout *layoutMain = new QHBoxLayout(this);
        layoutMain->addWidget(mWidgetMain);
        layoutMain->setMargin(0);
        layoutMain->setSpacing(0);
        setLayout(layoutMain);

        buildLabel(mLabCodec, "codec", mWidgetMain,  "#626873", 13);
        buildLabel(mLabProfile, "profile", mWidgetMain,  "#626873", 13);
        buildLabel(mLabLevel, "level", mWidgetMain,  "#626873", 13);
        buildLabel(mLabRes, "Res", mWidgetMain,  "#626873", 13);
        buildButton(mBtnEnc, "Enc", mWidgetMain,  "#626873", 13);
        buildButton(mBtnDec, "Dec", mWidgetMain,  "#626873", 13);

        mControls << mLabCodec << mLabProfile << mLabLevel << mLabRes << mBtnEnc << mBtnDec;
        int baseWidth = 0;
        QHBoxLayout *layout = new QHBoxLayout(mWidgetMain);
        foreach (QWidget *control, mControls) {
            layout->addWidget(control);
            baseWidth += control->minimumWidth();
        }
        layout->setMargin(0);
        layout->setSpacing(0);
        mWidgetMain->setLayout(layout);

        baseWidth = 600;
        setMinimumSize(baseWidth, 26);
        setBaseSize(baseWidth, 26);
        mWidgetMain->setObjectName("mWidgetMain");
        mWidgetMain->setStyleSheet("QWidget#mWidgetMain{"
                                   "border-color: rgb(27,27,28);"
                                   "background-color: rgb(27,27,28);"
                                   "border-radius: 6px;"
                                   "font-family:\"Tahoma\";"
                                   "}");
    }

    void setInfo(const HardwareCheckItem &item)
    {
        mLabCodec->setText(item.codec);
        mLabProfile->setText(item.profile);
        mLabLevel->setText(item.level);
        mLabRes->setText(item.res);
        mBtnDec->setIcon(QString::compare(item.dec, "true") == 0 ? QIcon(":/res/png/hwRight.png")
                                                                 : QIcon(":/res/png/hwError.png"));
        mBtnEnc->setIcon(QString::compare(item.enc, "true") == 0 ? QIcon(":/res/png/hwRight.png")
                                                                 : QIcon(":/res/png/hwError.png"));
    }

    void resizeEvent(QResizeEvent *)
    {

    }

private:
    QWidget *mWidgetMain;
    QVector<QWidget *> mControls;
    QLabel *mLabCodec, *mLabProfile, *mLabLevel, *mLabRes;
    QPushButton *mBtnDec, *mBtnEnc;
};

HardwareAnalysis::HardwareAnalysis(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    _scaleRatio = Global->_screenRatio;
    m_timer     = NULL;

    _analysisArea   = new QScrollArea(this);
    _analysisWidget = new QWidget(_analysisArea);
    _analysisArea->setWidget(_analysisWidget);
    //_analysisArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    _analysisArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _analysisArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _analysisArea->setFrameShape(QFrame::NoFrame);

    initPixLabel(&_codecBtn,     ":/res/png/Codec",     Lang("HardCheck/codec"));
    initPixLabel(&_profileBtn,   ":/res/png/Profile",   Lang("HardCheck/profile"));
    initPixLabel(&_levelBtn,     ":/res/png/Level",     Lang("HardCheck/level"));
    initPixLabel(&_resolutionBtn,":/res/png/Dimensions",Lang("HardCheck/maxres"));
    initPixLabel(&_isEncodecBtn, ":/res/png/Encode",    Lang("HardCheck/encoder"));
    initPixLabel(&_isDecodecBtn, ":/res/png/Decode",    Lang("HardCheck/decoder"));
}

void HardwareAnalysis::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _codecBtn->updateUI(scaleRatio);
    _profileBtn->updateUI(scaleRatio);
    _levelBtn->updateUI(scaleRatio);
    _resolutionBtn->updateUI(scaleRatio);
    _isEncodecBtn->updateUI(scaleRatio);
    _isDecodecBtn->updateUI(scaleRatio);
}

void HardwareAnalysis::initPixLabel(DYPixLabel **label, QString res, QString text)
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setBold(true);
    font.setFamily(Global->getFontFamily());
    (*label) = new DYPixLabel(res,text,this);
    (*label)->setFont(font);
    int width = (*label)->fontMetrics().width(text) /  _scaleRatio;
    (*label)->initSize(width + 28,20);
}

void HardwareAnalysis::setCheckInfo(QList<HardwareCheckInfo> *lstCheckInfo)
{
    m_lstCheckInfo = lstCheckInfo;
}

void HardwareAnalysis::setCheckFinished(bool finished)
{
    m_checkFinished = finished;
}

void HardwareAnalysis::clear()
{
    _analysisWidget->setFixedHeight(0);
    for (int i = 0; i < m_lstItems.count(); ++i) {
        if (m_lstItems[i].isTitle)
            continue;
        m_lstItems[i].clear();
    }
    m_itemIntel.clear();
    m_itemNvidia.clear();
    m_itemAMD.clear();
    m_lstItems.clear();
}

void HardwareAnalysis::addCardInfo(const CardInfo &info)
{
    CheckItem *item;
    if (info.type == CardIntel) {
        item = &m_itemIntel;
    } else if (info.type == CardNvidia) {
        item = &m_itemNvidia;
    } else if (info.type == CardAMD) {
        item = &m_itemAMD;
    }
    if (!item)
        return;
    item->widget = new TitleWidget(_analysisWidget);
    TitleWidget *widget = static_cast<TitleWidget *>(item->widget);
    widget->setText(info.name);

    showItems();
}

void HardwareAnalysis::showItems()
{
    if (m_timer)
        return;
    m_index = 0;
    m_currentY = 0;
    _analysisWidget->setFixedHeight(0);
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &HardwareAnalysis::showOneItem);
    connect(this, &HardwareAnalysis::doShowCompleted, [this](){
        if (m_timer) {
            m_timer->stop();
            delete m_timer;
            m_timer = NULL;
        }
    });
    m_timer->start(200);
}

void HardwareAnalysis::showOneItemInfo(const HardwareCheckItem &item)
{
    CheckItem *itemTitle;
    if (item.type == CardIntel) {
        itemTitle = &m_itemIntel;
    } else if (item.type == CardNvidia) {
        itemTitle = &m_itemNvidia;
    } else if (item.type == CardAMD) {
        itemTitle = &m_itemAMD;
    }
    if (itemTitle) {
        if (!itemTitle->isShow) {
            CheckItem item = *itemTitle;
            item.isTitle = true;
            m_lstItems.append(item);
            itemTitle->isShow = true;
        }
    }
    CheckItem chechItem;
    chechItem.widget = new ItemWidget(_analysisWidget);
    ItemWidget *widget = dynamic_cast<ItemWidget *>(chechItem.widget);
    widget->setInfo(item);
    m_lstItems.append(chechItem);
    showItems();
}

void HardwareAnalysis::showOneItem()
{
    if (m_index >= m_lstItems.count() || m_index < 0) {
        if (m_checkFinished) {
            emit doShowCompleted();
            m_index = 0;
        }
        return;
    }
    const int spacing = 3*_scaleRatio;//RATIO_FIT(3);
    CheckItem item = m_lstItems.at(m_index);
    item.geomety = QRect(0, m_currentY + spacing,
                         item.widget->baseSize().width()*_scaleRatio,
                         item.widget->baseSize().height()*_scaleRatio
                         /*RATIO_FIT(item.widget->baseSize().width()),
                         RATIO_FIT(item.widget->baseSize().height())*/);
    _analysisWidget->setMinimumHeight((m_index + 1) * (36*_scaleRatio));
    showItem(item);
    ++m_index;
    m_currentY += (item.geomety.height() + spacing);
}

void HardwareAnalysis::showItem(const CheckItem &item, bool needAnimation)
{
    if (needAnimation) {
        QPropertyAnimation *aniOpacity, *aniMove, *aniScroll;
        QParallelAnimationGroup *aniGroup;
        const int duration = 350;

        item.widget->show();
        aniMove = new QPropertyAnimation(item.widget, "geometry", this);
        aniMove->setDuration(duration);
        aniMove->setStartValue(QRect(item.geomety.x() + 300, item.geomety.y(), item.geomety.width(), item.geomety.height()));
        aniMove->setEndValue(item.geomety);
        aniMove->setEasingCurve(QEasingCurve::OutCubic);

        aniOpacity = new QPropertyAnimation(this);
        aniOpacity->setTargetObject(item.widget->effect());
        aniOpacity->setPropertyName("opacity");
        aniOpacity->setDuration(duration);
        aniOpacity->setStartValue(0.0);
        aniOpacity->setEndValue(1.0);

        aniScroll = new QPropertyAnimation(this);
        aniScroll->setTargetObject(_analysisArea->verticalScrollBar());
        aniScroll->setPropertyName("value");
        aniScroll->setDuration(duration);
        aniScroll->setStartValue(_analysisArea->verticalScrollBar()->maximum() - 36/*RATIO_FIT(50)*/);
        aniScroll->setEndValue(_analysisArea->verticalScrollBar()->maximum());
        aniScroll->setEasingCurve(QEasingCurve::OutCubic);

        aniGroup = new QParallelAnimationGroup(this);
        aniGroup->addAnimation(aniMove);
        aniGroup->addAnimation(aniOpacity);
        aniGroup->start(QAbstractAnimation::DeleteWhenStopped);
        aniScroll->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        item.widget->setOpacity(1.0);
        item.widget->setGeometry(item.geomety);
        item.widget->show();
        _analysisArea->verticalScrollBar()->setValue(_analysisArea->verticalScrollBar()->maximum());
    }
}

void HardwareAnalysis::resizeTable()
{
    m_currentY = 0;
    for (int i = 0; i < m_lstItems.count(); ++i) {
        CheckItem &item = m_lstItems[i];
        const int spacing = 3*_scaleRatio;//RATIO_FIT(3);
        item.geomety = QRect(0, m_currentY + spacing,
                             item.widget->baseSize().width()*_scaleRatio,
                             item.widget->baseSize().height()*_scaleRatio);
        item.widget->setGeometry(item.geomety);
        _analysisWidget->setMinimumHeight((i + 1) * 36*_scaleRatio);
        m_currentY += (item.geomety.height() + spacing);
    }
}

void HardwareAnalysis::resizeEvent(QResizeEvent *)
{
    QFont font;
    font.setPixelSize(12*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    font.setBold(true);
    _codecBtn->setFont(font);
    _profileBtn->setFont(font);
    _levelBtn->setFont(font);
    _resolutionBtn->setFont(font);
    _isEncodecBtn->setFont(font);
    _isDecodecBtn->setFont(font);

    int space = 4 * _scaleRatio;


    _codecBtn->move(28*_scaleRatio,4*_scaleRatio);
    _profileBtn->move(120*_scaleRatio,4*_scaleRatio);
    _levelBtn->move(218*_scaleRatio,4*_scaleRatio);
    _resolutionBtn->move(290*_scaleRatio,4*_scaleRatio);
    _isEncodecBtn->move(428*_scaleRatio,4*_scaleRatio);
    _isDecodecBtn->move(520*_scaleRatio,4*_scaleRatio);
    _analysisArea->setGeometry(space,28*_scaleRatio,
                                 this->width() - space*2, this->height() - 32*_scaleRatio);
    _analysisWidget->setGeometry(0,0,_analysisArea->width(),_analysisArea->height());
    resizeTable();

}
