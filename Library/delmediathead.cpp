#include "delmediathead.h"
#include <QMetaType>
#include "sqloperation.h"
DelMediaThead::DelMediaThead(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<MEDIAINFO>>("QList<MEDIAINFO>");
}

void DelMediaThead::setDelMediaInfo(const QList<MEDIAINFO> &delMedia, const QString &itemName,bool isSelectAll, bool ismove)
{
    _isSelectAll = isSelectAll;
    _delData = delMedia;
    _itemName= itemName;
    _isMove  = ismove;
}

void DelMediaThead::delData()
{
    if(_isSelectAll) {
        if(_itemName == "pMovies" || _itemName == "pMusic") {
            Sql->sqlDelforMediaType(_delData.at(0).mediaType);
        } else {
            Sql->sqlDelforItemName(_delData.at(0).itemname);
        }
    } else {
        if(_itemName == "pMovies" || _itemName == "pMusic" || _isMove) {
            Sql->sqlDelforfilePath(_delData);
        } else {
            Sql->sqlDelforfile(_delData);
        }
    }
}

void DelMediaThead::run()
{
    delData();
}
