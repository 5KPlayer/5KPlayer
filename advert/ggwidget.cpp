#include "ggwidget.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QTimer>
#include <QPalette>
#include <QBitmap>
#include <QBuffer>
#include <QLabel>
#include <QDesktopServices>
#include <QPropertyAnimation>
#include <QApplication>
#include <QFileInfo>
#include <QMovie>
#include <QDir>
#include <QScreen>
#include <Windows.h>

#include "dycheckbox.h"
#include "dylabel.h"
#include "svgbutton.h"
#include "gglabel.h"
#include "globalarg.h"
#include "playermainwidget.h"
#include "httprequest.h"

GGWidget::GGWidget(QWidget *parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow|Qt::WindowStaysOnTopHint);
    setWindowState(Qt::WindowActive);
    qsrand(QDateTime::currentMSecsSinceEpoch());
    init();
}

void GGWidget::updateLanguage()
{

}

void GGWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
//    _closeBtn->updateUI(_scaleRatio);
//    _noshow->updateUI(_scaleRatio);
    upUI();
}

void GGWidget::clearWidget()
{
    for(int i=0;i<10;i++) {
        _hosts[i]->setMouseHead(false);
        _hosts[i]->clear();
        _hosts[i]->setPixMapPath("","");
        _hosts[i]->setVisible(false);
        _hosts[i]->disconnect();
    }
    for(int i=0;i<10;i++) {
        _texts[i]->setMouseHead(false);
        _texts[i]->clear();
        _texts[i]->setPixMapPath("","");
        _texts[i]->setVisible(false);
        _texts[i]->disconnect();

    }
    AdvertStruct ggInfo;
    _advertInfo  = ggInfo;
    _ico->icoMoveClear();
    _ico->setMouseHead(false);
    _ico->setGeometry(0,0,0,0);
    _ico->disconnect();
}

void GGWidget::startShow(int number, QString adType)
{
    int googleRate = 1;
    if(Global->getGoogleRate() > 0)
        googleRate = 100/Global->getGoogleRate();
    int rand = qrand()%100;
    _googleSap = (rand % googleRate) == 0;

    if(_adShowState)
        return;

    if(haveFullScreen() || PMW->isFullScreen())
        return;

    _scaleRatio = Global->_screenRatio;

    // 安装过软件就不弹
    bool isPop;
    if(adType == "Video") {
        isPop = haveConvert();
    } else if(adType == "iPhone") {
        isPop = haveiPhoneManager();
    } else if(adType == "DVD"){
        isPop = haveDVDRipper();
    } else {
        isPop = false;
    }

    if(isPop)
        return;
    _curAdNumber = number;
    _curAdType   = adType;
    clearWidget();
    if(!loadFileByAdNumber(number))
        return;

    _adShowState = true;
    initGGWidget(_advertInfo);

    const QRect rect = PMW->curScreen()->availableGeometry();

    _showAdType = _advertInfo._adShowType;
    this->move(rect.width() - this->width() + rect.x(),
               rect.height() - this->height() + rect.y());
    this->setVisible(true);
    _adWidget->setVisible(false);

    if(_showAdType == "BtoTop") {
        _adWidget->move(0, this->height() + 20*_scaleRatio);
        _showAnimal->setStartValue(QPoint(0, _adWidget->y()));
        _showAnimal->setEndValue(QPoint(0,0));
    } else {
        _adWidget->move(this->width() + 20*_scaleRatio, 0);
        _showAnimal->setStartValue(QPoint(_adWidget->x(), 0));
        _showAnimal->setEndValue(QPoint(0,0));
    }
    _showAnimal->start();

    if(_advertInfo._adShowTime > 0) {
        _widgeShowTimer->setInterval(_advertInfo._adShowTime * 1000);
        _widgeShowTimer->start();
    }
    // 保存显示时间
    Global->saveShowAdsTime(QDateTime::currentDateTime());
    // 发送谷歌统计
    if(_googleSap) {
        const QString url = QString("https://ssl.google-analytics.com/collect?tid=UA-58082298-1"
                                    "&cid=%1&t=event&v=1&ec=win-5KPlayer&ea=ads_show&ev=0&el=%2")
                                    .arg(Global->getUuid()).arg(QString::number(_curAdNumber));
        Http->get(url);
    }
}

void GGWidget::init()
{
    _adShowState = false;
    _adWidget = new QWidget(this);
    _adWidget->setStyleSheet("background-color: rgb(51, 51, 71);");
    _showAnimal  = new QPropertyAnimation(this);
    _showAnimal->setPropertyName("pos");
    _showAnimal->setTargetObject(_adWidget);
    _showAnimal->setDuration(400);
    _widgeShowTimer  = new QTimer(this);
    _widgeClostTimer = new QTimer(this);
    _widgeClostTimer->setInterval(1 * 1000);
    connect(_widgeClostTimer,SIGNAL(timeout()),SLOT(closeTimeOut()));
    connect(_widgeShowTimer,SIGNAL(timeout()),SLOT(showTimeOut()));
    this->resize(300*_scaleRatio,200*_scaleRatio);
    _adWidget->resize(this->size());

    /*
     * ico
     */
    _ico    = new GGLabel(_adWidget);

    for(int i=0;i<10;i++) {
        _texts[i]   = new GGLabel(_adWidget);
        _texts[i]->setVisible(false);
        _texts[i]->setOpenExternalLinks(true);
    }

    for(int i=0;i<10;i++) {
       _hosts[i] = new GGLabel(_adWidget);
       _hosts[i]->setVisible(false);
    }

    _closeBtn = new SvgButton(_adWidget);
    _closeBtn->setImagePrefixName("close");
    _noshow = new DYCheckBox(_adWidget,"");

    connect(_showAnimal,SIGNAL(finished()),SLOT(animalFinish()));
    connect(_showAnimal,SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)),
            SLOT(animalstateChanged(QAbstractAnimation::State,QAbstractAnimation::State)));
    connect(_closeBtn,SIGNAL(clicked()),SLOT(clickCloseBtn()));
    connect(_noshow,SIGNAL(checkChange(bool)),SLOT(clickNoShow(bool)));
}

void GGWidget::upUI()
{
    _closeBtn->updateUI(_scaleRatio);
    _noshow->updateUI(_scaleRatio);
    _noshow->move(10*_scaleRatio,_adWidget->height() - 24*_scaleRatio);
    _closeBtn->move(_adWidget->width() - _closeBtn->width(),0);
}

void GGWidget::initGGWidget(AdvertStruct ggInfo)
{
    if(!ggInfo._adBackColor.isEmpty()) {
        QString adback = QString("background-color: %1;").arg(ggInfo._adBackColor);
        _adWidget->setStyleSheet(adback);
    }

    if(ggInfo._adNoshowText.isEmpty()) {
        _noshow->setVisible(false);
    } else {
        QLabel label;
        _noshow->resize(label.fontMetrics().width(ggInfo._adNoshowText) + 20*_scaleRatio,24*_scaleRatio);
        _noshow->setVisible(true);
        _noshow->setTextColor(ggInfo._adNoshowColor);
        _noshow->setText(ggInfo._adNoshowText);
    }
    if(ggInfo._adWinSize.width() != 0 && ggInfo._adWinSize.height() != 0) {
        this->resize(ggInfo._adWinSize.width()*_scaleRatio,ggInfo._adWinSize.height()*_scaleRatio);
        _adWidget->resize(this->size());
    }

    //　初始化关闭按钮大小
    _closeBtn->initSize(26,22);
    // 初始化图片
    if(ggInfo._adBackIco._icoSize.width() != 0) {
        _ico->setGeometry(ggInfo._adBackIco._icoPos.x()*_scaleRatio,ggInfo._adBackIco._icoPos.y()*_scaleRatio,
                          ggInfo._adBackIco._icoSize.width()*_scaleRatio,ggInfo._adBackIco._icoSize.height()*_scaleRatio);
    }
    if(!ggInfo._adBackIco._fileName.isEmpty()) {
        _ico->setPixMapPath(ggInfo._adBackIco._fileName,"");
        if(!ggInfo._adBackIco._icoUrl.isEmpty()) {
            _ico->setMouseHead(true);
            _ico->setGGData(ggInfo._adBackIco._icoUrl);
            connect(_ico,SIGNAL(clicked()),SLOT(openUrl()));
        }
    } else {
        _ico->setPixMapPath("","");
    }


    // 初始化热点
    for(int i=0;i<ggInfo._adHotspots.count();i++) {
        if(i > 9)
            break;
        _hosts[i]->setVisible(true);
        if(!ggInfo._adHotspots.at(i)->_hotspotUrl.isEmpty()) {
            _hosts[i]->setMouseHead(true);
            _hosts[i]->setGGData(ggInfo._adHotspots.at(i)->_hotspotUrl);
            connect(_hosts[i],SIGNAL(clicked()),SLOT(openUrl()));
        }
        _hosts[i]->setGeometry(ggInfo._adHotspots.at(i)->_hotRect.x()*_scaleRatio,
                               ggInfo._adHotspots.at(i)->_hotRect.y()*_scaleRatio,
                               ggInfo._adHotspots.at(i)->_hotRect.width()*_scaleRatio,
                               ggInfo._adHotspots.at(i)->_hotRect.height()*_scaleRatio);
        if(!ggInfo._adHotspots.at(i)->_hotspotNorRes.isEmpty() || !ggInfo._adHotspots.at(i)->_hotspotEnRes.isEmpty()) {
            _hosts[i]->setPixMapPath(ggInfo._adHotspots.at(i)->_hotspotNorRes,ggInfo._adHotspots.at(i)->_hotspotEnRes);
        }
        _hosts[i]->raise();
    }
    // 初始化文字
    for(int i=0;i<ggInfo._adTexts.count();i++) {
        if(i > 9)
            break;
        _texts[i]->setVisible(true);
        if(!ggInfo._adTexts.at(i)._textUrl.isEmpty()) {
            _texts[i]->setMouseHead(true);
            _texts[i]->setGGData(ggInfo._adTexts.at(i)._textUrl);
            connect(_texts[i],SIGNAL(clicked()),SLOT(openUrl()));
        }
        _texts[i]->setFontColor(ggInfo._adTexts.at(i)._textColor);
        _texts[i]->setAlignment(Qt::AlignTop|Qt::AlignLeft);
        _texts[i]->setOpenExternalLinks(true);
        _texts[i]->setText(ggInfo._adTexts.at(i)._text);
        _texts[i]->setFont(ggInfo._adTexts.at(i)._font);
        _texts[i]->setStyleSheet(QString("background-color:transparent; color: %1")
                             .arg(ggInfo._adTexts.at(i)._textColor));


        int height = _texts[i]->fontMetrics().height()*_scaleRatio;
        int width = _texts[i]->fontMetrics().width(ggInfo._adTexts.at(i)._text)*_scaleRatio;

        _texts[i]->setGeometry(ggInfo._adTexts.at(i)._textPos.x()*_scaleRatio,
                       ggInfo._adTexts.at(i)._textPos.y()*_scaleRatio
                               ,width,height);

        _texts[i]->raise();
    }

    // 初始化不再显示
    _noshow->setChecked(false);
    _noshow->raise();
    _closeBtn->setVisible(!ggInfo._adCloseHide);
    _closeBtn->raise();
    upUI();
}

bool GGWidget::loadFileByAdNumber(int adNumber)
{
    bool loadFinish = false;
    QString adText = QString::number(adNumber);
    QString loadFolder = Global->adFloderPath() + "/"  + Global->getLanguage() + "/" + adText;
    QString loadFile = loadFolder + "/" + adText + ".json";
    QFile file(loadFile);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray jsonByte = QByteArray::fromBase64(file.readAll());
        _advertInfo = readJsonFile(jsonByte,loadFolder);
        loadFinish = true;
    }
    return loadFinish;
}

AdvertStruct GGWidget::readJsonFile(QByteArray json, QString loadFolder)
{
    AdvertStruct jsonInfo;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &error);
    if(error.error == QJsonParseError::NoError) {
        QJsonObject json_object = jsonDoc.object();
        jsonInfo._adBackColor   = json_object.value("ad_background_color").toString();
        jsonInfo._adNoshowColor = json_object.value("ad_noshow_color").toString();
        jsonInfo._adNoshowText  = json_object.value("ad_noshow_text").toString();
        jsonInfo._adWinSize.setHeight(json_object.value("ad_height").toInt());
        jsonInfo._adWinSize.setWidth(json_object.value("ad_width").toInt());
        jsonInfo._adSizeAuto    = json_object.value("ad_size_auto").toBool();

        jsonInfo._adShowTime    = json_object.value("ad_showtime").toInt();
        jsonInfo._adWinKey      = json_object.value("ad_winkey").toString();
        jsonInfo._adShowType    = json_object.value("ad_show_type").toString();
        jsonInfo._adCloseHide   = json_object.value("ad_close_hide").toBool();

        // 图片信息
        QJsonObject png_object;
        png_object = json_object.value("picture").toObject();
        jsonInfo._adBackIco._icoUrl     = png_object.value("ad_pic_url").toString();
        jsonInfo._adBackIco._icoPos.setX(png_object.value("ad_pic_x").toInt());
        jsonInfo._adBackIco._icoPos.setY(png_object.value("ad_pic_y").toInt());
        jsonInfo._adBackIco._icoSize.setHeight(png_object.value("ad_pic_h").toInt());
        jsonInfo._adBackIco._icoSize.setWidth(png_object.value("ad_pic_w").toInt());

        QString icoName = png_object.value("ad_pic_name").toString();
        if(!icoName.isEmpty()) {
            jsonInfo._adBackIco._fileName   = loadFolder + "/" + icoName;
        }

        // 文字信息
        QJsonArray textList_object;
        textList_object = json_object.value("textList").toArray();
        for(int i=0;i<textList_object.count();i++) {
            TextStruct ggtext;
            ggtext._textGuid    = textList_object.at(i).toObject().value("ad_txt_guid").toString();
            ggtext._text        = textList_object.at(i).toObject().value("ad_txt_text").toString();
            ggtext._textColor   = textList_object.at(i).toObject().value("ad_txt_color").toString();
            ggtext._textUrl     = textList_object.at(i).toObject().value("ad_txt_url").toString();
            ggtext._textPos.setX(textList_object.at(i).toObject().value("ad_txt_posx").toInt());
            ggtext._textPos.setY(textList_object.at(i).toObject().value("ad_txt_posy").toInt());
            // 文字字体
            ggtext._font.setBold(textList_object.at(i).toObject().value("ad_txt_font_bold").toBool());
            ggtext._font.setItalic(textList_object.at(i).toObject().value("ad_txt_font_italic").toBool());
            ggtext._font.setStrikeOut(textList_object.at(i).toObject().value("ad_txt_font_strikeOut").toBool());
            ggtext._font.setUnderline(textList_object.at(i).toObject().value("ad_txt_font_underline").toBool());
            ggtext._font.setPointSize(textList_object.at(i).toObject().value("ad_txt_font_pointSize").toInt());
            ggtext._font.setFamily(textList_object.at(i).toObject().value("ad_txt_font_family").toString());
            jsonInfo._adTexts.append(ggtext);
        }


        // 热点
        QJsonArray hotList_object;
        hotList_object = json_object.value("hotspotList").toArray();
        for(int i=0;i<hotList_object.count();i++) {
            HotspotStruct* hotSpot = new HotspotStruct;
            int x = hotList_object.at(i).toObject().value("ad_hot_x").toInt();
            int y = hotList_object.at(i).toObject().value("ad_hot_y").toInt();
            int w = hotList_object.at(i).toObject().value("ad_hot_w").toInt();
            int h = hotList_object.at(i).toObject().value("ad_hot_h").toInt();
            hotSpot->_hotRect   = QRect(x,y,w,h);
            hotSpot->_hotGuid   = hotList_object.at(i).toObject().value("ad_hot_Guid").toString();
            hotSpot->_hotspotUrl    = hotList_object.at(i).toObject().value("ad_hot_url").toString();
            QString enRes = hotList_object.at(i).toObject().value("ad_hot_res_en").toString();
            if(!enRes.isEmpty()) {
                hotSpot->_hotspotEnRes = loadFolder + "/" + enRes;
            }
            QString norRes = hotList_object.at(i).toObject().value("ad_hot_res_nor").toString();
            if(!norRes.isEmpty()) {
                hotSpot->_hotspotNorRes = loadFolder + "/" + norRes;
            }
            jsonInfo._adHotspots.append(hotSpot);
        }
    }
    return jsonInfo;
}

void GGWidget::adWidgetclose()
{
    if(_showAdType == "BtoTop") {
        _showAnimal->setStartValue(QPoint(0,0));
        _showAnimal->setEndValue(QPoint(0, this->height() + 20*_scaleRatio));
    } else {
        _showAnimal->setStartValue(QPoint(0,0));
        _showAnimal->setEndValue(QPoint(this->width() + 20*_scaleRatio,0));
    }
    _showAnimal->start();
}

bool GGWidget::haveConvert()
{
    static bool isCheck = false;//是否检测存在
    static bool isExist = false; //
    if(!isCheck)
    {
        isCheck = true;
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(path);
        dir.cdUp();
        QStringList paths = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString &subPath, paths)
        {
            if(dir.cd(subPath + "/WinX HD Video Converter Deluxe"))
            {
                isExist = true;
                break;
            }
        }
    }
    return isExist;
}

bool GGWidget::haveiPhoneManager()
{
    static bool isCheck = false;//是否检测存在
    static bool isExist = false; //
    if(!isCheck)
    {
        isCheck = true;
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(path);
        dir.cdUp();
        QStringList paths = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString &subPath, paths)
        {
            if(dir.cd(subPath + "/iPhoneMusicManager"))
            {
                isExist = true;
                break;
            }
        }
    }
    return isExist;
}

bool GGWidget::haveDVDRipper()
{
    static bool isCheck = false;//是否检测存在
    static bool isExist = false; //
    if(!isCheck)
    {
        isCheck = true;
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(path);
        dir.cdUp();
        QStringList paths = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString &subPath, paths)
        {
            if(dir.cd(subPath + "/WinX DVD Ripper Platinum"))
            {
                isExist = true;
                break;
            }
        }
    }
    return isExist;
}

bool GGWidget::haveFullScreen()
{
    bool isFull = false;
    QScreen *screen = PMW->curScreen();
    QRect deskRect = screen->geometry();

    POINT point;
    point.x = deskRect.x() + deskRect.width() / 2;
    point.y = deskRect.y() + deskRect.height() / 2;
    HWND hand = WindowFromPoint(point);
    //HWND hand = GetForegroundWindow();
    if(hand != NULL) {
        RECT lpRect;
        if(GetWindowRect(hand, &lpRect)) {
            int width  = lpRect.right - lpRect.left;
            int height = lpRect.bottom - lpRect.top;

            if(deskRect.width() == width && deskRect.height() == height) {
                isFull = true;
            }
        }
    }

    return isFull;
}

void GGWidget::animalFinish()
{
    if(_showAdType == "BtoTop") {
        if(_showAnimal->startValue().toPoint().y() < _showAnimal->endValue().toPoint().y()){
            _adWidget->setVisible(false);
            this->setVisible(false);
            _adShowState = false;
            clearWidget();
        }
    } else {
        if(_showAnimal->startValue().toPoint().x() < _showAnimal->endValue().toPoint().x()){
            _adWidget->setVisible(false);
            this->setVisible(false);
            _adShowState = false;
            clearWidget();
        }
    }
}

void GGWidget::animalstateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    if(newState == QAbstractAnimation::Running) {
        _adWidget->setVisible(true);
    }
}

void GGWidget::showTimeOut()
{
    _widgeShowTimer->stop();
//    if(_googleSap) {
//        const QString url = QString("https://ssl.google-analytics.com/collect?tid=UA-58082298-1"
//                                    "&cid=%1&t=event&v=1&ec=win-5KPlayer&ea=ads_autoclose&ev=0&el=%2")
//                                    .arg(Global->getUuid()).arg(QString::number(_curAdNumber));
//        qDebug() << url;
//        Http->get(url);
//    }
    adWidgetclose();
}

void GGWidget::closeTimeOut()
{
    _widgeClostTimer->stop();
    adWidgetclose();
}

void GGWidget::clickCloseBtn()
{
    _widgeShowTimer->stop();
    _widgeClostTimer->stop();
    adWidgetclose();
//    if(_googleSap) {
//        const QString url = QString("https://ssl.google-analytics.com/collect?tid=UA-58082298-1"
//                                    "&cid=%1&t=event&v=1&ec=win-5KPlayer&ea=ads_close&ev=0&el=%2")
//                            .arg(Global->getUuid()).arg(QString::number(_curAdNumber));
//        qDebug() << url;
//        Http->get(url);
//    }
}

void GGWidget::clickNoShow(bool check)
{
    Global->addNoShowAdNumber(_curAdNumber);
    _noshow->setChecked(check);
    _widgeShowTimer->stop();
    _widgeClostTimer->stop();
    adWidgetclose();
//    if(_googleSap) {
//        const QString url = QString("https://ssl.google-analytics.com/collect?tid=UA-58082298-1"
//                                    "&cid=%1&t=event&v=1&ec=win-5KPlayer&ea=ads_donotshowagain&ev=0&el=%2")
//                                    .arg(Global->getUuid()).arg(QString::number(_curAdNumber));
//        qDebug() << url;
//        Http->get(url);
//    }
}

void GGWidget::openUrl()
{
    GGLabel *label = qobject_cast<GGLabel *>(this->sender());
    QString adUrl = label->getGGData();

    // 打开网页
    QDesktopServices::openUrl(QUrl(adUrl, QUrl::TolerantMode));

    _widgeShowTimer->stop();
    if(!_widgeClostTimer->isActive()) {
        _widgeClostTimer->start();
    }

    // 发送谷歌统计
    if(_googleSap) {
        const QString googleUrl = QString("https://ssl.google-analytics.com/collect?tid=UA-58082298-1"
                                    "&cid=%1&t=event&v=1&ec=win-5KPlayer&ea=ads_link&ev=0&el=%2")
                                    .arg(Global->getUuid()).arg(QString::number(_curAdNumber));
        Http->get(googleUrl);
    }

    //emit clickWinKey(_advertInfo._adWinKey);
}
