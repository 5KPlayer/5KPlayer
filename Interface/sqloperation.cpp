#include "sqloperation.h"
#include <QtSql>
#include "globalarg.h"
#include <QVersionNumber>

SqlOperation::SqlOperation(QObject *parent) : QObject(parent)
{
    QString path = Global->getAppdataPath();
    _db = QSqlDatabase::addDatabase("QSQLITE");    //添加数据库驱动
    _db.setDatabaseName(path + "/5KPlay.db");
    _db.open();

    initTabel();
}

SqlOperation::~SqlOperation()
{
    _db.close();
}

void SqlOperation::initTabel()
{
    QSqlQuery query;
    if (!query.exec("create table IF NOT EXISTS library( \
                    itemname       varchar, \
                    title          varchar, \
                    ext            varchar, \
                    filepath       varchar, \
                    fileSoure      varchar, \
                    type           INTEGER, \
                    webUrl         varchar, \
                    subtitle       varchar, \
                    videoinfojson  varchar, \
                    formatID       varchar, \
                    favorites      int,     \
                    album          varchar, \
                    artist         varchar, \
                    pin            varchar, \
                    rotate         INTEGER, \
                    duration       INTEGER, \
                    filesize       DOUBLE,  \
                    fileAddTime    INTEGER, \
                    screenshot     BLOB,    \
                    node1          varchar, \
                    node2          varchar);")){
        qDebug("创建library表 error");
    }
    //每个媒体文件的流信息
    if (!query.exec("create table IF NOT EXISTS streamTabel( \
                    FilePin     varchar, \
                    FilePath    varchar, \
                    ItemName    varchar, \
                    MediaType   INTEGER, \
                    Streamindex varchar, \
                    Streamtype  INTEGER, \
                    filesize    DOUBLE,  \
                    Bitrate     INTEGER, \
                    ext         varchar, \
                    Codec       varchar, \
                    Language    varchar, \
                    Description varchar, \
                    AspectRatio varchar, \
                    Width       INTEGER, \
                    Height      INTEGER, \
                    RateDen     INTEGER, \
                    RateNum     INTEGER, \
                    Channel     INTEGER, \
                    Rate        INTEGER, \
                    Encoding    varchar, \
                    node1       varchar, \
                    node2       varchar);")){
        qDebug("创建streamTabel表 error");
    }
    //新建列表信息
    if (!query.exec("create table IF NOT EXISTS itemlist( \
                    itemname    varchar);")){
        qDebug("创建itemlist表 error");
    }
    //live url信息
    if (!query.exec("create table IF NOT EXISTS liveUrl( \
                    LivePath    varchar, \
                    LiveType    INTEGER);")){ //是否收藏
        qDebug("liveUrl error");
    }
    //播放历史记录
    if (!query.exec("create table IF NOT EXISTS Recent(RecentPath varchar);")){
        qDebug("Recent error");
    }
}

void SqlOperation::initData(QList<MEDIAINFO> *urlMedia, QList<MEDIAINFO> *media, QList<QString> *itemList)
{
    QSqlQuery query;
    query.exec(QString("select * from library"));
    while(query.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        MEDIAINFO videoinfo;
        videoinfo.itemname              = query.value("itemname").toString();
        videoinfo.title                 = query.value("title").toString();
        videoinfo.ext                   = query.value("ext").toString();
        videoinfo.filepath              = query.value("filepath").toString();
        videoinfo.fileSoure             = query.value("fileSoure").toString();
        videoinfo.mediaType             = query.value("type").toInt();
        videoinfo.webUrl                = query.value("webUrl").toString();
        videoinfo.subtitleList          = subToList(query.value("subtitle").toString());
        videoinfo.videoinfojson         = query.value("videoinfojson").toString();
        videoinfo.formatID              = query.value("formatID").toString();
        videoinfo.isFavorites           = query.value("favorites").toInt();
        videoinfo.album                 = query.value("album").toString();
        videoinfo.artist                = query.value("artist").toString();
        videoinfo.rotate                = query.value("rotate").toInt();
        videoinfo.duration              = query.value("duration").toInt();
        videoinfo.filesize              = query.value("filesize").toDouble();
        videoinfo.fileAddTime           = query.value("fileAddTime").toInt();
        videoinfo.pin                   = query.value("pin").toString();
        videoinfo.screenshot            = query.value("screenshot").toByteArray();
        QString filepath = videoinfo.filepath;
        if(videoinfo.mediaType == 3)
            filepath = videoinfo.webUrl;
        QSqlQuery streamquery;
        streamquery.prepare(QString("select * from streamTabel where FilePin = :Pin and FilePath = :FilePaht and ItemName = :itemName"));
        streamquery.bindValue(":Pin",       videoinfo.pin);
        streamquery.bindValue(":FilePaht",  filepath);
        streamquery.bindValue(":itemName",  videoinfo.itemname);
        streamquery.exec();
        while(streamquery.next())
        {
            STREAMINFO streaminfo;
            streaminfo.streamId     = streamquery.value("Streamindex").toString();
            streaminfo.streamType   = streamquery.value("Streamtype").toInt();
            streaminfo.bitRate      = streamquery.value("Bitrate").toInt();
            streaminfo.fileSize     = streamquery.value("filesize").toInt();
            streaminfo.ext          = streamquery.value("ext").toString();
            streaminfo.codec        = streamquery.value("Codec").toString();
            streaminfo.language     = streamquery.value("Language").toString();
            streaminfo.description  = streamquery.value("Description").toString();
            streaminfo.aspectRatio  = streamquery.value("AspectRatio").toString();
            streaminfo.resolution.setWidth(streamquery.value("Width").toInt());
            streaminfo.resolution.setHeight(streamquery.value("Height").toInt());
            streaminfo.rateDen      = streamquery.value("RateDen").toInt();
            streaminfo.rateNum      = streamquery.value("RateNum").toInt();
            streaminfo.Channel      = streamquery.value("Channel").toInt();
            streaminfo.Encoding     = streamquery.value("Encoding").toString();
            videoinfo.streamList.append(streaminfo);
        }
        if(videoinfo.mediaType == 3) {
            urlMedia->prepend(videoinfo);
        } else if(videoinfo.mediaType == 2 || videoinfo.mediaType == 1){
            media->prepend(videoinfo);
        }
    }

    query.exec(QString("select * from itemlist"));
    while(query.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
      QString itemname =query.value(0).toString();
      itemList->append(itemname);
    }
}

void SqlOperation::insertmediaListtosql(const QList<MEDIAINFO> &mediaList)
{
    QSqlDatabase    db_sqlite = QSqlDatabase::database();
    QSqlQuery   query("", db_sqlite);
    QSqlQuery   streamquery("", db_sqlite);
    query.prepare("insert into \
        library(itemname,title,ext,filepath,fileSoure,type,webUrl,subtitle,videoinfojson,formatID,favorites,album,artist,\
                pin,rotate,duration,filesize,fileAddTime,screenshot)\
        VALUES (:itemname,:title,:ext,:filepath,:fileSoure,:type,:webUrl,:Subtitle,:videoinfojson,:formatID,:favorites,:album,:artist,\
                :pin,:rotate,:duration,:filesize,:fileAddTime,:screenshot)");

    streamquery.prepare("insert into \
        streamTabel(FilePin,FilePath,ItemName,MediaType,Streamindex,Streamtype,filesize,Bitrate,ext,Codec,Language,Description,\
                    AspectRatio,Width,Height,RateDen,RateNum,Channel,Rate,Encoding)\
        VALUES (:FilePin,:FilePath,:ItemName,:MediaType,:Streamindex,:Streamtype,:filesize,:Bitrate,:ext,:Codec,:Language,:Description,\
                :AspectRatio,:Width,:Height,:RateDen,:RateNum,:Channel,:Rate,:Encoding)");
    db_sqlite.transaction();
    for(int i=0;i<mediaList.size();++i) {
        query.bindValue(":itemname",        mediaList.at(i).itemname);   //在绑定要插入的值
        query.bindValue(":title",           mediaList.at(i).title);
        query.bindValue(":ext",             mediaList.at(i).ext);
        query.bindValue(":filepath",        mediaList.at(i).filepath);
        query.bindValue(":fileSoure",       mediaList.at(i).fileSoure);
        query.bindValue(":type",            mediaList.at(i).mediaType);
        query.bindValue(":webUrl",          mediaList.at(i).webUrl);
        query.bindValue(":Subtitle",        ListToStr(mediaList.at(i).subtitleList));
        query.bindValue(":videoinfojson",   mediaList.at(i).videoinfojson);
        query.bindValue(":formatID",        mediaList.at(i).formatID);
        query.bindValue(":favorites",       mediaList.at(i).isFavorites);
        query.bindValue(":album",           mediaList.at(i).album);
        query.bindValue(":artist",          mediaList.at(i).artist);
        query.bindValue(":duration",        mediaList.at(i).duration);
        query.bindValue(":rotate",          mediaList.at(i).rotate);
        query.bindValue(":filesize",        mediaList.at(i).filesize);
        query.bindValue(":fileAddTime",     mediaList.at(i).fileAddTime);
        query.bindValue(":pin",             mediaList.at(i).pin);
        query.bindValue(":screenshot",      mediaList.at(i).screenshot);
        query.exec();
        QString filepath = mediaList.at(i).filepath;
        if(mediaList.at(i).mediaType == 3)
            filepath = mediaList.at(i).webUrl;
        for(int j=0;j< mediaList.at(i).streamList.size();++j) {
            streamquery.bindValue(":FilePin",       mediaList.at(i).pin);
            streamquery.bindValue(":FilePath",      filepath);
            streamquery.bindValue(":ItemName",      mediaList.at(i).itemname);
            streamquery.bindValue(":MediaType",     mediaList.at(i).mediaType);
            streamquery.bindValue(":Streamindex",   mediaList.at(i).streamList.at(j).streamId);
            streamquery.bindValue(":Streamtype",    mediaList.at(i).streamList.at(j).streamType);
            streamquery.bindValue(":filesize",      mediaList.at(i).streamList.at(j).fileSize);
            streamquery.bindValue(":Bitrate",       mediaList.at(i).streamList.at(j).bitRate);
            streamquery.bindValue(":ext",           mediaList.at(i).streamList.at(j).ext);
            streamquery.bindValue(":Codec",         mediaList.at(i).streamList.at(j).codec);
            streamquery.bindValue(":Language",      mediaList.at(i).streamList.at(j).language);
            streamquery.bindValue(":Description",   mediaList.at(i).streamList.at(j).description);
            streamquery.bindValue(":AspectRatio",   mediaList.at(i).streamList.at(j).aspectRatio);
            streamquery.bindValue(":Width",         mediaList.at(i).streamList.at(j).resolution.width());
            streamquery.bindValue(":Height",        mediaList.at(i).streamList.at(j).resolution.height());
            streamquery.bindValue(":RateDen",       mediaList.at(i).streamList.at(j).rateDen);
            streamquery.bindValue(":RateNum",       mediaList.at(i).streamList.at(j).rateNum);
            streamquery.bindValue(":Channel",       mediaList.at(i).streamList.at(j).Channel);
            streamquery.bindValue(":Rate",          mediaList.at(i).streamList.at(j).rate);
            streamquery.bindValue(":Encoding",      mediaList.at(i).streamList.at(j).Encoding);
            streamquery.exec();
        }
    }

    db_sqlite.commit();
}

bool SqlOperation::insertmediatosql(const MEDIAINFO &mediainfo)
{
    QSqlDatabase    db_sqlite = QSqlDatabase::database();
    QSqlQuery   query("", db_sqlite);
    QSqlQuery   streamquery("", db_sqlite);
    query.prepare("insert into \
        library(itemname,title,ext,filepath,fileSoure,type,webUrl,subtitle,videoinfojson,formatID,favorites,album,artist,\
                pin,rotate,duration,filesize,fileAddTime,screenshot)\
        VALUES (:itemname,:title,:ext,:filepath,:fileSoure,:type,:webUrl,:Subtitle,:videoinfojson,:formatID,:favorites,:album,:artist,\
                :pin,:rotate,:duration,:filesize,:fileAddTime,:screenshot)");

    streamquery.prepare("insert into \
        streamTabel(FilePin,FilePath,ItemName,MediaType,Streamindex,Streamtype,filesize,Bitrate,ext,Codec,Language,Description,\
                    AspectRatio,Width,Height,RateDen,RateNum,Channel,Rate,Encoding)\
        VALUES (:FilePin,:FilePath,:ItemName,:MediaType,:Streamindex,:Streamtype,:filesize,:Bitrate,:ext,:Codec,:Language,:Description,\
                :AspectRatio,:Width,:Height,:RateDen,:RateNum,:Channel,:Rate,:Encoding)");
    query.bindValue(":itemname",        mediainfo.itemname);   //在绑定要插入的值
    query.bindValue(":title",           mediainfo.title);
    query.bindValue(":ext",             mediainfo.ext);
    query.bindValue(":filepath",        mediainfo.filepath);
    query.bindValue(":fileSoure",       mediainfo.fileSoure);
    query.bindValue(":type",            mediainfo.mediaType);
    query.bindValue(":webUrl",          mediainfo.webUrl);
    query.bindValue(":Subtitle",        ListToStr(mediainfo.subtitleList));
    query.bindValue(":videoinfojson",   mediainfo.videoinfojson);
    query.bindValue(":formatID",        mediainfo.formatID);
    query.bindValue(":favorites",       mediainfo.isFavorites);
    query.bindValue(":album",           mediainfo.album);
    query.bindValue(":artist",          mediainfo.artist);
    query.bindValue(":duration",        mediainfo.duration);
    query.bindValue(":rotate",          mediainfo.rotate);
    query.bindValue(":filesize",        mediainfo.filesize);
    query.bindValue(":fileAddTime",     mediainfo.fileAddTime);
    query.bindValue(":pin",             mediainfo.pin);
    query.bindValue(":screenshot",      mediainfo.screenshot);

    QString filepath = mediainfo.filepath;
    if(mediainfo.mediaType == 3)
        filepath = mediainfo.webUrl;
    for(int j=0;j< mediainfo.streamList.size();++j) {
        streamquery.bindValue(":FilePin",       mediainfo.pin);
        streamquery.bindValue(":FilePath",      filepath);
        streamquery.bindValue(":ItemName",      mediainfo.itemname);
        streamquery.bindValue(":MediaType",     mediainfo.mediaType);
        streamquery.bindValue(":Streamindex",   mediainfo.streamList.at(j).streamId);
        streamquery.bindValue(":Streamtype",    mediainfo.streamList.at(j).streamType);
        streamquery.bindValue(":filesize",      mediainfo.streamList.at(j).fileSize);
        streamquery.bindValue(":Bitrate",       mediainfo.streamList.at(j).bitRate);
        streamquery.bindValue(":ext",           mediainfo.streamList.at(j).ext);
        streamquery.bindValue(":Codec",         mediainfo.streamList.at(j).codec);
        streamquery.bindValue(":Language",      mediainfo.streamList.at(j).language);
        streamquery.bindValue(":Description",   mediainfo.streamList.at(j).description);
        streamquery.bindValue(":AspectRatio",   mediainfo.streamList.at(j).aspectRatio);
        streamquery.bindValue(":Width",         mediainfo.streamList.at(j).resolution.width());
        streamquery.bindValue(":Height",        mediainfo.streamList.at(j).resolution.height());
        streamquery.bindValue(":RateDen",       mediainfo.streamList.at(j).rateDen);
        streamquery.bindValue(":RateNum",       mediainfo.streamList.at(j).rateNum);
        streamquery.bindValue(":Channel",       mediainfo.streamList.at(j).Channel);
        streamquery.bindValue(":Rate",          mediainfo.streamList.at(j).rate);
        streamquery.bindValue(":Encoding",      mediainfo.streamList.at(j).Encoding);
        streamquery.exec();
    }
    return query.exec();
}

void SqlOperation::updatamediatosql(const QList<MEDIAINFO> &mediaList, const MEDIAINFO &mediainfo)
{
    QList<MEDIAINFO> install;
    QList<MEDIAINFO> delList;
    for(int i=0;i<mediaList.size();++i) {
        if(QString::compare(mediaList.at(i).filepath,mediainfo.filepath,Qt::CaseInsensitive) == 0) {
            install.append(mediaList.at(i));
        }
    }
    delList.append(mediainfo);
    // 删除数据库中的路径和media相同的数据
    sqlDelforfilePath(delList);
    // 插入数据
    insertmediaListtosql(install);
}

void SqlOperation::updataUrlmediatoSql(const QList<MEDIAINFO> &delList, const QList<MEDIAINFO> &mediaList)
{
    // 只有在解析列表时才会更新，
    if(delList.size() > 0) {
        if(sqlDelUrlList(delList)) {
            insertmediaListtosql(mediaList);
        }
    } else {
        insertmediaListtosql(mediaList);
    }
}

bool SqlOperation::upMediaFavorites(const MEDIAINFO &mediainfo)
{
    bool res = false;
    QSqlQuery query;
    query.prepare("update library set favorites = :favorites where filepath = :filepath COLLATE NOCASE");
    query.bindValue(":filepath",    mediainfo.filepath);
    query.bindValue(":favorites",   mediainfo.isFavorites);
    if(query.exec()) {
        qDebug() << "更新成功";
        res = true;
    }
    return res;
}

void SqlOperation::upUrlMediaSub(const MEDIAINFO &mediainfo)
{
    QSqlQuery query;
    query.prepare("update library set subtitle = :Subtitle where webUrl = :WebUrl COLLATE NOCASE");
    query.bindValue(":Subtitle", ListToStr(mediainfo.subtitleList));
    query.bindValue(":WebUrl",   mediainfo.webUrl);
    if(query.exec()) {
        qDebug() << "更新成功";
    }
}

bool SqlOperation::addItemtoSql(const QString &itemName)
{
    bool res = false;
    QSqlQuery query;
    query.prepare("insert into itemlist(itemname) VALUES(:itemname);");
    query.bindValue(":itemname", itemName);   //在绑定要插入的值
    if(query.exec()){
        res = true;
        qDebug("插入数据成功...........");
    }
    return res;
}

bool SqlOperation::delItemtoSql(const QString &itemName)
{
    bool res = false;
    QSqlQuery query;
    query.prepare("delete from itemlist where itemname = :itemname");
    query.bindValue(":itemname", itemName);
    if(query.exec()) {
        res = true;
    }
    return res;
}

void SqlOperation::sqlGetMusicMediaList(QList<MEDIAINFO> *musicList)
{
    QSqlQuery query;
    query.prepare(QString("select * from library where type = :MediaType"));
    query.bindValue(":MediaType", 2);
    query.exec();
    while(query.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        MEDIAINFO videoinfo;
        videoinfo.itemname              = query.value("itemname").toString();
        videoinfo.title                 = query.value("title").toString();
        videoinfo.ext                   = query.value("ext").toString();
        videoinfo.filepath              = query.value("filepath").toString();
        videoinfo.fileSoure             = query.value("fileSoure").toString();
        videoinfo.mediaType             = query.value("type").toInt();
        videoinfo.webUrl                = query.value("webUrl").toString();
        videoinfo.subtitleList          = subToList(query.value("subtitle").toString());
        videoinfo.videoinfojson         = query.value("videoinfojson").toString();
        videoinfo.formatID              = query.value("formatID").toString();
        videoinfo.isFavorites           = query.value("favorites").toInt();
        videoinfo.album                 = query.value("album").toString();
        videoinfo.artist                = query.value("artist").toString();
        videoinfo.rotate                = query.value("rotate").toInt();
        videoinfo.duration              = query.value("duration").toInt();
        videoinfo.filesize              = query.value("filesize").toDouble();
        videoinfo.fileAddTime           = query.value("fileAddTime").toInt();
        videoinfo.pin                   = query.value("pin").toString();
        videoinfo.screenshot            = query.value("screenshot").toByteArray();
        QString filepath = videoinfo.filepath;
        QSqlQuery streamquery;
        streamquery.prepare(QString("select * from streamTabel where FilePin = :Pin and FilePath = :FilePaht and ItemName = :itemName"));
        streamquery.bindValue(":Pin",       videoinfo.pin);
        streamquery.bindValue(":FilePaht",  filepath);
        streamquery.bindValue(":itemName",  videoinfo.itemname);
        streamquery.exec();
        while(streamquery.next())
        {
            STREAMINFO streaminfo;
            streaminfo.streamId     = streamquery.value("Streamindex").toString();
            streaminfo.streamType   = streamquery.value("Streamtype").toInt();
            streaminfo.bitRate      = streamquery.value("Bitrate").toInt();
            streaminfo.fileSize     = streamquery.value("filesize").toInt();
            streaminfo.ext          = streamquery.value("ext").toString();
            streaminfo.codec        = streamquery.value("Codec").toString();
            streaminfo.language     = streamquery.value("Language").toString();
            streaminfo.description  = streamquery.value("Description").toString();
            streaminfo.aspectRatio  = streamquery.value("AspectRatio").toString();
            streaminfo.resolution.setWidth(streamquery.value("Width").toInt());
            streaminfo.resolution.setHeight(streamquery.value("Height").toInt());
            streaminfo.rateDen      = streamquery.value("RateDen").toInt();
            streaminfo.rateNum      = streamquery.value("RateNum").toInt();
            streaminfo.Channel      = streamquery.value("Channel").toInt();
            streaminfo.Encoding     = streamquery.value("Encoding").toString();
            videoinfo.streamList.append(streaminfo);
        }
        musicList->prepend(videoinfo);
    }
}

void SqlOperation::initLiveUrl(QStringList *LiveList, const int &urlType)
{
    QSqlQuery query;
    query.exec(QString("select * from liveUrl"));
    while(query.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        QString livePath    = query.value("LivePath").toString();
        int     liveType    = query.value("LiveType").toInt();
        if(liveType == urlType)
            LiveList->prepend(livePath);
    }
}

void SqlOperation::removeLiveUrl(const QString &liveurl)
{
    QSqlQuery query;
    query.prepare("delete from liveUrl where LivePath = :LivePath");
    query.bindValue(":LivePath", liveurl);
    if(query.exec()) {
        qDebug("liveUrl数据删除成功...........");
    }
}

void SqlOperation::addLiveurlToSql(const QString &liveurl, const int &urlType)
{
    QSqlQuery query;
    query.prepare("insert into liveUrl(LivePath,LiveType) VALUES(:LivePath,:LiveType);");
    query.bindValue(":LivePath", liveurl);   //在绑定要插入的值
    query.bindValue(":LiveType", urlType);   // 1 手动输入   2 收藏
    if(query.exec()){
        qDebug("插入数据成功...........");
    }
}

void SqlOperation::initRecent(QStringList *recentList)
{
    QSqlQuery query;
    query.exec(QString("select * from Recent"));
    while(query.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        QString Path    = query.value("RecentPath").toString();
        recentList->prepend(Path);
    }
}

bool SqlOperation::removeRecent(const QString &recentPath)
{
    QSqlQuery query;
    query.prepare("delete from Recent where RecentPath = :RecentPath COLLATE NOCASE");
    query.bindValue(":RecentPath", recentPath);
    return query.exec();
}

void SqlOperation::addRecent(const QString &recentPath)
{
    QSqlQuery query;
    query.prepare("insert into Recent(RecentPath) VALUES(:RecentPath);");
    query.bindValue(":RecentPath", recentPath);   //在绑定要插入的值
    if(query.exec()){
        qDebug("插入数据成功...........");
    }
}

void SqlOperation::clearRecent()
{
    QSqlQuery query;
    query.prepare("delete from Recent;");
    if(query.exec()){
        qDebug("清空数据成功...........");
    }
}

QStringList SqlOperation::subToList(const QString &sub)
{
    return sub.split(",", QString::SkipEmptyParts);

    QStringList subTitleList;
    subTitleList.clear();
    QStringList subList = sub.split(",");
    foreach (QString sub, subList) {
        if(!sub.isEmpty()){
            subTitleList.append(sub);
        }
    }
    return subTitleList;
}

QString SqlOperation::ListToStr(const QStringList &subList)
{
    return subList.join(",");
}

bool SqlOperation::sqlDelforMediaType(int mediaType)
{
    bool res = false;
    QSqlQuery query;
    query.prepare("delete from library where type = :type");
    query.bindValue(":type", mediaType);
    if(query.exec()) {
        res = true;
        qDebug() << "删除成功";
    }
    query.prepare("delete from streamTabel where MediaType = :MediaType");
    query.bindValue(":MediaType", mediaType);
    if(query.exec()) {
        res = true;
        qDebug() << "删除成功";
    }
    return res;
}

bool SqlOperation::sqlDelforItemName(QString itemName)
{
    bool res = false;
    QSqlQuery query;
    query.prepare("delete from library where itemname = :itemname");
    query.bindValue(":itemname", itemName);
    if(query.exec()) {
        res = true;
        qDebug() << "删除成功";
    }
    query.prepare("delete from streamTabel where ItemName = :ItemName");
    query.bindValue(":ItemName", itemName);
    if(query.exec()) {
        res = true;
        qDebug() << "删除成功";
    }
    return res;
}

bool SqlOperation::sqlDelforfilePath(QList<MEDIAINFO> mediadata)
{
    QSqlDatabase    db_sqlite = QSqlDatabase::database();

    QSqlQuery   query("", db_sqlite);
    bool    bsuccess = true;

    // 开始启动事务
    db_sqlite.transaction();
    for(int i = 0; i<mediadata.size(); ++i)
    {
        query.prepare("delete from library where filepath = :filepath COLLATE NOCASE");
        query.bindValue(":filepath", mediadata.at(i).filepath);
        if(!query.exec()) {
            bsuccess = false;
        }

        query.prepare("delete from streamTabel where FilePath = :FilePath COLLATE NOCASE");
        query.bindValue(":FilePath", mediadata.at(i).filepath);
        if(!query.exec()) {
            bsuccess = false;
        }
    }
    db_sqlite.commit();
    return bsuccess;
}

bool SqlOperation::sqlDelforfile(QList<MEDIAINFO> mediadata)
{
    QSqlDatabase    db_sqlite = QSqlDatabase::database();

    QSqlQuery   query("", db_sqlite);
    bool    bsuccess = false;

    // 开始启动事务
    db_sqlite.transaction();
    for(int i = 0; i<mediadata.size(); ++i)
    {
        query.prepare("delete from library where filepath = :filepath and itemname = :itemname COLLATE NOCASE");
        query.bindValue(":filepath", mediadata.at(i).filepath);
        query.bindValue(":itemname", mediadata.at(i).itemname);
        if(query.exec()) {
            qDebug() << "删除成功";
        }
        query.prepare("delete from streamTabel where FilePath = :FilePath and ItemName = :ItemName COLLATE NOCASE");
        query.bindValue(":FilePath", mediadata.at(i).filepath);
        query.bindValue(":ItemName", mediadata.at(i).itemname);
        if(query.exec()) {
            qDebug() << "删除成功";
        }
    }
    db_sqlite.commit();
    return bsuccess;
}

bool SqlOperation::sqlDelUrlList(QList<MEDIAINFO> mediadata)
{
    QSqlDatabase    db_sqlite = QSqlDatabase::database();

    QSqlQuery   query("", db_sqlite);
    bool    bsuccess = true;

    // 开始启动事务
    db_sqlite.transaction();
    for(int i = 0; i<mediadata.size(); ++i)
    {
        query.prepare("delete from library where webUrl = :webUrl");
        query.bindValue(":webUrl", mediadata.at(i).webUrl);
        if(!query.exec()) {
            bsuccess = false;
        }

        query.prepare("delete from streamTabel where FilePath = :FilePath");
        query.bindValue(":FilePath", mediadata.at(i).webUrl);
        if(!query.exec()) {
            bsuccess = false;
        }
    }
    db_sqlite.commit();
    return bsuccess;
}

bool SqlOperation::sqlDelforUrlMedia(MEDIAINFO mediadata)
{
    bool queryRet = true;
    QSqlDatabase    db_sqlite = QSqlDatabase::database();

    QSqlQuery   query("", db_sqlite);
    query.prepare("delete from library where webUrl = :webUrl");
    query.bindValue(":webUrl", mediadata.webUrl);
    if(!query.exec()) {
        queryRet = false;
    }
    query.prepare("delete from streamTabel where FilePath = :FilePath");
    query.bindValue(":FilePath", mediadata.webUrl);
    if(!query.exec()) {
        queryRet = false;
    }
    return queryRet;
}
