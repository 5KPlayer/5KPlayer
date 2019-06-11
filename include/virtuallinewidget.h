#ifndef VIRTUALLINEWIDGET_H
#define VIRTUALLINEWIDGET_H

#include <QDialog>

class VirtualLineWidget : public QDialog
{
    Q_OBJECT
public:
    explicit VirtualLineWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    void setLineWidth(const int &w);
    void setLineColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    int _lineWidth;
    QColor _lineColor;
};

#endif // VIRTUALLINEWIDGET_H
