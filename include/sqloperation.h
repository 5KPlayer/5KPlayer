#ifndef SQLOPERATION_H
#define SQLOPERATION_H

#include <QObject>
#include <QSqlDatabase>
#include "singleton.h"
#include "dystructu.h"

#define Sql SqlOperation::getInstance()

class SqlOperation : public QObject,public Singleton<SqlOperation>
{
    Q_OBJECT
    friend class  Singleton<SqlOperation>;
    friend class QSharedPointer<SqlOperation>;

public:
    void initTabel();

    void initData(QList<MEDIAINFO>*urlMedia, QList<MEDIAINFO> *media,QList<QString> *itemList);

    void insertmediaListtosql(const QList<MEDIAINFO> &mediaList);
    bool insertmediatosql(const MEDIAINFO &mediainfo);
    void updatamediatosql(const QList<MEDIAINFO> &mediaList, const MEDIAINFO &mediainfo);

    void updataUrlmediatoSql(const  QList<MEDIAINFO> &delList,const QList<MEDIAINFO> &mediaList);

    bool sqlDelforMediaType(int mediaType);
    bool sqlDelforItemName(QString itemName);
    bool sqlDelforfilePath(QList<MEDIAINFO> mediadata);
    bool sqlDelforfile(QList<MEDIAINFO> mediadata);
    bool sqlDelUrlList(QList<MEDIAINFO> mediadata);
    bool sqlDelforUrlMedia(MEDIAINFO mediadata);

    bool upMediaFavorites(const MEDIAINFO &mediainfo);
    void upUrlMediaSub(const MEDIAINFO &mediainfo);

    bool addItemtoSql(const QString &itemName);
    bool delItemtoSql(const QString &itemName);

    void sqlGetMusicMediaList(QList<MEDIAINFO> *musicList);

    // Live历史记录
    /*!
     * \brief initLiveUrl
     * \param recentList
     * \param urlType 1、是历史记录 2、收藏链接
     */
    void initLiveUrl(QStringList* LiveList, const int &urlType);
    void removeLiveUrl(const QString &liveurl);
    void addLiveurlToSql(const QString &liveurl, const int &urlType);

    void initRecent(QStringList* recentList);
    bool removeRecent(const QString &recentPath);
    void addRecent(const QString &recentPath);
    void clearRecent();

private:
    Q_DISABLE_COPY(SqlOperation)
    explicit SqlOperation(QObject *parent = 0);
    ~SqlOperation();

    QStringList subToList(const QString &sub);
    QString     ListToStr(const QStringList &subList);

    QSqlDatabase _db;
};

#endif // SQLOPERATION_H
