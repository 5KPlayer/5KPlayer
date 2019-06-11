#include "hardwarewidget.h"

#include <QDebug>
#include <QLabel>
#include <QFile>
#include <QMovie>
#include <QFrame>
#include <QScrollArea>
#include <QAbstractAnimation>
#include <QPropertyAnimation>
#include <QJsonParseError>
#include <QJsonArray>
#include <QEasingCurve>
#include <QScrollBar>

#include "globalarg.h"
#include "dylabel.h"
#include "openfile.h"
#include "combobox.h"
#include "hardwareanalysis.h"
#include "hardwarestatistics.h"
#include "hardwareset.h"

#define SWITCH_DURATION 1000

HardwareWidget::HardwareWidget(QWidget *parent)
    : UpdateInterfaceWidget(parent)
{
    this->setMouseTracking(false);
    Init();
    initialize();
    checkHardware();
}

HardwareWidget::~HardwareWidget()
{
    if (m_frameState == FrameSetting) {
        m_lstCheckInfo = m_setting->checkInfos();
    }
    m_hardwareCheck.stopHardware();
    saveToGlobal(m_lstCheckInfo);
    saveToJson(m_lstCheckInfo);
}

void HardwareWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    m_analysis->updateUI(scaleRatio);
    m_statistics->updateUI(scaleRatio);
    m_setting->updateUI(scaleRatio);
    UpUI();
}

void HardwareWidget::Init()
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);
    _title      = new QLabel(this);
    _title->setPalette(p1);
    _title->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    _title->setText(Lge->getLangageValue("SetMain/hard"));
    _titleLine  = new QLabel(this);
    _titleLine2 = new QLabel(this);

    _titleLine->setStyleSheet("background-color: rgb(20, 20, 21);");
    _titleLine2->setStyleSheet("background-color: rgb(71, 71, 75);");

    _titleInfo  = new QLabel(this);
    _titleLine3 = new QLabel(this);
    _titleGif   = new QLabel(this);
    _titleMovie = new QMovie(":/res/loading.gif");
    _titleGif->setScaledContents(true);
    _titleGif->setMovie(_titleMovie);

    _titleGif->setAlignment(Qt::AlignCenter);
    _titleGif->setVisible(false);

    _titleInfo->setPalette(p1);
    _titleInfo->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _titleInfo->setText(Lang("HardCheck/analysisTitle"));
    _titleLine3->setStyleSheet("background-color: rgb(53, 53, 53)");


    _bottomWidget      = new QWidget(this);
    _reCheckHardBtn    = new DYLabel(_bottomWidget,true);
    _reCheckHardBtn->setText(Lang("HardCheck/recheck"));
    _reCheckHardBtn->setAlignment(Qt::AlignCenter);
    connect(_reCheckHardBtn,SIGNAL(clicked()),SLOT(reCheckHardware()));
    _nextBtn            = new DYLabel(_bottomWidget,true);
    _nextBtn->setBackClolr("rgb(0,140,220)","rgb(51,191,242)","rgb(0,175,239)");
    _nextBtn->setText(Lang("HardCheck/next"));
    _nextBtn->setAlignment(Qt::AlignCenter);
    _nextBtn->setVisible(false);

    _scaleRatio = Global->_fontRatio;
    _scanCount  = 0;

    connect(_nextBtn,SIGNAL(clicked()),SLOT(nextBtnClick()));
    connect(&m_hardwareCheck, SIGNAL(doShowInfo(QVariant)),this,SLOT(onUpdateHardwareInfo(QVariant)));

}

void HardwareWidget::UpUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12*_scaleRatio);
    font.setBold(true);
    _reCheckHardBtn->setFont(font);
    _nextBtn->setFont(font);

    font.setPixelSize(14*_scaleRatio);
    _title->setFont(font);
    font.setPixelSize(16*_scaleRatio);
    _titleInfo->setFont(font);

    double space     = 4*_scaleRatio;
    double height   = 20*_scaleRatio;
    int titleW  = _title->fontMetrics().width(_title->text());
    _title->setGeometry(space,       5*_scaleRatio,titleW,height);
    int lineW   = this->width()- (_title->x() + _title->width() + 28*_scaleRatio);
    _titleLine->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,15*_scaleRatio,lineW, 1*_scaleRatio);
    _titleLine2->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,16*_scaleRatio,lineW, 1*_scaleRatio);

    int titleLength = _titleInfo->fontMetrics().width(Lang("HardCheck/analysisTitle"));
    int titleX = (this->width() - titleLength) / 2;
    _titleInfo->setGeometry(titleX,24*_scaleRatio,titleLength,28*_scaleRatio);
    _titleMovie->setScaledSize(QSize(20*_scaleRatio,20*_scaleRatio));
    _titleGif->setGeometry(_titleInfo->geometry().right() + space,28*_scaleRatio,20*_scaleRatio,20*_scaleRatio);

    _titleLine3->setGeometry(space+2,60*_scaleRatio,this->width() - space*3,1*_scaleRatio);

    _bottomWidget->setGeometry(space,this->height() - 24*_scaleRatio,this->width(),20*_scaleRatio);
    _reCheckHardBtn->setGeometry(120*_scaleRatio,0,120*_scaleRatio,20*_scaleRatio);
    _nextBtn->setGeometry(360*_scaleRatio,0,120*_scaleRatio,20*_scaleRatio);

    int widgetH = 210 *_scaleRatio;
    m_mainArea->setGeometry(space,62*_scaleRatio,this->width() - space*2,widgetH);
    m_scrollW->setGeometry(0,0,m_mainArea->width(),widgetH*3);
    m_mainArea->setMinimumHeight(widgetH);
    m_mainArea->verticalScrollBar()->setMinimum(0);
    m_mainArea->verticalScrollBar()->setMaximum(widgetH);
    m_analysis->setGeometry(0,0,m_mainArea->width(),widgetH);
    m_statistics->setGeometry(0,widgetH,m_mainArea->width(),widgetH);
    m_setting->setGeometry(0,widgetH*2,m_mainArea->width(),widgetH);
    if (m_frameState == FrameAnalysis) {
        m_mainArea->verticalScrollBar()->setValue(0);
    } else if (m_frameState == FrameStatistics) {
        m_mainArea->verticalScrollBar()->setValue(widgetH);
    } else {
        m_mainArea->verticalScrollBar()->setValue(widgetH * 2);
    }

    _bottomWidget->raise();
}

void HardwareWidget::initialize()
{
    m_mainArea   = new QScrollArea(this);
    m_scrollW    = new QWidget(m_mainArea);
    m_mainArea->setWidget(m_scrollW);
    m_mainArea->viewport()->installEventFilter(this);
    m_mainArea->setFrameShape(QFrame::NoFrame);
    m_mainArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    m_mainArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_mainArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    m_analysis   = new HardwareAnalysis(m_scrollW);
    m_analysis->setVisible(false);
    m_statistics = new HardwareStatistics(m_scrollW);
    m_statistics->setVisible(false);
    m_setting = new HardwareSet(m_scrollW);
    m_setting->setVisible(false);

    m_frameState = FrameAnalysis;
    connect(m_analysis, SIGNAL(doShowCompleted()),SLOT(onShowCompleted()));
}

void HardwareWidget::initAnalysis()
{
    m_analysis->setVisible(true);
    m_analysis->clear();
    m_analysis->setCheckFinished(false);
}

void HardwareWidget::initStatistics()
{
    m_statistics->setVisible(true);
    m_statistics->clear();
    m_statistics->setStatisticsTable(m_lstCheckInfo);
}

void HardwareWidget::initSetting()
{
    m_setting->setVisible(true);
    m_setting->clear();

    /*----------------------*/
    bool davxInfoExits = false;
    for(int i=0;i<m_lstCheckInfo.count();i++) {
        if(m_lstCheckInfo.at(i).card.type == CardDXVA) {
            davxInfoExits = true;
            break;
        }
    }
    // 这里添加上dxva的信息和config文件中的选中信息
    if(!davxInfoExits) {
        int decInfo = Global->getHardwareDecInfo();
        int encInfo = Global->getHardwareEncInfo();

        QString decHEVC = "false";
        for(int i=0;i<m_lstCheckInfo.count();i++) {
            for(int j=0;j<m_lstCheckInfo.at(i).items.count();j++) {
                if(m_lstCheckInfo.at(i).items.at(j).codec == "HEVC") {
                    decHEVC = m_lstCheckInfo.at(i).items.at(j).dec;
                    break;
                }
            }
            if(decHEVC == "true") {
                break;
            }
        }
        HardwareCheckInfo info;
        info.card.type = CardDXVA;
        info.card.name = "Mirosoft DXVA";
        HardwareCheckItem h264Item;
        h264Item.codec = "H264";
        h264Item.dec   = "true";
        HardwareCheckItem hevcItem;
        hevcItem.codec = "HEVC";
        hevcItem.dec   = "true";
        info.items.append(h264Item);
        info.items.append(hevcItem);
        m_lstCheckInfo.append(info);
        for(int i=0;i<m_lstCheckInfo.count();i++) {
            CardInfo info;
            info.name   = m_lstCheckInfo.at(i).card.name;
            info.pro    = m_lstCheckInfo.at(i).card.pro;
            info.version= m_lstCheckInfo.at(i).card.version;
            switch (m_lstCheckInfo.at(i).card.type) {
            case CardIntel:
                info.type   = CardIntel;
                info.decode_h264_sel = decInfo & 0x04;
                info.decode_hevc_sel = decInfo & 0x08;
                info.encode_h264_sel = encInfo & 0x04;
                info.encode_hevc_sel = encInfo & 0x08;
                break;
            case CardNvidia:
                info.type   = CardNvidia;
                info.decode_h264_sel = decInfo & 0x10;
                info.decode_hevc_sel = decInfo & 0x20;
                info.encode_h264_sel = encInfo & 0x10;
                info.encode_hevc_sel = encInfo & 0x20;
                break;
            case CardDXVA:
                info.type   = CardDXVA;
                info.decode_h264_sel = decInfo & 0x01;
                info.decode_hevc_sel = decInfo & 0x02;
                info.encode_h264_sel = encInfo & 0x01;
                info.encode_hevc_sel = encInfo & 0x02;
                break;
            case CardAMD:
                info.type   = CardAMD;
                info.decode_h264_sel = decInfo & 0x40;
                info.decode_hevc_sel = decInfo & 0x80;
                info.encode_h264_sel = encInfo & 0x40;
                info.encode_hevc_sel = encInfo & 0x80;
            default:
                break;
            }
            HardwareCheckInfo checkInfo;
            checkInfo.card  = info;
            checkInfo.items = m_lstCheckInfo.at(i).items;
            m_lstCheckInfo.replace(i,checkInfo);
        }
    }
    /*------------------------*/

    m_setting->setSettingTable(m_lstCheckInfo);
}

bool HardwareWidget::parseCardInfo(const QString &data)
{
    QStringList items = data.split("\r\n", QString::SkipEmptyParts);

    for (int i = 0; i < items.count(); ++i) {
        QStringList panels = items.at(i).split("\t", QString::SkipEmptyParts);
        if (panels.count() < 3) {
            continue;
        }
        CardInfo info;
        for (int j = 0; j < panels.count(); ++j) {
            QStringList lst = panels.at(j).split(":", QString::SkipEmptyParts);
            if (lst.count() != 2)
                continue;
            if (lst.at(0) == "pro")
                info.pro = lst.at(1);
            else if (lst.at(0) == "name")
                info.name = lst.at(1);
            else if (lst.at(0) == "ver")
                info.version = lst.at(1);
        }
        info.type = cardType(info.pro.left(1));
        if (info.type == CardNone)
            continue;
        HardwareCheckInfo * checkinfo = containsType(info.type);
        if (!checkinfo)
            continue;
        checkinfo->card = info;

        if (m_analysis)
            m_analysis->addCardInfo(info);
    }
    return true;
}

bool HardwareWidget::parseCheckInfo(const QString &data)
{
    CardType type = cardType(data.left(1));
    if (type == CardNone)
        return false;

    QString tempData = data;
    QString result = tempData.remove(0, 1);

    if (result.startsWith("Error", Qt::CaseInsensitive))
        return false;
    QStringList items = result.split("\r\n", QString::SkipEmptyParts);

    HardwareCheckInfo * checkinfo = containsType(type);
    if (!checkinfo)
        return false;
    for (int i = 0; i < items.count(); ++i) {
        QStringList panels = items.at(i).split("\t", QString::SkipEmptyParts);
        if (panels.count() < 7)
            continue;
        HardwareCheckItem item;
        item.type = type;
        for (int j = 0; j < panels.count(); ++j) {
            QStringList lst = panels.at(j).split(":", QString::SkipEmptyParts);
            if (lst.count() != 2)
                continue;
            if (QString::compare(lst.at(0), "codec", Qt::CaseInsensitive) == 0) {
                item.codec = lst.at(1);
            } else if (QString::compare(lst.at(0), "profile", Qt::CaseInsensitive) == 0) {
                item.profile = lst.at(1);
            } else if (QString::compare(lst.at(0), "level", Qt::CaseInsensitive) == 0) {
                item.level = lst.at(1);
            } else if (QString::compare(lst.at(0), "Res", Qt::CaseInsensitive) == 0) {
                item.res = lst.at(1);
            } else if (QString::compare(lst.at(0), "Dec", Qt::CaseInsensitive) == 0) {
                item.dec = lst.at(1);
            } else if (QString::compare(lst.at(0), "Enc", Qt::CaseInsensitive) == 0) {
                item.enc = lst.at(1);
            } else if (QString::compare(lst.at(0), "Dev", Qt::CaseInsensitive) == 0) {
                item.device = lst.at(1);
            }
        }
        checkinfo->items.append(item);

        if (m_analysis)
            m_analysis->showOneItemInfo(item);
    }

    return true;
}

CardType HardwareWidget::cardType(const QString &data)
{
    if (QString::compare(data, "I", Qt::CaseInsensitive) == 0)
        return CardIntel;
    if (QString::compare(data, "N", Qt::CaseInsensitive) == 0)
        return CardNvidia;
    if (QString::compare(data, "A", Qt::CaseInsensitive) == 0)
        return CardAMD;
    return CardNone;
}

HardwareCheckInfo *HardwareWidget::containsType(CardType type)
{
    for (int i = 0; i < m_lstCheckInfo.count(); ++i) {
        if (m_lstCheckInfo[i].card.type == type) {
            return &m_lstCheckInfo[i];
        }
    }
    HardwareCheckInfo info;
    info.card.type = type;
    m_lstCheckInfo.append(info);

    return &m_lstCheckInfo.last();
}

void HardwareWidget::checkCompleted()
{
    _titleMovie->stop();
    _titleGif->setVisible(false);
    _reCheckHardBtn->setEnabled(true);
    _nextBtn->setVisible(true);
    emit upCheckState(false);
}

void HardwareWidget::startCheck()
{
    _titleMovie->start();
    _titleGif->setVisible(true);
    _reCheckHardBtn->setEnabled(false);
    _nextBtn->setVisible(false);
    emit upCheckState(true);
}

QList<HardwareCheckInfo> HardwareWidget::fromJson(const QByteArray &data)
{
    QJsonParseError error;
    QList<HardwareCheckInfo> infos;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError)
        return infos;
    if (!doc.isArray())
        return infos;
    QJsonArray arrayList = doc.array();
    for (int i = 0; i < arrayList.count(); ++i) {
        HardwareCheckInfo info;
        if (!arrayList.at(i).isObject())
            continue;
        QJsonObject object = arrayList.at(i).toObject();
        if (object.contains("type")) {
            info.card.type = (CardType)(object.value("type").toInt());
        }
        if (object.contains("pro")) {
            info.card.pro = object.value("pro").toString();
        }
        if (object.contains("name")) {
            info.card.name = object.value("name").toString();
        }
        if (object.contains("version")) {
            info.card.version = object.value("version").toString();
        }
        if (object.contains("enc_h264")) {
            info.card.encode_h264_sel = object.value("enc_h264").toBool();
        }
        if (object.contains("enc_hevc")) {
            info.card.encode_hevc_sel = object.value("enc_hevc").toBool();
        }
        if (object.contains("dec_h264")) {
            info.card.decode_h264_sel = object.value("dec_h264").toBool();
        }
        if (object.contains("dec_hevc")) {
            info.card.decode_hevc_sel = object.value("dec_hevc").toBool();
        }
        if (object.contains("items")) {
            if (!object.value("items").isArray())
                continue;
            QJsonArray items = object.value("items").toArray();
            QList<HardwareCheckItem> checkItemsList;
            for (int j = 0; j < items.count(); ++j) {
                QJsonObject item = items.at(j).toObject();
                HardwareCheckItem checkItem;
                if (item.contains("codec")) {
                    checkItem.codec = item.value("codec").toString();
                }
                if (item.contains("profile")) {
                    checkItem.profile = item.value("profile").toString();
                }
                if (item.contains("level")) {
                    checkItem.level = item.value("level").toString();
                }
                if (item.contains("Res")) {
                    checkItem.res = item.value("Res").toString();
                }
                if (item.contains("Enc")) {
                    checkItem.enc = item.value("Enc").toString();
                }
                if (item.contains("Dec")) {
                    checkItem.dec = item.value("Dec").toString();
                }
                if (item.contains("Dev")) {
                    checkItem.device = item.value("Dev").toString();
                }
                checkItemsList.append(checkItem);
            }
            info.items = checkItemsList;
        }
        infos.append(info);
    }

    return infos;
}

bool HardwareWidget::saveToJson(const QList<HardwareCheckInfo> &checkInfo)
{
    QString hardwareInfo;
    QJsonArray arrayList;

    for (int i = 0; i < checkInfo.count(); ++i) {
        const HardwareCheckInfo &info = checkInfo.at(i);
        QJsonObject object;
        object.insert("type", info.card.type);
        object.insert("pro", info.card.pro);
        object.insert("name", info.card.name);
        object.insert("version", info.card.version);
        object.insert("enc_h264", info.card.encode_h264_sel);
        object.insert("enc_hevc", info.card.encode_hevc_sel);
        object.insert("dec_h264", info.card.decode_h264_sel);
        object.insert("dec_hevc", info.card.decode_hevc_sel);
        QJsonArray arrayItem;
        for (int j = 0; j < info.items.count(); ++j) {
            QJsonObject item;
            item.insert("codec", info.items.at(j).codec);
            item.insert("profile", info.items.at(j).profile);
            item.insert("level", info.items.at(j).level);
            item.insert("Res", info.items.at(j).res);
            item.insert("Enc", info.items.at(j).enc);
            item.insert("Dec", info.items.at(j).dec);
            item.insert("Dev", info.items.at(j).device);
            arrayItem.append(item);
        }
        object.insert("items", arrayItem);
        arrayList.append(object);
    }
    if (arrayList.isEmpty()) {
        hardwareInfo = "error";
//        return false;
    } else {
        QJsonDocument doc(arrayList);
        hardwareInfo = doc.toJson();
    }

    /*Save json to file*/
    QFile file(Global->getHardwareJsonPath());
    if (file.exists()) {
        file.remove();
    }
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return false;
    }
    file.write(hardwareInfo.toUtf8());
    file.close();

    return true;
}

void HardwareWidget::saveToGlobal(const QList<HardwareCheckInfo> &checkInfo)
{
    // 保存选择的信息
    int decInfo = 0;
    int encInfo = 0;
    for(int i=0;i<checkInfo.count();i++) {
        if(checkInfo.at(i).card.type == CardDXVA) {
            if(checkInfo.at(i).card.decode_h264_sel) {
                decInfo |= 0x01;
            }
            if(checkInfo.at(i).card.decode_hevc_sel) {
                decInfo |= 0x02;
            }
            if(checkInfo.at(i).card.encode_h264_sel) {
                encInfo |= 0x01;
            }
            if(checkInfo.at(i).card.encode_hevc_sel) {
                encInfo |= 0x02;
            }
        } else if(checkInfo.at(i).card.type == CardIntel) {
            if(checkInfo.at(i).card.decode_h264_sel) {
                decInfo |= 0x04;
            }
            if(checkInfo.at(i).card.decode_hevc_sel) {
                decInfo |= 0x08;
            }
            if(checkInfo.at(i).card.encode_h264_sel) {
                encInfo |= 0x04;
            }
            if(checkInfo.at(i).card.encode_hevc_sel) {
                encInfo |= 0x08;
            }
        } else if(checkInfo.at(i).card.type == CardNvidia) {
            if(checkInfo.at(i).card.decode_h264_sel) {
                decInfo |= 0x10;
            }
            if(checkInfo.at(i).card.decode_hevc_sel) {
                decInfo |= 0x20;
            }
            if(checkInfo.at(i).card.encode_h264_sel) {
                encInfo |= 0x10;
            }
            if(checkInfo.at(i).card.encode_hevc_sel) {
                encInfo |= 0x20;
            }
        } else if(checkInfo.at(i).card.type == CardAMD) {
            if(checkInfo.at(i).card.decode_h264_sel) {
                decInfo |= 0x40;
            }
            if(checkInfo.at(i).card.decode_hevc_sel) {
                decInfo |= 0x80;
            }
            if(checkInfo.at(i).card.encode_h264_sel) {
                encInfo |= 0x40;
            }
            if(checkInfo.at(i).card.encode_hevc_sel) {
                encInfo |= 0x80;
            }
        }
    }

    Global->setHardwareDecInfo(decInfo);
    Global->setHardwareEncInfo(encInfo);
}

void HardwareWidget::resizeEvent(QResizeEvent *)
{
    UpUI();
}

void HardwareWidget::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

bool HardwareWidget::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == m_mainArea->viewport() && e->type() == QEvent::Wheel) {
        e->ignore();
        return true;
    }
    return QWidget::eventFilter(obj,e);
}

void HardwareWidget::onUpdateHardwareInfo(const QVariant &info)
{
    if (info.type() == QVariant::ByteArray) {
        parseCardInfo(info.toString());
    } else if (info.type() == QVariant::String) {
        parseCheckInfo(info.toString());
    } else if (info.type() == QVariant::Bool) {
        if (info.toBool()) {
            /*Check Success*/
            bool empty = true;
            for (int i = 0; i < m_lstCheckInfo.count(); ++i) {
                if (!m_lstCheckInfo.at(i).items.isEmpty()) {
                    empty = false;
                    break;
                }
            }
            if (empty) {
                qDebug("No supportable hardware detected on this machine!");
//                MsgDialog::exec(tr("No supportable hardware detected on this machine!"));
                checkCompleted();
            }
        } else {
            /*Check Error*/
            qDebug("check Error");
//            MsgDialog::exec(tr("Failed to detect hardware information!"));
            checkCompleted();
        }
        if (m_analysis)
            m_analysis->setCheckFinished(true);
    }
}

void HardwareWidget::checkHardware()
{
    m_lstCheckInfo.clear();
    // 这里检查是否有硬件检测信息保存的文件
    QFile file(Global->getHardwareJsonPath());
    if (file.exists() && file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        m_lstCheckInfo = fromJson(file.readAll());
        initSetting();
        showFrame(FrameSetting);
        return;
    }
    initAnalysis();
    showFrame(FrameAnalysis);
    startCheck();
    m_hardwareCheck.checkHardware();
}

void HardwareWidget::reCheckHardware()
{
    if (m_frameState == FrameSetting) {
        m_lstCheckInfo = m_setting->checkInfos();
    }
    saveToGlobal(m_lstCheckInfo);

    m_lstCheckInfo.clear();
    initAnalysis();
    showFrame(FrameAnalysis);
    startCheck();
    m_hardwareCheck.checkHardware();
}

void HardwareWidget::nextBtnClick()
{
    initSetting();
    showFrame(FrameSetting);
    _nextBtn->setVisible(false);
}

void HardwareWidget::onShowCompleted()
{
    initStatistics();
    showFrame(FrameStatistics);
    checkCompleted();
}

void HardwareWidget::showFrame(FrameState state)
{
    int stopValue;

    if (state == m_frameState)
        return;
    int diff = qAbs((int)m_frameState - (int)state);
    m_frameState = state;
    if (state == FrameAnalysis) {
        stopValue = 0;
    } else if (state == FrameStatistics) {
        stopValue = 210*_scaleRatio;
    } else {
        stopValue = 210*_scaleRatio * 2;
    }
    QPropertyAnimation *aniScroll = new QPropertyAnimation(this);
    aniScroll->setTargetObject(m_mainArea->verticalScrollBar());
    aniScroll->setPropertyName("value");
    aniScroll->setDuration(diff > 1 ? SWITCH_DURATION : 700);
    aniScroll->setEndValue(stopValue);
    aniScroll->setEasingCurve(QEasingCurve::OutCubic);
    aniScroll->start(QAbstractAnimation::DeleteWhenStopped);
}
