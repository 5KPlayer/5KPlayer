#include "YtbAnalysisThread.h"
#include <QProcess>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QPixmap>
#include <QJsonDocument>
#include <QBuffer>
#include <QtDebug>
#include <QEventLoop>
#include <QTimer>
#include "globalarg.h"

YtbAnalysisThread::YtbAnalysisThread(const AnalysisArgs &args, QObject *parent) : YtbThread(parent)
{
    qRegisterMetaType<QList<MEDIAINFO>>("QList<MEDIAINFO>");
    m_args = args;
}

YtbAnalysisThread::~YtbAnalysisThread()
{
    qDebug("YtbAnalysisThread del");
}

void YtbAnalysisThread::threadRun()
{
    QProcess pro;
    QStringList args;

    m_json.clear();
    if(m_args.https.isEmpty()) {
        args << "-J" << m_args.url;
    } else {
        args << "--proxy" << m_args.https << "-J" << m_args.url;
    }

    pro.start(m_args.appPath, args);

    while(pro.state() != QProcess::NotRunning)
    {
        pro.waitForFinished(500);
        QByteArray outPut = pro.readAllStandardOutput();
        if(!outPut.isEmpty())
            m_json.append(outPut);
        if(b_isStopped)  {
            pro.close();
            break;
        }
    }

    if(!b_isStopped)
    {
        removeInvalidByte();
        QByteArray outPut;
        for(int i=0;i<m_json.size();i++) {
            if(!m_json.at(i).isEmpty()) {
                outPut = m_json.at(i);
                break;
            }
        }
        if(pro.exitCode() == 0 || !outPut.isEmpty())
        {
            emit analysisFinish(m_args.url,analysisJsonByte(outPut));
            return;
        } else {
            emit analysisFinish(m_args.url,analysisJsonByte(outPut));
        }
    }
}

QList<MEDIAINFO> YtbAnalysisThread::analysisJsonByte(const QByteArray &jsonByte)
{
    QList<MEDIAINFO> mediaList;
    mediaList.clear();
    QJsonParseError error;
    QJsonDocument::fromJson(jsonByte, &error);
    if(error.error != QJsonParseError::NoError) {
        return mediaList;
    }
    QJsonObject json_object = QJsonDocument::fromJson(jsonByte).object();
    //这里需要根据json的extracotr_key  == Youtube 来分辨视频来源
    if(json_object.value("_type").toString() == "playlist") {
        QJsonArray jsonEntries   = json_object.value("entries").toArray();
        for(int i=0; i< jsonEntries.size();++i) {
            MEDIAINFO mediainfo = analysisJsonObj(jsonEntries.at(i).toObject());
            if(!mediainfo.title.isEmpty())
                mediaList.append(mediainfo);
        }
    } else {
        MEDIAINFO mediainfo = analysisJsonObj(json_object);
        if(!mediainfo.title.isEmpty())
            mediaList.append(mediainfo);
    }
    return mediaList;
}

MEDIAINFO YtbAnalysisThread::analysisJsonObj(const QJsonObject &jsonObj)
{
    MEDIAINFO mediainfo;
    if(jsonObj.value("title").isNull())
        return mediainfo;
    QString thumbnail;
    QJsonArray jsonformat   = jsonObj.value("formats").toArray();
    mediainfo.mediaType     = 3;
    mediainfo.title         = jsonObj.value("title").toString().simplified();
    mediainfo.ext           = jsonObj.value("ext").toString();
    mediainfo.formatID      = jsonObj.value("format_id").toString(); // 这里需要根据download设置的格式和分辨率来设置当前显示的format；
    mediainfo.webUrl        = jsonObj.value("webpage_url").toString();
    mediainfo.itemname      = "YouTuBe";
    thumbnail               = jsonObj.value("thumbnail").toString();
    mediainfo.filepath      = m_args.url;
    QSize scale;
    for(int i=0;i<jsonformat.size();++i) {
        _StreamInfo formatinfo;
        // 只加入视频相关信息
        formatinfo.streamId     = jsonformat.at(i).toObject().value("format_id").toString();
        formatinfo.ext          = jsonformat.at(i).toObject().value("ext").toString();
        formatinfo.bitRate      = jsonformat.at(i).toObject().value("fps").toInt();
        formatinfo.codec        = jsonformat.at(i).toObject().value("acodec").toString();
        //formatinfo.format_note  = jsonformat.at(i).toObject().value("format_note").toString();
        formatinfo.fileSize     = jsonformat.at(i).toObject().value("filesize").toInt();
        int height  = jsonformat.at(i).toObject().value("height").toInt();
        int width   = jsonformat.at(i).toObject().value("width").toInt();
        if(jsonformat.at(i).toObject().value("format_note").toString()== "tiny" ||
          (height == 0 && width == 0))
            formatinfo.streamType = 1;
        else
            formatinfo.streamType = 2;
        formatinfo.resolution.setHeight(height);
        formatinfo.resolution.setWidth(width);
        if(formatinfo.streamType == 2 && scale.width() < formatinfo.resolution.width())
            scale = formatinfo.resolution;
        mediainfo.streamList.append(formatinfo);
    }
    mediainfo.screenshot    = getPicture(thumbnail);
    return mediainfo;
}

QByteArray YtbAnalysisThread::getPicture(const QString &szUrl)
{
    QUrl url(szUrl);
    QEventLoop loop;
    QNetworkAccessManager manager;
    QTimer time;
    if(Global->openProxy()) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        QString proxyText = Global->proxyUrl();
        QStringList proxyList = proxyText.split(":");
        if(proxyList.size() == 3) {
            QString proxyUrl = proxyList.at(1);
            proxyUrl.replace("/","");
            int proxyId = proxyList.at(2).toInt();
            proxy.setHostName(proxyUrl);
            proxy.setPort(proxyId);

            manager.setProxy(proxy);
        }
    }

    QNetworkReply *reply = manager.get(QNetworkRequest(url));

    QObject::connect(&time,SIGNAL(timeout()),&loop,SLOT(quit()));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    time.start(3000);
    loop.exec();

    QByteArray pixData = reply->readAll();

    QPixmap pixmap;
    int imageH;
    QByteArray ba;
    if(pixmap.loadFromData(pixData)) {
        imageH = 300*pixmap.height()/pixmap.width();
        QBuffer buf(&ba);
        buf.open(QIODevice::ReadWrite);

        pixmap.scaled(300,imageH,Qt::IgnoreAspectRatio, Qt::SmoothTransformation).save(&buf,"png");
    }
    return ba;
}

void YtbAnalysisThread::removeInvalidByte()
{
    for(int i=0;i<m_json.size();i++) {
        QByteArray &array = m_json[i];
        while (array.length() > 0 && array.right(1) != "}") {
            array.remove(array.length() - 1, 1);
        }
    }
}
