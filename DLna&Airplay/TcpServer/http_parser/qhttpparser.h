#ifndef QHTTPPARSER_H
#define QHTTPPARSER_H

#include <QHash>
#include <QObject>

typedef QHash<QByteArray, QByteArray> HeaderHash;

struct http_parser;
struct http_parser_settings;

class QHttpParser : public QObject
{
    Q_OBJECT
public:
    explicit QHttpParser(QObject *parent = 0);
    ~QHttpParser();

    bool parserRequest(const QByteArray &buf); //解析请求参数
    bool parserResponse(const QByteArray &buf);//解析返回参数

    QByteArray url()  const    {return m_currentUrl;}
    QByteArray body()  const   {return m_body;}
    QByteArray method() const  {return m_method;}
    HeaderHash headers() const {return m_currentHeaders;}
    QByteArray headerValue(const QByteArray &headerName) const;
    QString httpVersion() const;
    int statusCode() const;

private:
    static int MessageBegin(http_parser *parser);
    static int MyUrl(http_parser *parser, const char *at, size_t length);
    static int HeaderField(http_parser *parser, const char *at, size_t length);
    static int HeaderValue(http_parser *parser, const char *at, size_t length);
    static int HeadersComplete(http_parser *parser);
    static int Body(http_parser *parser, const char *at, size_t length);
    static int MessageComplete(http_parser *parser);

private:
    http_parser *m_parser;
    http_parser_settings *m_parserSettings;

    QByteArray m_body;
    QByteArray m_method;
    QByteArray m_currentUrl;
    HeaderHash m_currentHeaders;

    QByteArray m_currentHeaderField;

    bool m_headersFinish;
    bool m_messageFinish;
};

#endif // QHTTPPARSER_H
