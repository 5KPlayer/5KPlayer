#ifndef HARDWARESET_H
#define HARDWARESET_H

#include "updateinterface.h"
#include <QTableWidget>

#include "dystructu.h"

class DYCheckBox;
class HardwareSetSelect: public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    HardwareSetSelect(QWidget* parent, CodecType type, HardwareCheckInfo *info);
    ~HardwareSetSelect();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void setSelectInfo();

    void setDecCheck(bool check);
    void setEncCheck(bool check);

signals:
    sendEncCheckInfo(HardwareCheckInfo *info,CodecType type,bool check);
    sendDecCheckInfo(HardwareCheckInfo *info,CodecType type,bool check);
protected slots:
    void onEncodecClick(bool check);
    void onDecodecClick(bool check);
private:
    bool checkdecEnable();
    bool checkencEnable();

    void upUI();
protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    HardwareCheckInfo * m_checkInfo;
    CodecType           m_type;
    QWidget*            _mainWidget;
    DYCheckBox*         _enCoder;
    DYCheckBox*         _deCoder;
    double              _scaleRatio;
};

class StableTableWidget: public QTableWidget
{
public:
    StableTableWidget(QWidget *parent);

protected:
    bool eventFilter(QObject *obj, QEvent *e);
};

class HardwareSet : public UpdateInterfaceWidget
{
    Q_OBJECT

public:
    explicit HardwareSet(QWidget *parent = 0);
    ~HardwareSet();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

public slots:
    void clear();
    void setSettingTable(const QList<HardwareCheckInfo>& infos);
    QList<HardwareCheckInfo> checkInfos() {return m_lstCheckInfo;}

    void encCheckInfoSlot(HardwareCheckInfo *info,CodecType type,bool check);
    void decCheckInfoSlot(HardwareCheckInfo *info,CodecType type,bool check);
private:
    void conCheckBoxSlot();
protected:
    HardwareCheckInfo* getInfo(CardType cardType);
    bool eventFilter(QObject *watched, QEvent *event);
    void resizeEvent(QResizeEvent *e);
    void resizeCardInfo();
    void resizeTable();

private:
    QList<HardwareCheckInfo> m_lstCheckInfo;

    StableTableWidget* _tableHardware;

    HardwareSetSelect *m_setIntelH264;
    HardwareSetSelect *m_setIntelHEVC;
    HardwareSetSelect *m_setNvidiaH264;
    HardwareSetSelect *m_setNvidiaHEVC;
    HardwareSetSelect *m_setAMDH264;
    HardwareSetSelect *m_setAMDHEVC;
    HardwareSetSelect *m_setDXVAH264;
    HardwareSetSelect *m_setDXVAHEVC;

    double              _scaleRatio;
};

#endif // HARDWARESET_H
