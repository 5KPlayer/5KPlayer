#include "analyzewidget.h"
#include <QClipboard>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QFileInfo>
#include "librarydata.h"
#include "openfile.h"
#include "playermainwidget.h"
#include "globalarg.h"

AnalyzeWidget::AnalyzeWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void AnalyzeWidget::updateLanguage()
{
    InitLanguage();
    upUI();
}

void AnalyzeWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _analyzebtn->updateUI(_scaleRatio);
    upUI();
}

void AnalyzeWidget::Init()
{
    _scaleRatio     = 1.0;
    _pastebtn       = new DYLabel(this);
    _analyzebtn     = new SvgButton(this);
    _message        = new QLabel(this);
    _edit           = new QLineEdit(this);
    _progrress      = new QProgressBar(this);
    _ytbBtn         = new DYLabel(this);
    _moreBtn        = new DYLabel(this);
    _progrress->setTextVisible(false);

    _moreBtn->setAlignment(Qt::AlignCenter);
    _moreBtn->setStyleSheet("Color:rgb(60,160,212)");
    _moreBtn->setMouseHead(true);
    _ytbBtn->setStyleSheet("Color:rgb(60,160,212)");
    _ytbBtn->setAlignment(Qt::AlignCenter);
    _ytbBtn->setMouseHead(true);
    _ytbBtn->setText("YouTube");
    _moreBtn->setText(Lge->getLangageValue("YTBAnalyze/more"));
    _pastebtn->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _pastebtn->setStyleSheet("border-image: url(:/res/png/Paste_nalyze_nor.png);Color:rgb(255,255,255)");
    _message->setStyleSheet("border-image:url(:/res/png/not_video_url.png);Color:rgb(145,60,3)");
    _message->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    _message->hide();
    _analyzebtn->initSize(27,20);
    _analyzebtn->setImagePrefixName("analyze");
    setprogressSty(true);

    _edit->setStyleSheet("background-color: rgba(255, 255, 255,0.1);");
    _edit->setAlignment(Qt::AlignVCenter);
    _edit->setContextMenuPolicy(Qt::NoContextMenu);
    _edit->setFrame(false);
    _edit->raise();

    connect(_analyzebtn,SIGNAL(clicked()),SLOT(analyzeClick()));
    connect(_pastebtn,SIGNAL(clicked()),SLOT(analyAndPaste()));
    connect(_edit,SIGNAL(textChanged(QString)),SLOT(editChange(QString)));
    connect(_ytbBtn,SIGNAL(clicked()),SLOT(clickYoutube()));
    connect(_moreBtn,SIGNAL(clicked()),SLOT(clickMore()));

    upUI();
    InitLanguage();
}

void AnalyzeWidget::upUI()
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    double distanceW = 20*_scaleRatio;
    double distanceH = 5*_scaleRatio;
    int editH = 20*_scaleRatio;
    int pastebtnH = 25*_scaleRatio;
    font.setPixelSize(pastebtnH/2);
    _pastebtn->setFont(font);
    double pasteWidth = _pastebtn->fontMetrics().width(_pastebtn->text());
    _pastebtn->setGeometry(distanceW,distanceH,pasteWidth+pasteWidth/5,pastebtnH);
    _pastebtn->setContentsMargins(0,0,2*_scaleRatio,0);
    _analyzebtn->setGeometry(this->width()-_analyzebtn->width()-distanceW,distanceH*2+_pastebtn->height(),_analyzebtn->width(),_analyzebtn->height());

    _ytbBtn->setGeometry(_pastebtn->x()+_pastebtn->width()+distanceW,_pastebtn->y()+_pastebtn->height()-18*_scaleRatio,65*_scaleRatio,18*_scaleRatio);
    _moreBtn->setGeometry(_ytbBtn->x()+_ytbBtn->width()+distanceW,_ytbBtn->y(),50*_scaleRatio,18*_scaleRatio);

    font.setPixelSize(editH*3/5);
    _edit->setFont(font);
    _edit->setGeometry(distanceW,distanceH*2+_pastebtn->height(),this->width()-distanceW-60*_scaleRatio,editH);
    _progrress->setGeometry(distanceW,distanceH*2+_pastebtn->height(),this->width()-distanceW-60*_scaleRatio,editH);

    font.setPixelSize(_message->height()/2);
    _message->setFont(font);
    _message->setMargin(3*_scaleRatio);
    int w = _message->fontMetrics().width(_message->text())+10*_scaleRatio;
    _message->setGeometry(250*_scaleRatio,(_progrress->y()-24*_scaleRatio)/2,w,24*_scaleRatio);
    font.setUnderline(true);
    _ytbBtn->setFont(font);
    _moreBtn->setFont(font);
}

bool AnalyzeWidget::analyzepath()
{
    if(_edit->text().isEmpty()){
        _message->setText(_UrlEmpty);
        _message->setObjectName("empty");
        int w = _message->fontMetrics().width(_message->text())+10*_scaleRatio;
        _message->resize(w,_message->height());
        setprogressSty(false);
        _message->raise();
        _message->show();
        return false;
    }
    if((_edit->text().size() > 8) && ((_edit->text().left(7).toUpper() == "HTTP://") || (_edit->text().left(8).toUpper() == "HTTPS://"))) {
        return true;
    } else {
        _message->setText(_UrlInvalid);
        _message->setObjectName("invalid");
        int w = _message->fontMetrics().width(_message->text())+10*_scaleRatio;
        _message->resize(w,_message->height());
        setprogressSty(false);
        _message->raise();
        _message->show();
        return false;
    }
}

void AnalyzeWidget::setprogressSty(bool isInval)
{
    if(isInval) {
        _progrress->setStyleSheet("QProgressBar {"
                                  "border: 1px solid  #b3e6f5;"
                                  "background-color: #FFFFFF;"
                               "}"
                               "QProgressBar::chunk {"
                                  "background-color: #b3e6f5;"
                               "}");
    } else {
        _progrress->setStyleSheet("QProgressBar {"
                                  "border: 1px solid  #FF0000;"
                                  "background-color: #FFFFFF;"
                               "}"
                               "QProgressBar::chunk {"
                                  "background-color: #b3e6f5;"
                               "}");
    }
}

void AnalyzeWidget::analyzeClick()
{
    pathanalyze();
}

void AnalyzeWidget::analyAndPaste()
{
    QClipboard *board = QApplication::clipboard();
    _edit->setText(board->text());
    pathanalyze();
}

void AnalyzeWidget::pathanalyze()
{
    if(!analyzepath()) {
        return;
    }
    if(!LibData->checkYtbDeploy()) {
        int res = OpenFileDialog->ytbDeploy();
        if(res == 0)
            return;
    }

    emit addUrlAnalyze(_edit->text());
}

void AnalyzeWidget::editChange(QString text)
{
    Q_UNUSED(text);
    setprogressSty(true);
    _message->hide();
}

void AnalyzeWidget::clickYoutube()
{
    QString url = "https://www.youtube.com/";
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

void AnalyzeWidget::clickMore()
{
    Global->openLanguageUrl("more");
}

void AnalyzeWidget::InitLanguage()
{
    _UrlEmpty = Lge->getLangageValue("YTBAnalyze/urlEmpty");
    _UrlInvalid=Lge->getLangageValue("YTBAnalyze/urlInvalid");
    _moreBtn->setText(Lge->getLangageValue("YTBAnalyze/more"));
    _pastebtn->setText(Lge->getLangageValue("YTBAnalyze/pasteBtn"));
    if(!_message->isHidden()) {
        if(_message->objectName() == "empty")
            _message->setText(_UrlEmpty);
        else
            _message->setText(_UrlInvalid);
    }
}

void AnalyzeWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
