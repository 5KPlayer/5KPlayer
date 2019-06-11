#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>

class ComboBox : public QComboBox
{
public:
    ComboBox(QWidget *parent = Q_NULLPTR);
    void setFont(const QFont &);
    void upStyleSheet(const qreal &ratio = 1.0);
    void setViewWidth(const uint &w);

    static const QString verticalStyle(const qreal &ratio = 1.0, const QString &color="white");

private:
    const QString style(const qreal &ratio = 1.0) const;
protected:
    void wheelEvent(QWheelEvent* e);

};

#endif // COMBOBOX_H
