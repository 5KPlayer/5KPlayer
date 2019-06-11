#ifndef DYCHECKBOX_H
#define DYCHECKBOX_H

#include "updateinterface.h"

class DYCheckBox : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit DYCheckBox(QWidget *parent = 0,QString text = "");

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setChecked(const bool &check);
    bool isChecked() const {return _checkDown;}

    void setCheckEnable(const bool &enable);

    void setText(const QString &text) {_checkText = text; this->update();}
    QString text() const {return _checkText;}
    void setRightText(bool right);
    void setTextFontBlod(bool bold);
    void setTextColor(QString color);

signals:
    void checkChange(bool check);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void paintEvent(QPaintEvent *);

private:
    QString     _checkText;
    bool        _enable;
    bool        _checkDown;
    bool        _fontBlod;
    bool        b_mouseIn;
    bool        b_rightText;
    double      _scaleRatio;
    QString     _textColor;
};

#endif // DYCHECKBOX_H
