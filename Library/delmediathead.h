#ifndef DELMEDIATHEAD_H
#define DELMEDIATHEAD_H

#include <QObject>
#include <QThread>
#include "dystructu.h"
class DelMediaThead : public QThread
{
    Q_OBJECT
public:
    explicit DelMediaThead(QObject *parent = 0);

    void setDelMediaInfo(const QList<MEDIAINFO> &delMedia, const QString &itemName, bool isSelectAll, bool ismove);
    void delData();

public slots:
protected:
     void run();
private:
     QList<MEDIAINFO> _delData;
     QString          _itemName;
     bool             _isSelectAll;
     bool             _isMove;
};

#endif // DELMEDIATHEAD_H
