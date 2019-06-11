#include "qhttpparser.h"
#include "./3rd/http_parser.h"

QHttpParser::QHttpParser(QObject *parent) : QObject(parent)
{
    m_parser = (http_parser *)malloc(sizeof(http_parser));

    m_parserSettings = new http_parser_settings();
    m_parserSettings->on_message_begin = MessageBegin;
    m_parserSettings->on_url = MyUrl;
    m_parserSettings->on_header_field = HeaderField;
    m_parserSettings->on_header_value = HeaderValue;
    m_parserSettings->on_headers_complete = HeadersComplete;
    m_parserSettings->on_body = Body;
    m_parserSettings->on_status = nullptr;
    m_parserSettings->on_message_complete = MessageComplete;

    m_parser->data = this;
}

QHttpParser::~QHttpParser()
{
    free(m_parser);
    m_parser = 0;

    delete m_parserSettings;
    m_parserSettings = 0;
}

bool QHttpParser::parserRequest(const QByteArray &buf)
{
    http_parser_init(m_parser, HTTP_REQUEST); //初始化为Request类型
    http_parser_execute(m_parser, m_parserSettings, buf, buf.size());  //执行解析过程
    return m_headersFinish;
}

bool QHttpParser::parserResponse(const QByteArray &buf)
{
    http_parser_init(m_parser, HTTP_RESPONSE); //初始化为Response类型
    http_parser_execute(m_parser, m_parserSettings, buf, buf.size());  //执行解析过程
    return m_messageFinish;
}

QByteArray QHttpParser::headerValue(const QByteArray &headerName) const
{
    return m_currentHeaders.value(headerName.toLower(), "");
}

QString QHttpParser::httpVersion() const
{
    return QString("%1.%2")
            .arg(m_parser->http_major)
            .arg(m_parser->http_minor);
}

int QHttpParser::statusCode() const
{
    return m_parser->status_code;
}

//----------------------回调开始----------------------//
int QHttpParser::MessageBegin(http_parser *parser)
{
    QHttpParser *h = static_cast<QHttpParser *>(parser->data);

    h->m_body.clear();
    h->m_method.clear();
    h->m_currentUrl.clear();
    h->m_currentHeaders.clear();
    h->m_currentHeaderField.clear();

    h->m_headersFinish = false;
    h->m_messageFinish = false;

    //获取 method
    h->m_method = http_method_str(http_method(parser->method));
    return 0;
}

int QHttpParser::MyUrl(http_parser *parser, const char *at, size_t length)
{
    QHttpParser *h = static_cast<QHttpParser *>(parser->data);
    h->m_currentUrl = QByteArray(at, length);
    return 0;
}

int QHttpParser::HeaderField(http_parser *parser, const char *at, size_t length)
{
    QHttpParser *h = static_cast<QHttpParser *>(parser->data);

    h->m_currentHeaderField = QByteArray(at, length).toLower();
    h->m_currentHeaders[h->m_currentHeaderField] = "";
    return 0;
}

int QHttpParser::HeaderValue(http_parser *parser, const char *at, size_t length)
{
    QHttpParser *h = static_cast<QHttpParser *>(parser->data);
    h->m_currentHeaders[h->m_currentHeaderField] = QByteArray(at, length);
    return 0;
}

int QHttpParser::HeadersComplete(http_parser *parser)
{
    QHttpParser *h = static_cast<QHttpParser *>(parser->data);
    h->m_headersFinish = true;
    return 0;
}

int QHttpParser::Body(http_parser *parser, const char *at, size_t length)
{
    QHttpParser *h = static_cast<QHttpParser *>(parser->data);
    h->m_body = QByteArray(at, length);
    return 0;
}

int QHttpParser::MessageComplete(http_parser *parser) //完成
{
    QHttpParser *h = static_cast<QHttpParser *>(parser->data);
    h->m_messageFinish = true;
    return 0;
}
//----------------------回调结束----------------------//
