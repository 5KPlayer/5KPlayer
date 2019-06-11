#ifndef HARDWARESTATISTICS_H
#define HARDWARESTATISTICS_H

#include <QMap>
#include "updateinterface.h"
#include "dystructu.h"


class QLabel;
class QTableWidget;
class CardInfoWidget: public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    CardInfoWidget(QWidget* parent = 0);
    ~CardInfoWidget();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void setInfo(const HardwareCheckInfo& info);
protected:
    virtual void resizeEvent(QResizeEvent *);
private:
    QLabel*     _devIco;
    QLabel*     _devName;
    QLabel*     _devVer;

    double      _scaleRatio;

};

class HardwareStatistics : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit HardwareStatistics(QWidget *parent = 0);
    ~HardwareStatistics();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

public slots:
    void setStatisticsTable(const QList<HardwareCheckInfo> &infos);
    void clear();

protected:
    void getInfo(CardType type,
                  const QString &codec,
                  const QString &profile,
                  QString &res,
                  bool &encode, bool &decode, QString &level);

    void resizeTable();
    bool eventFilter(QObject *obj, QEvent *e);

private:
    QTableWidget* _tableStatistics;

    QMap<QString, QList<QString>> m_codec2Profiles;
    QList<HardwareCheckInfo> m_lstCheckInfo;
    double _scaleRatio;
};

#endif // HARDWARESTATISTICS_H
