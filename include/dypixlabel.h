#ifndef DYPIXLABEL_H
#define DYPIXLABEL_H

#include "updateinterface.h"

class DYPixLabel : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit DYPixLabel(QString res,QString text,QWidget *parent = 0);

    void initSize(int w, int h);
    bool setFontUnderline(bool overLine);
    void setMouseHead(bool head);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);
signals:
    void clicked();
protected:
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
private:
    bool        _fontUnderrline;
    bool        _mousePress;
    bool        _isMouseHead;
    double      _scaleRatio;
    QString     _pixRes;
    QString     _text;
};

#endif // DYPIXLABEL_H
