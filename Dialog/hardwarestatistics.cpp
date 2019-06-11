#include "hardwarestatistics.h"
#include "globalarg.h"
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QEvent>
#include <QDebug>

CardInfoWidget::CardInfoWidget(QWidget *parent):
    UpdateInterfaceWidget(parent)
{
    _devIco     = new QLabel(this);
    _devName    = new QLabel(this);
    _devVer     = new QLabel(this);
    _scaleRatio = Global->_screenRatio;
}

CardInfoWidget::~CardInfoWidget()
{

}

void CardInfoWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
}

void CardInfoWidget::setInfo(const HardwareCheckInfo &info)
{
    if (info.card.type == CardIntel) {
        _devIco->setPixmap(QPixmap(":/res/png/intel_result.png").scaled(42*_scaleRatio,28*_scaleRatio));
    } else if (info.card.type == CardNvidia) {
        _devIco->setPixmap(QPixmap(":/res/png/nvidia_result.png").scaled(42*_scaleRatio,28*_scaleRatio));
    } else if (info.card.type == CardAMD) {
        _devIco->setPixmap(QPixmap(":/res/png/amd_result.png").scaled(42*_scaleRatio,28*_scaleRatio));
    }
    _devName->setText(info.card.name);
    _devVer->setText(info.card.version);

    _devName->setStyleSheet("color:rgb(125,125,125)");
    _devVer->setStyleSheet("color:rgb(125,125,125)");
}

void CardInfoWidget::resizeEvent(QResizeEvent *)
{
    QFont font;
    font.setFamily("Tahoma");
    font.setPixelSize(12*_scaleRatio);
    _devName->setFont(font);
    _devVer->setFont(font);

    int nameW = _devName->fontMetrics().width(_devName->text());
    int xfix = 0;
    int labWidth = 144*_scaleRatio;
    if(this->width() - 54*_scaleRatio > nameW) {
        xfix = (this->width() - 54*_scaleRatio - labWidth) / 2;
    } else {
        // 这里需要重新渲染name的text
    }
    _devName->setGeometry(52*_scaleRatio+xfix,10*_scaleRatio,labWidth,20*_scaleRatio);
    _devVer->setGeometry(52*_scaleRatio+xfix,32*_scaleRatio,labWidth,20*_scaleRatio);
    _devIco->setGeometry(2*_scaleRatio+xfix,16*_scaleRatio,48*_scaleRatio,28*_scaleRatio);
}

static QWidget *buildItem(const QString &text = QString(),
                         int baseFontSize = 13,
                         const QString &color = "#e6e6e6",
                         bool bold = false,
                         const QString &icon = QString())
{
    if (icon.isEmpty()) {
        QLabel *lab = new QLabel;
        lab->setProperty("baseFontSize", baseFontSize);
        if (bold) {
            lab->setStyleSheet(QString("background-color:#202021; color:%1; font-weight: bold;").arg(color));
        } else {
            lab->setStyleSheet(QString("background-color:#202021; color:%1").arg(color));
        }
        lab->setAlignment(Qt::AlignCenter);
        lab->setText(text);
        return lab;
    } else {
        QPushButton *btn = new QPushButton;
        btn->setProperty("baseFontSize", baseFontSize);
        btn->setStyleSheet(QString("background-color:#202021; border: 0px; color:%1; font-weight: %2;").arg(color).arg(bold ? "bold" : "normal"));
        btn->setText(text);
        btn->setProperty("baseIconSize", QVariant(QSize(14*Global->_screenRatio, 11*Global->_screenRatio)));
        btn->setIcon(QIcon(icon));
        return btn;
    }
}

HardwareStatistics::HardwareStatistics(QWidget *parent) :
    UpdateInterfaceWidget(parent)
{
    _scaleRatio = Global->_screenRatio;
    this->installEventFilter(this);
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_codec2Profiles.insert("H264", QStringList() << "Baseline" << "Main" << "Extended" << "High");
    m_codec2Profiles.insert("HEVC", QStringList() << "Main" /*<< "Main10" << "MainSP" << "Rext"*/);
    _tableStatistics    = new QTableWidget(this);
    _tableStatistics->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _tableStatistics->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _tableStatistics->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _tableStatistics->verticalHeader()->setVisible(false);
    _tableStatistics->horizontalHeader()->setVisible(false);
    _tableStatistics->setStyleSheet("QTableWidget \
                                    { \
                                        background-color: #202021; \
                                        border: 1px solid #454545; \
                                        gridline-color: #454545; \
                                    }");

}

HardwareStatistics::~HardwareStatistics()
{

}

void HardwareStatistics::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    resizeTable();
}

void HardwareStatistics::clear()
{
    _tableStatistics->setRowCount(0);
}

void HardwareStatistics::setStatisticsTable(const QList<HardwareCheckInfo> &infos)
{
    int rowCount = 2, colCount = 0;
    int index = 0;
    int curRowIndex = 0;

    /*Title*/
    m_lstCheckInfo = infos;
    colCount = 2 + m_lstCheckInfo.count() * 3;
    _tableStatistics->setRowCount(rowCount);
    _tableStatistics->setColumnCount(colCount);
    _tableStatistics->setSpan(0, 0, 2, 1);
    _tableStatistics->setCellWidget(0, 0, buildItem(Lang("HardCheck/codec"), 14*_scaleRatio, "#7d7d7e", true));
    _tableStatistics->setSpan(0, 1, 2, 1);
    _tableStatistics->setCellWidget(0, 1, buildItem(Lang("HardCheck/profile"), 14*_scaleRatio, "#7d7d7e", true));
    for (int i = 2; i < colCount; i += 3) {
        _tableStatistics->setSpan(0, i, 1, 3);
        if (index >= m_lstCheckInfo.count())
            continue;
        CardInfoWidget *widget = new CardInfoWidget();
        widget->setInfo(m_lstCheckInfo.at(index++));
        _tableStatistics->setCellWidget(0, i, widget);
        _tableStatistics->setCellWidget(1, i, buildItem(Lang("HardCheck/level"), 14*_scaleRatio, "#7d7d7e", true));
        _tableStatistics->setCellWidget(1, i + 1, buildItem(Lang("HardCheck/encoder"), 14*_scaleRatio, "#7d7d7e", true));
        _tableStatistics->setCellWidget(1, i + 2, buildItem(Lang("HardCheck/decoder"), 14*_scaleRatio, "#7d7d7e", true));
    }

    /*Content*/
    curRowIndex = _tableStatistics->rowCount();
    QMapIterator<QString, QList<QString>> itor(m_codec2Profiles);
    while (itor.hasNext()) {
        itor.next();
        const QString codec = itor.key();
        const QList<QString> &lstProfile = itor.value();
        _tableStatistics->setRowCount(_tableStatistics->rowCount() + lstProfile.count());
        for (int i = 0; i < lstProfile.count(); ++i) {
            _tableStatistics->setCellWidget(curRowIndex, 0, buildItem(codec, 13*_scaleRatio, "#626873"));
            _tableStatistics->setCellWidget(curRowIndex, 1, buildItem(lstProfile.at(i), 13*_scaleRatio, "#626873"));
            int decencIndex = 0;
            for (int index = 0; index < m_lstCheckInfo.count(); ++index) {
                const HardwareCheckInfo &info = m_lstCheckInfo.at(index);
                bool encEnabled = false, decEnabled = false;
                QString level, res;
                getInfo(info.card.type, codec, lstProfile.at(i), res, encEnabled, decEnabled, level);
                _tableStatistics->setCellWidget(curRowIndex, 2 + decencIndex++, buildItem(level, 13*_scaleRatio, "#626873"));
                if (encEnabled) {
                    _tableStatistics->setCellWidget(curRowIndex, 2 + decencIndex++, buildItem(res, 13*_scaleRatio, "#626873"));
                } else {
                    _tableStatistics->setCellWidget(curRowIndex, 2 + decencIndex++, buildItem("", 13*_scaleRatio, "#626873", false, ":/res/png/hwError.png"));
                }
                if (decEnabled) {
                    _tableStatistics->setCellWidget(curRowIndex, 2 + decencIndex++, buildItem(res, 13*_scaleRatio, "#626873"));
                } else {
                    _tableStatistics->setCellWidget(curRowIndex, 2 + decencIndex++, buildItem("", 13*_scaleRatio, "#626873", false, ":/res/png/hwError.png"));
                }
            }
            ++curRowIndex;
        }
    }

    resizeTable();
}


void HardwareStatistics::getInfo(CardType type,
                                  const QString &codec,
                                  const QString &profile,
                                  QString &res,
                                  bool &encode, bool &decode, QString &level)
{
    for (int i = 0; i < m_lstCheckInfo.count(); ++i) {
        if (m_lstCheckInfo.at(i).card.type == type) {
            const QList<HardwareCheckItem> &items = m_lstCheckInfo.at(i).items;
            for (int j = 0; j < items.count(); ++j) {
                const HardwareCheckItem &item = items.at(j);
                if (QString::compare(item.codec, codec, Qt::CaseInsensitive) == 0 &&
                    QString::compare(item.profile, profile, Qt::CaseInsensitive) == 0) {
                    encode = (item.enc == "true" || item.enc == "E");
                    decode = (item.dec == "true" || item.dec == "E");
                    level = item.level;
                    res = item.res;
                    return;
                }
            }
        }
    }
}

void HardwareStatistics::resizeTable()
{
    _tableStatistics->setGeometry(2*_scaleRatio,2*_scaleRatio,this->width()-4*_scaleRatio,this->height()-4*_scaleRatio);
    const int itemCount = 5;
    int rowOneH = 62*_scaleRatio;//RATIO_FIT(62);
    int rowTwoH = 45*_scaleRatio;//RATIO_FIT(45);
    int tableWidth = _tableStatistics->width();
    int tableHeight = _tableStatistics->height();
    int rowHeight = (tableHeight - rowOneH - rowTwoH) / itemCount;
    int lastHeight = tableHeight - rowHeight * (itemCount - 1) - rowOneH - rowTwoH;
    for (int i = 0; i < _tableStatistics->rowCount(); ++i) {
        if (i == 0) {
            _tableStatistics->setRowHeight(i, rowOneH);
        } else if (i == 1) {
            _tableStatistics->setRowHeight(i, rowTwoH);
        } else if (i == (_tableStatistics->rowCount() - 1)) {
            _tableStatistics->setRowHeight(i, lastHeight - 1);
        } else {
            _tableStatistics->setRowHeight(i, rowHeight);
        }
    }
    int otherWidth = 112*_scaleRatio;//RATIO_FIT(112);
    if (m_lstCheckInfo.count() == 1) {
        otherWidth = 162*_scaleRatio;//RATIO_FIT(162);
    }
    int colCount = _tableStatistics->columnCount();
    int cardWidth = (tableWidth - otherWidth * 2) / (colCount - 2);
    int lastWidth = tableWidth - otherWidth * 2 - cardWidth * (colCount - 3);
    for (int i = 0; i < _tableStatistics->columnCount(); ++i) {
        if (i == (_tableStatistics->columnCount() - 1)) {
            _tableStatistics->setColumnWidth(i, lastWidth - 1);
        } else {
            _tableStatistics->setColumnWidth(i, i < 2 ? otherWidth : cardWidth);
        }
    }
}

bool HardwareStatistics::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == _tableStatistics && e->type() == QEvent::Resize) {
        resizeTable();
    }
    return QWidget::eventFilter(obj, e);
}
