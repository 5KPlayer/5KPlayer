#ifndef GGLABEL_H
#define GGLABEL_H

#include <QObject>
#include <QLabel>
#include "updateinterface.h"

class GGLabel : public QLabel
{
    Q_OBJECT
public:
    explicit GGLabel(QWidget *parent = 0);

    void setMouseHead(bool state);

    void setGGData(QString Url);
    QString getGGData();

    void icoMoveClear();

    void setFontColor(QString color);
    QString getFontColor();
    void setPixMapPath(QString norPath,QString enPath);

signals:
    void clicked();
private:
    void showPixMap(QString path);
protected:
    virtual void mouseMoveEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);


private:
    bool    _mouseHead;
    QString _dataUrl;
    QString _norPixPath;
    QString _enPixPath;
    QMovie* _proMovie;
    double  _scaleRatio;
};

#endif // GGLABEL_H
