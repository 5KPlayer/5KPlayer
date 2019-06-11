#ifndef TRANSEVENTTEXTBROWSER_H
#define TRANSEVENTTEXTBROWSER_H

#include <QTextBrowser>

class TransEventTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit TransEventTextBrowser(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
};

#endif // TRANSEVENTTEXTBROWSER_H
