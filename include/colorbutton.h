#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include "updateinterface.h"

class QLabel;
class ComboBox;

class ColorButton : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget *parent = 0);

    void setColor(const QString &);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private slots:
    void showColor(const QString &);

protected:
    virtual void mousePressEvent(QMouseEvent*);
    virtual void resizeEvent(QResizeEvent *);

signals:
    void valueChanged(QString);

private:
    ComboBox *_comboBox;
    QWidget *_colorW;
};

#endif // COLORBUTTON_H
