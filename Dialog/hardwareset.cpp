#include "hardwareset.h"

#include "dycheckbox.h"
#include "language.h"
#include "globalarg.h"
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QHeaderView>
#include <QLabel>

static QWidget *buildItem(const QString &text = QString(),
                         int baseFontSize = 14,
                         const QString &color = "#D8D8D8",
                         bool bold = true,
                         const QString &icon = QString())
{
    if (icon.isEmpty()) {
        QLabel *lab = new QLabel;
        lab->setProperty("baseFontSize", baseFontSize);
        lab->setStyleSheet(QString("background-color:#202021; color:%1; font-weight: %2").arg(color).arg(bold ? "bold" : "normal"));
        lab->setAlignment(Qt::AlignCenter);
        lab->setText(text);
        return lab;
    } else {
        QPushButton *btn = new QPushButton;
        btn->setProperty("baseFontSize", baseFontSize);
        btn->setStyleSheet(QString("background-color:#202021; border: 0px; color:%1; font-weight: %2").arg(color).arg(bold ? "bold" : "normal"));
        btn->setText(text);
        btn->setProperty("baseIconSize", QVariant(QSize(42*Global->_screenRatio, 28*Global->_screenRatio)));
        btn->setIconSize((QSize(42*Global->_screenRatio, 28*Global->_screenRatio)));
        btn->setIcon(QIcon(icon));
        return btn;
    }
}

HardwareSetSelect::HardwareSetSelect(QWidget *parent, CodecType type, HardwareCheckInfo *info):
    UpdateInterfaceWidget(parent),
    m_checkInfo(info),
    m_type(type)
{
    _mainWidget = new QWidget(this);
    _enCoder    = new DYCheckBox(_mainWidget,Lang("HardCheck/encoder"));
    _deCoder    = new DYCheckBox(_mainWidget,Lang("HardCheck/decoder"));
    _scaleRatio = Global->_screenRatio;

    connect(_enCoder,SIGNAL(checkChange(bool)),SLOT(onEncodecClick(bool)));
    connect(_deCoder,SIGNAL(checkChange(bool)),SLOT(onDecodecClick(bool)));
    setSelectInfo();
}

HardwareSetSelect::~HardwareSetSelect()
{

}

void HardwareSetSelect::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _enCoder->updateUI(scaleRatio);
    _deCoder->updateUI(scaleRatio);
    upUI();
}

void HardwareSetSelect::setSelectInfo()
{
    if (!m_checkInfo) {
        _enCoder->setCheckEnable(false);
        _deCoder->setCheckEnable(false);
        return;
    }

    _enCoder->setCheckEnable(checkencEnable());
    _deCoder->setCheckEnable(checkdecEnable());

    if (m_type == CodecH264) {
        _enCoder->setChecked(m_checkInfo->card.encode_h264_sel);
        _deCoder->setChecked(m_checkInfo->card.decode_h264_sel);
    } else if (m_type == CodecHEVC) {
        _enCoder->setChecked(m_checkInfo->card.encode_hevc_sel);
        _deCoder->setChecked(m_checkInfo->card.decode_hevc_sel);
    }
}

void HardwareSetSelect::setDecCheck(bool check)
{
    if(m_checkInfo) {
        _deCoder->setChecked(check);
        if(m_type == CodecH264) {
            m_checkInfo->card.decode_h264_sel = check;
        } else {
            m_checkInfo->card.decode_hevc_sel = check;
        }
    }
}

void HardwareSetSelect::setEncCheck(bool check)
{
    if(m_checkInfo) {
        _enCoder->setChecked(check);
        if(m_type == CodecH264) {
            m_checkInfo->card.encode_h264_sel = check;
        } else {
            m_checkInfo->card.encode_hevc_sel = check;
        }
    }
}

void HardwareSetSelect::onEncodecClick(bool check)
{
    if (m_type == CodecH264) {
        m_checkInfo->card.encode_h264_sel = check;
    } else if (m_type == CodecHEVC) {
        m_checkInfo->card.encode_hevc_sel = check;
    }
    emit sendEncCheckInfo(m_checkInfo,m_type,check);
}

void HardwareSetSelect::onDecodecClick(bool check)
{
    if (m_type == CodecH264) {
        m_checkInfo->card.decode_h264_sel = check;
    } else if (m_type == CodecHEVC) {
        m_checkInfo->card.decode_hevc_sel = check;
    }
    emit sendDecCheckInfo(m_checkInfo,m_type,check);
}

bool HardwareSetSelect::checkdecEnable()
{
    bool    enable = false;
    QString codec = "H264";
    if(m_type == CodecHEVC)
        codec = "HEVC";
    for(int i=0;i<m_checkInfo->items.count();i++) {
        if(m_checkInfo->items.at(i).codec == codec) {
            enable = m_checkInfo->items.at(i).dec == "true";
            if(enable)
                break;
        }
    }
    return enable;
}

bool HardwareSetSelect::checkencEnable()
{
    bool    enable = false;
    QString codec = "H264";
    if(m_type == CodecHEVC)
        codec = "HEVC";
    for(int i=0;i<m_checkInfo->items.count();i++) {
        if(m_checkInfo->items.at(i).codec == codec) {
            enable = m_checkInfo->items.at(i).enc == "true";
            if(enable)
                break;
        }
    }
    return enable;
}

void HardwareSetSelect::upUI()
{
    _mainWidget->setGeometry(0,0,this->width(),this->height());
    _enCoder->setGeometry(8*_scaleRatio,10*_scaleRatio,136*_scaleRatio,20*_scaleRatio);
    _deCoder->setGeometry(8*_scaleRatio,40*_scaleRatio,136*_scaleRatio,20*_scaleRatio);
}

void HardwareSetSelect::resizeEvent(QResizeEvent *)
{
    upUI();
}

HardwareSet::HardwareSet(QWidget *parent) :
    UpdateInterfaceWidget(parent),
    m_setIntelH264(NULL),
    m_setIntelHEVC(NULL),
    m_setNvidiaH264(NULL),
    m_setNvidiaHEVC(NULL),
    m_setAMDH264(NULL),
    m_setAMDHEVC(NULL),
    m_setDXVAH264(NULL),
    m_setDXVAHEVC(NULL)
{
    _scaleRatio = Global->_screenRatio;
    _tableHardware = new StableTableWidget(this);
    _tableHardware->installEventFilter(this);
    _tableHardware->setFrameShape(QFrame::NoFrame);
    _tableHardware->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _tableHardware->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _tableHardware->setEditTriggers(QAbstractItemView::NoEditTriggers
                                    | QAbstractItemView::SelectedClicked
                                    | QAbstractItemView::AnyKeyPressed);
    _tableHardware->verticalHeader()->setVisible(false);
    _tableHardware->horizontalHeader()->setVisible(false);
    _tableHardware->setStyleSheet("QTableWidget \
                                    { \
                                        background-color: #202021; \
                                        border: 0px solid #454545; \
                                        border-top:1px solid #454545; \
                                        border-left:1px solid #454545; \
                                        gridline-color: #454545; \
                                    }");
}

HardwareSet::~HardwareSet()
{

}

void HardwareSet::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    if(m_setIntelH264)
        m_setIntelH264->updateUI(scaleRatio);
    if(m_setIntelHEVC)
        m_setIntelHEVC->updateUI(scaleRatio);
    if(m_setNvidiaH264)
        m_setNvidiaH264->updateUI(scaleRatio);
    if(m_setNvidiaHEVC)
        m_setNvidiaHEVC->updateUI(scaleRatio);
    if(m_setAMDH264)
        m_setAMDH264->updateUI(scaleRatio);
    if(m_setAMDHEVC)
        m_setAMDHEVC->updateUI(scaleRatio);
    if(m_setDXVAH264)
        m_setDXVAH264->updateUI(scaleRatio);
    if(m_setDXVAHEVC)
        m_setDXVAHEVC->updateUI(scaleRatio);
    resizeCardInfo();
    resizeTable();
}

void HardwareSet::clear()
{
    _tableHardware->setRowCount(0);
    m_lstCheckInfo.clear();
}

static int hasCard(const QList<HardwareCheckInfo>& infos, CardType type)
{
    for (int i = 0; i < infos.count(); ++i) {
        if (infos.at(i).card.type == type) {
            return i;
        }
    }
    return -1;
}

void HardwareSet::setSettingTable(const QList<HardwareCheckInfo>& infos)
{
    m_lstCheckInfo = infos;

    _tableHardware->setRowCount(3);
    _tableHardware->setColumnCount(5);

    _tableHardware->setCellWidget(0, 0, buildItem(""));
    _tableHardware->setCellWidget(1, 0, buildItem("H264",14*_scaleRatio));
    _tableHardware->setCellWidget(2, 0, buildItem("HEVC",14*_scaleRatio));

    _tableHardware->setCellWidget(0, 1, buildItem("", 14*_scaleRatio, "#434652", true,
                                                   getInfo(CardDXVA) ? ":/res/png/dxva_result.png" : ":/res/png/dxva_dis.png"));
    _tableHardware->setCellWidget(0, 2, buildItem("", 14*_scaleRatio, "#434652", true,
                                                   getInfo(CardIntel) ? ":/res/png/intel_result.png" : ":/res/png/intel_dis.png"));
    _tableHardware->setCellWidget(0, 3, buildItem("", 14*_scaleRatio, "#434652", true,
                                                   getInfo(CardNvidia) ? ":/res/png/nvidia_result.png" : ":/res/png/nvidia_dis.png"));
    _tableHardware->setCellWidget(0, 4, buildItem("", 14*_scaleRatio, "#434652", true,
                                                   getInfo(CardAMD) ? ":/res/png/amd_result.png" : ":/res/png/amd_dis.png"));

    m_setDXVAH264   = new HardwareSetSelect(NULL, CodecH264, getInfo(CardDXVA));
    m_setDXVAHEVC   = new HardwareSetSelect(NULL, CodecHEVC, getInfo(CardDXVA));
    _tableHardware->setCellWidget(1, 1, m_setDXVAH264);
    _tableHardware->setCellWidget(2, 1, m_setDXVAHEVC);

    m_setIntelH264 = new HardwareSetSelect(NULL, CodecH264, getInfo(CardIntel));
    m_setIntelHEVC = new HardwareSetSelect(NULL, CodecHEVC, getInfo(CardIntel));
    _tableHardware->setCellWidget(1, 2, m_setIntelH264);
    _tableHardware->setCellWidget(2, 2, m_setIntelHEVC);

    m_setNvidiaH264 = new HardwareSetSelect(NULL, CodecH264, getInfo(CardNvidia));
    m_setNvidiaHEVC = new HardwareSetSelect(NULL, CodecHEVC, getInfo(CardNvidia));
    _tableHardware->setCellWidget(1, 3, m_setNvidiaH264);
    _tableHardware->setCellWidget(2, 3, m_setNvidiaHEVC);

    m_setAMDH264 = new HardwareSetSelect(NULL, CodecH264, getInfo(CardAMD));
    m_setAMDHEVC = new HardwareSetSelect(NULL, CodecHEVC, getInfo(CardAMD));
    _tableHardware->setCellWidget(1, 4, m_setAMDH264);
    _tableHardware->setCellWidget(2, 4, m_setAMDHEVC);

    conCheckBoxSlot();
    resizeCardInfo();
    resizeTable();
}

void HardwareSet::encCheckInfoSlot(HardwareCheckInfo *info, CodecType type, bool check)
{
    switch (info->card.type) {
    case CardIntel:
        if(check) {
            if(type == CodecH264) {
                m_setNvidiaH264->setEncCheck(false);
                m_setDXVAH264->setEncCheck(false);
                m_setAMDH264->setEncCheck(false);
            } else {
                m_setNvidiaHEVC->setEncCheck(false);
                m_setDXVAHEVC->setEncCheck(false);
                m_setAMDHEVC->setEncCheck(false);
            }
        }
        break;
    case CardNvidia:
        if(check) {
            if(type == CodecH264) {
                m_setIntelH264->setEncCheck(false);
                m_setDXVAH264->setEncCheck(false);
                m_setAMDH264->setEncCheck(false);
            } else {
                m_setIntelHEVC->setEncCheck(false);
                m_setDXVAHEVC->setEncCheck(false);
                m_setAMDHEVC->setEncCheck(false);
            }
        }
        break;
    case CardDXVA:
        if(check) {
            if(type == CodecH264) {
                m_setNvidiaH264->setEncCheck(false);
                m_setIntelH264->setEncCheck(false);
                m_setAMDH264->setEncCheck(false);
            } else {
                m_setIntelHEVC->setEncCheck(false);
                m_setNvidiaHEVC->setEncCheck(false);
                m_setAMDHEVC->setEncCheck(false);
            }
        }
        break;
    case CardAMD:
        if(check) {
            if(type == CodecH264) {
                m_setNvidiaH264->setEncCheck(false);
                m_setIntelH264->setEncCheck(false);
                m_setDXVAH264->setEncCheck(false);
            } else {
                m_setIntelHEVC->setEncCheck(false);
                m_setNvidiaHEVC->setEncCheck(false);
                m_setDXVAHEVC->setEncCheck(false);
            }
        }
        break;
    default:
        break;
    }
}

void HardwareSet::decCheckInfoSlot(HardwareCheckInfo *info, CodecType type, bool check)
{
    switch (info->card.type) {
    case CardIntel:
        if(check) {
            if(type == CodecH264) {
                m_setNvidiaH264->setDecCheck(false);
                m_setDXVAH264->setDecCheck(false);
                m_setAMDH264->setDecCheck(false);
            } else {
                m_setNvidiaHEVC->setDecCheck(false);
                m_setDXVAHEVC->setDecCheck(false);
                m_setAMDHEVC->setDecCheck(false);
            }

        }
        break;
    case CardNvidia:
        if(check) {
            if(type == CodecH264) {
                m_setIntelH264->setDecCheck(false);
                m_setDXVAH264->setDecCheck(false);
                m_setAMDH264->setDecCheck(false);
            } else {
                m_setIntelHEVC->setDecCheck(false);
                m_setDXVAHEVC->setDecCheck(false);
                m_setAMDHEVC->setDecCheck(false);
            }
        }
        break;
    case CardDXVA:
        if(check) {
            if(type == CodecH264) {
                m_setNvidiaH264->setDecCheck(false);
                m_setIntelH264->setDecCheck(false);
                m_setAMDH264->setDecCheck(false);
            } else {
                m_setIntelHEVC->setDecCheck(false);
                m_setNvidiaHEVC->setDecCheck(false);
                m_setAMDHEVC->setDecCheck(false);
            }
        }
        break;
    case CardAMD:
        if(check) {
            if(type == CodecH264) {
                m_setNvidiaH264->setDecCheck(false);
                m_setIntelH264->setDecCheck(false);
                m_setDXVAH264->setDecCheck(false);
            } else {
                m_setIntelHEVC->setDecCheck(false);
                m_setNvidiaHEVC->setDecCheck(false);
                m_setDXVAHEVC->setDecCheck(false);
            }
        }
        break;
    default:
        break;
    }
}

void HardwareSet::conCheckBoxSlot()
{
    connect(m_setDXVAH264,SIGNAL(sendDecCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(decCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setDXVAH264,SIGNAL(sendEncCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(encCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setIntelH264,SIGNAL(sendDecCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(decCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setIntelH264,SIGNAL(sendEncCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(encCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setNvidiaH264,SIGNAL(sendDecCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(decCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setNvidiaH264,SIGNAL(sendEncCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(encCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setAMDH264,SIGNAL(sendDecCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(decCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setAMDH264,SIGNAL(sendEncCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(encCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setDXVAHEVC,SIGNAL(sendDecCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(decCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setDXVAHEVC,SIGNAL(sendEncCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(encCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setIntelHEVC,SIGNAL(sendDecCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(decCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setIntelHEVC,SIGNAL(sendEncCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(encCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setNvidiaHEVC,SIGNAL(sendDecCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(decCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setNvidiaHEVC,SIGNAL(sendEncCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(encCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setAMDHEVC,SIGNAL(sendDecCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(decCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
    connect(m_setAMDHEVC,SIGNAL(sendEncCheckInfo(HardwareCheckInfo*,CodecType,bool)),
            SLOT(encCheckInfoSlot(HardwareCheckInfo*,CodecType,bool)));
}

HardwareCheckInfo *HardwareSet::getInfo(CardType cardType)
{
    for (int i = 0; i < m_lstCheckInfo.count(); ++i) {
        HardwareCheckInfo *info = &(m_lstCheckInfo[i]);
        if (info->card.type == cardType) {
            return info;
        }
    }
    return NULL;
}

bool HardwareSet::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == _tableHardware && event->type() == QEvent::Resize) {
        resizeTable();
    }
    return QWidget::eventFilter(watched, event);
}

void HardwareSet::resizeEvent(QResizeEvent *e)
{
    resizeCardInfo();
    QWidget::resizeEvent(e);
}

void HardwareSet::resizeCardInfo()
{
    _tableHardware->setGeometry(2*_scaleRatio,5*_scaleRatio,
                                this->width() - 4*_scaleRatio,200*_scaleRatio);
}

void HardwareSet::resizeTable()
{
    int tableWidth, tableHeight;
    tableWidth = _tableHardware->width();
    tableHeight = _tableHardware->height();
    _tableHardware->setRowHeight(0, tableHeight * (float(50) / float(200)));
    _tableHardware->setRowHeight(1, tableHeight * (float(75) / float(200)));
    _tableHardware->setRowHeight(2, tableHeight - (_tableHardware->rowHeight(0) + _tableHardware->rowHeight(1)) - 2*_scaleRatio);

    _tableHardware->setColumnWidth(0, (82*_scaleRatio));
    _tableHardware->setColumnWidth(1, (132*_scaleRatio));
    _tableHardware->setColumnWidth(2, (132*_scaleRatio));
    _tableHardware->setColumnWidth(3, (132*_scaleRatio));
    _tableHardware->setColumnWidth(4, tableWidth - _tableHardware->columnWidth(0)
                                                 - _tableHardware->columnWidth(1)
                                                 - _tableHardware->columnWidth(2)
                                                 - _tableHardware->columnWidth(3) - 2*_scaleRatio);
}

StableTableWidget::StableTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    this->viewport()->installEventFilter(this);
}

bool StableTableWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == this->viewport()
        && (e->type() == QEvent::Wheel || e->type() == QEvent::MouseMove)) {
        e->ignore();
        return true;
    }
    return QTableWidget::eventFilter(obj, e);
}
