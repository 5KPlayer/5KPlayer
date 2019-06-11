#ifndef BACKSVGTEXTBUTTON_H
#define BACKSVGTEXTBUTTON_H

#include <QObject>
#include <QLabel>

#include "updateinterface.h"
#include "transeventlabel.h"
#include "transeventico.h"

class BackSvgTextButton : public QLabel, public UpdateInterface
{
    Q_OBJECT
public:
    explicit BackSvgTextButton(QWidget *parent = 0,bool isRadiu = false);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setButtonColor(QString norCol,QString onCol,QString downCol);
    void setButtonInfo(QString svg,QString text);
private:
    void upUI();
    void upButtonState(int state);
signals:
    void clicked();
protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

private:
    TransEventIco*      _svg;
    TransEventLabel*    _text;

    QString     _norColor;
    QString     _onColor;
    QString     _downColor;
    QString     _textInfo;

    QString     _pressImage;
    QString     _normalImage;

    double      _scaleRatio;
    bool        _isRadius;
    int         _btnState;

    enum {
        BUTTON_NOR      = 0,
        BUTTON_HOVER    = 1,
        BUTTON_DOWN     = 2
    };

};

#endif // BACKSVGTEXTBUTTON_H
