#ifndef HARDWAREANALYSIS_H
#define HARDWAREANALYSIS_H

#include <QWidget>
#include "updateinterface.h"
#include "dystructu.h"

// 硬件检测解析界面

class QScrollArea;
class DYPixLabel;
class QLabel;
class QTimer;
class OpacityWidget;


typedef struct CheckItem {
    CardType type;
    bool isShow;
    bool isTitle;
    OpacityWidget *widget;
    QRect geomety;
    CheckItem();
    void clear();
} CheckItem;

class HardwareAnalysis : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit HardwareAnalysis(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private:
    void initPixLabel(DYPixLabel** label,QString res,QString text);

signals:
    void doShowCompleted();

public slots:
    void setCheckInfo(QList<HardwareCheckInfo> *lstCheckInfo);
    void setCheckFinished(bool finished);
    void clear();
    void addCardInfo(const CardInfo &info);
    void showItems();
    void showOneItemInfo(const HardwareCheckItem &item);
    void showOneItem();
    void showItem(const CheckItem &item, bool needAnimation = true);
private:
    void resizeTable();

protected:
    virtual void resizeEvent(QResizeEvent *);
private:
    QScrollArea*   _analysisArea;
    QWidget*       _analysisWidget;

    DYPixLabel*    _codecBtn;      // 编码
    DYPixLabel*    _profileBtn;    // 输出配置
    DYPixLabel*    _levelBtn;      // 等级
    DYPixLabel*    _resolutionBtn; // 最大分辨率
    DYPixLabel*    _isEncodecBtn;  // 是否支持编码
    DYPixLabel*    _isDecodecBtn;  // 是否支持解码

    QList<CheckItem> m_lstItems;
    QList<HardwareCheckInfo> *m_lstCheckInfo;

    CheckItem m_itemIntel, m_itemNvidia, m_itemAMD;
    bool m_checkFinished;

    QTimer *m_timer;
    int m_index;
    int m_currentY;

    double         _scaleRatio;
};

#endif // HARDWAREANALYSIS_H
