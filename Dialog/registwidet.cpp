#include "registwidet.h"
#include <QLabel>
#include "dylabel.h"
#include <QLineEdit>
#include "combobox.h"
#include "globalarg.h"
#include "dycheckbox.h"
#include "openfile.h"
#include "httprequest.h"
#include "cmenu.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include <QDebug>
#include <QMovie>

extern "C"
{
#include "digiarty_utilities.h"
}

#define ASTERISK "<font color=\"#FF0000\">*</font>"


RegistWidet::RegistWidet(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    p_reply = NULL;
    i_curWIndex = 1;
    //初始化 this的界面
    const QString str = Lge->getLangageValue("Register/label");

    initLabel(&p_infoLabel,    str,  this);
    initDYLabel(&p_okBtn,  Lge->getLangageValue("Register/next"), this);// ok  Activate
    initDYLabel(&p_cancelBtn, Lge->getLangageValue("Register/notReg"), this);
    p_okBtn->setBackClolr("rgb(0,140,220)","rgb(51,191,242)","rgb(0,175,239)");


#if 0
    p_checkbox = new DYCheckBox(this, Lge->getLangageValue("Register/send5k"));
    p_checkbox->setChecked(true);
    p_checkbox->setCheckEnable(false);
#endif


    //初始化第一界面
    p_firstWidget  = new QWidget(this);
    initLabel(&p_countryLabel, Lge->getLangageValue("Register/country")+ASTERISK, p_firstWidget);
    initLabel(&p_nameLabel,    Lge->getLangageValue("Register/name")+ASTERISK, p_firstWidget);
    initLabel(&p_addrLabel,    Lge->getLangageValue("Register/email")+ASTERISK, p_firstWidget);

    initLineEdit(&p_countryEdit, "", p_firstWidget);
    initLineEdit(&p_nameEdit, Lge->getLangageValue("Register/exapName"), p_firstWidget);
    initLineEdit(&p_addrEdit, "email@example.com", p_firstWidget);
    p_countryEdit->setVisible(false);

    p_countryBox = new ComboBox(p_firstWidget);
    QStringList list;
    list = Lge->getLangageValue("Register/countryList").split("|");
    p_countryBox->addItems(list);

    if(Global->getLanguage() == "jp") {
        p_countryBox->setCurrentIndex(5);
    }

    //初始化第二界面
    p_secondWidget = new QWidget(this);
    initLabel(&p_confirmEmailLabel,   Lge->getLangageValue("Register/conEmail")+ASTERISK, p_secondWidget);
    initLineEdit(&p_confirmEmailEdit, "email@example.com",     p_secondWidget);

    p_loadgif = new QLabel(p_secondWidget);
    p_movie = new QMovie(":/res/loading.gif");
    p_loadgif->setMovie(p_movie);
    //p_loadgif->setVisible(false);

    //初始化第三界面
    p_thirdWidget  = new QWidget(this);
    initLabel(&p_codeInfoLabel,Lge->getLangageValue("Register/checkCode"), p_thirdWidget);
    initLabel(&p_codeLabel,    Lge->getLangageValue("Register/enterReg")+ASTERISK, p_thirdWidget);
    initLineEdit(&p_codeEdit,  "xx-xxxxxxx-xxxxxxx",                   p_thirdWidget);


    //错误提示label
    p_errorLabel = new QLabel(this);
    p_errorLabel->setVisible(false);
    p_errorLabel->setStyleSheet("border-image:url(:/res/png/not_supperted.png);Color:rgb(145,60,3)");
    p_errorLabel->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

    connect(p_okBtn, SIGNAL(clicked()), SLOT(registCode()));
    connect(p_cancelBtn, &DYLabel::clicked, [](){
        Global->setOpenLogin(CGlobal::General_Login::Login_No);
        OpenFileDialog->closeDialog();
    });

    connect(p_countryBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [=](int index){
        if(index == p_countryBox->count()-1)
            p_countryEdit->setVisible(true);
        else
            p_countryEdit->setVisible(false);

    });

    p_startAnimal = new QPropertyAnimation;
    p_endAnimal   = new QPropertyAnimation;
    p_animalGroup = new QParallelAnimationGroup(this);

    p_startAnimal->setDuration(200);
    p_startAnimal->setPropertyName("geometry");

    p_endAnimal->setDuration(200);
    p_endAnimal->setPropertyName("geometry");

    p_animalGroup->addAnimation(p_startAnimal);
    p_animalGroup->addAnimation(p_endAnimal);

    i_ratio = 1.0;
    updateUI(1.0);
}

RegistWidet::~RegistWidet()
{
    Http->stopRequest(p_reply);
}

void RegistWidet::updateUI(const qreal &scaleRatio)
{
    i_ratio = scaleRatio;

    p_firstWidget->resize(440 * scaleRatio, 184 * scaleRatio);
    p_secondWidget->resize(p_firstWidget->size());
    p_thirdWidget->resize(p_firstWidget->size());

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);
    font.setBold(false);

    p_infoLabel->setFont(font);
    p_infoLabel->adjustSize();

    p_countryLabel->setFont(font);
    p_countryLabel->adjustSize();

    p_nameLabel->setFont(font);
    p_nameLabel->adjustSize();

    p_addrLabel->setFont(font);
    p_addrLabel->adjustSize();

    p_codeInfoLabel->setFont(font);
    p_codeInfoLabel->adjustSize();

    p_codeLabel->setFont(font);
    p_codeLabel->adjustSize();

    p_countryBox->resize(200 * scaleRatio, 24 * scaleRatio);
    p_countryBox->setFont(font);
    p_countryBox->upStyleSheet(scaleRatio);

    p_confirmEmailLabel->setFont(font);
    p_confirmEmailLabel->adjustSize();
    p_confirmEmailEdit->resize(200 * scaleRatio, 24 * scaleRatio);
    p_confirmEmailEdit->setFont(font);
    p_confirmEmailEdit->setStyleSheet(Global->lineEditStyleSheet(4 * scaleRatio));

    p_countryEdit->resize(120 * scaleRatio, 24 * scaleRatio);
    p_countryEdit->setFont(font);
    p_countryEdit->setStyleSheet(Global->lineEditStyleSheet(4 * scaleRatio));

    p_nameEdit->resize(200 * scaleRatio, 24 * scaleRatio);
    p_nameEdit->setFont(font);
    p_nameEdit->setStyleSheet(Global->lineEditStyleSheet(4 * scaleRatio));

    p_addrEdit->resize(200 * scaleRatio, 24 * scaleRatio);
    p_addrEdit->setFont(font);
    p_addrEdit->setStyleSheet(Global->lineEditStyleSheet(4 * scaleRatio));

    p_codeEdit->resize(200 * scaleRatio, 24 * scaleRatio);
    p_codeEdit->setFont(font);
    p_codeEdit->setStyleSheet(Global->lineEditStyleSheet(4 * scaleRatio));

    p_loadgif->resize(24 * scaleRatio, 24 * scaleRatio);
    p_movie->setScaledSize(p_loadgif->size());

    p_okBtn->updateUI(scaleRatio);
    p_okBtn->setFont(font);
    p_okBtn->resize(124 * scaleRatio, 24 * scaleRatio);

    p_cancelBtn->updateUI(scaleRatio);
    p_cancelBtn->setFont(font);
    p_cancelBtn->resize(384 * scaleRatio, 24 * scaleRatio);

#if 0
    p_checkbox->updateUI(scaleRatio);
    p_checkbox->setFont(font);
    p_checkbox->resize(288 * scaleRatio, 24 * scaleRatio);
#endif

    font.setPixelSize(10 * scaleRatio);
    p_errorLabel->setFont(font);
    p_errorLabel->resize(168 * scaleRatio, 20 * scaleRatio);
}

void RegistWidet::resizeEvent(QResizeEvent *)
{
    const int margin = 24 * i_ratio;
    const int w = this->width();
    const int h = this->height();

    p_infoLabel->move(margin/3, margin/6);

    const int cy = p_infoLabel->y() + p_infoLabel->height();
    if(i_curWIndex == 1) {
        p_firstWidget->move(0,   cy);
        p_secondWidget->move(w,  cy);
        p_thirdWidget->move(w,   cy);
    } else if(i_curWIndex == 2) {
        p_firstWidget->move(-w,  cy);
        p_secondWidget->move(0,  cy);
        p_thirdWidget->move(w,   cy);
    } else if(i_curWIndex == 3) {
        p_firstWidget->move(-w,  cy);
        p_secondWidget->move(-w, cy);
        p_thirdWidget->move(0,   cy);
    }

    //第一界面
    p_countryEdit->move(w-p_countryEdit->width()-margin/2, margin*3/2);
    p_countryBox->move(p_countryEdit->x() - p_countryBox->width()-margin/4, p_countryEdit->y());
    p_countryLabel->move(p_infoLabel->x(),
                         p_countryEdit->y() + (p_countryEdit->height()-p_countryLabel->height())/2);

    p_nameEdit->move(p_countryBox->x(), p_countryEdit->y()+p_countryEdit->height()+margin);
    p_nameLabel->move(p_infoLabel->x(),
                      p_nameEdit->y()+(p_nameEdit->height()-p_nameLabel->height())/2);

    p_addrEdit->move(p_countryBox->x(), p_nameEdit->y()+p_nameEdit->height()+margin);
    p_addrLabel->move(p_infoLabel->x(),
                      p_addrEdit->y()+(p_addrEdit->height()-p_addrLabel->height())/2);

    //第二界面
    p_confirmEmailLabel->move(p_nameLabel->pos());
    p_confirmEmailEdit->move(p_nameEdit->pos());
    p_loadgif->move(p_confirmEmailEdit->x() + p_confirmEmailEdit->width() + margin/4,
                    p_confirmEmailEdit->y());

    //第三界面
    p_codeInfoLabel->move(p_countryLabel->pos());
    p_codeLabel->move(p_nameLabel->pos());
    p_codeEdit->move(p_codeLabel->x() + p_codeLabel->width() + margin/4,
                     p_nameEdit->y());

#if 0 //主界面
    p_checkbox->move(p_infoLabel->x(), p_firstWidget->y() + p_firstWidget->height());

    p_okBtn->move((w-p_okBtn->width())/2, p_checkbox->y() + p_checkbox->height()+margin/2);
#endif

    p_okBtn->move((w-p_okBtn->width())/2, p_firstWidget->y() + p_firstWidget->height() +
                                          p_okBtn->height()+margin/2);

    p_cancelBtn->move((w-p_cancelBtn->width())/2, h-p_cancelBtn->height()-margin/2);

    //校正error label位置
}

void RegistWidet::sendEmail()
{
    p_errorLabel->setVisible(false);
    p_movie->start();
    p_loadgif->setVisible(true);

    const QString name = p_nameEdit->text().trimmed();
    const QString email = p_addrEdit->text().trimmed();
    QString country;
    if(p_countryBox->currentIndex() == p_countryBox->count()-1)
        country = p_countryEdit->text().trimmed();
    else
        country = p_countryBox->currentText().trimmed();

    const QString str=QString("{\"software\":\"5kplayer\",\"name\":\"%1\","
                              "\"email\":\"%2\",\"os\":\"windows\","
                              "\"lang\":\"%3\",\"country\":\"%4\",\"status\":1}")
            .arg(name).arg(email).arg(Global->getLanguage()).arg(country);

    String* json_content = string_create_with_cstring(str.toUtf8().constData());
    String* target_text = digiarty_encode_by_AES_and_base64(json_content);
    RELEASE_STRING(json_content);

    String* send_cmd_prefix = string_create_with_cstring("digiarty=");
    String* data_to_send = string_by_append_string(send_cmd_prefix, target_text);
    String* final_send = string_by_encode_to_url(data_to_send);

    const QByteArray array = QByteArray(string_get_cstring(final_send));
    RELEASE_STRING(target_text);
    RELEASE_STRING(send_cmd_prefix);
    RELEASE_STRING(data_to_send);
    RELEASE_STRING(final_send);

    //http请求
    QNetworkRequest req;
    req.setUrl(QUrl("http://www.5kplayer.com/common/info.php"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    p_reply = Http->post(req, array);
    connect(p_reply, SIGNAL(finished()), SLOT(sendEmailFinish()));
}

void RegistWidet::sendEmailFinish()
{
    p_movie->stop();
    p_loadgif->setVisible(false);
    if(p_reply->error() == QNetworkReply::NoError) {
        //跳转到第三界面
        i_curWIndex = 3;
        p_okBtn->setText(Lge->getLangageValue("Register/activate"));
        startAnimal(3);
    } else {
        showErrorInfo(4, Lge->getLangageValue("Register/netErr"));//显示错误提示
    }
}

void RegistWidet::registCode()
{
    //隐藏提示栏
    p_errorLabel->setVisible(false);

    if(i_curWIndex == 1) {
        if(checkFirstWidget()) {
            //开启动画 跳转到第二界面
            i_curWIndex = 2;
            p_okBtn->setText("Ok");
            startAnimal(2);
        }
    } else if(i_curWIndex == 2) {
        const int rt = checkSecondWidget();
        if(rt == 0) {
            p_addrEdit->setText("");
            p_confirmEmailEdit->setText("");
            // 跳转到第一界面
            i_curWIndex = 1;
            p_okBtn->setText(Lge->getLangageValue("Register/next"));
            startAnimal(1);
        } else if(rt == 1) {
            // 发送邮件 发送成功跳转到第三界面
            sendEmail();
        }
    } else if(i_curWIndex == 3) {
        checkThirdWidget();
    }
}

void RegistWidet::initLabel(QLabel **label, const QString &str, QWidget *parent)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12);
    font.setBold(false);

    (*label) = new QLabel(str, parent);
    (*label)->setFont(font);
    (*label)->setPalette(p1);
    (*label)->adjustSize();
}

void RegistWidet::initDYLabel(DYLabel **label, const QString &str, QWidget *parent)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12);
    font.setBold(false);

    (*label) = new DYLabel(parent, true);
    (*label)->setFont(font);
    (*label)->setText(str);
    (*label)->resize(80, 24);
    (*label)->setAlignment(Qt::AlignCenter);
}

void RegistWidet::initLineEdit(QLineEdit **edit, const QString &str, QWidget *parent)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12);
    font.setBold(false);

    (*edit) = new QLineEdit(parent);
    (*edit)->setContextMenuPolicy(Qt::NoContextMenu);
    (*edit)->setFont(font);
    (*edit)->setPlaceholderText(str);
    (*edit)->setStyleSheet(Global->lineEditStyleSheet(4));
}

void RegistWidet::showErrorInfo(const int &direct, const QString &msg)
{
    if(!msg.isEmpty())
        p_errorLabel->setText(msg.trimmed());

    int x=0;
    int y=0;
    QWidget *widget=NULL;

    switch (direct)
    {
    case 1://城市
        widget = p_firstWidget;
        x = p_countryBox->x() + (p_countryBox->width()-p_errorLabel->width())/2;
        y = p_countryBox->y()-p_errorLabel->height();
        break;
    case 2://名称
        widget = p_firstWidget;
        x = p_nameEdit->x() + (p_nameEdit->width()-p_errorLabel->width())/2;
        y = p_nameEdit->y()-p_errorLabel->height();
        break;
    case 3://邮箱
        widget = p_firstWidget;
        x = p_addrEdit->x() + (p_addrEdit->width()-p_errorLabel->width())/2;
        y = p_addrEdit->y()-p_errorLabel->height();
        break;
    case 4://确认邮箱
        widget = p_secondWidget;
        x = p_confirmEmailEdit->x() + (p_confirmEmailEdit->width()-p_errorLabel->width())/2;
        y = p_confirmEmailEdit->y()-p_errorLabel->height();
        break;
    case 5://注册码
        widget = p_thirdWidget;
        x = p_codeEdit->x() + (p_codeEdit->width()-p_errorLabel->width())/2;
        y = p_codeEdit->y()-p_errorLabel->height();
        break;
    }
    if(widget) {
        p_errorLabel->move(widget->x() + x,widget->y()+y);
        p_errorLabel->setVisible(true);
    }
}

bool RegistWidet::checkFirstWidget()
{
    //判断不能为空  和  格式是否正确
    QString country;
    if(p_countryBox->currentIndex() == p_countryBox->count()-1)
        country = p_countryEdit->text().trimmed();
    else
        country = p_countryBox->currentText().trimmed();

    if(country.isEmpty()) {
        showErrorInfo(1, Lge->getLangageValue("Register/counEmy"));//显示错误提示
        return false;
    }

    const QString name = p_nameEdit->text().trimmed();
    if(name.isEmpty()) {
        showErrorInfo(2, Lge->getLangageValue("Register/nameEmy"));//显示错误提示
        return false;
    }

    const QString email = p_addrEdit->text().trimmed();
    if(email.isEmpty()) {
        showErrorInfo(3, Lge->getLangageValue("Register/emailEmy"));//显示错误提示
        return false;
    }
    QRegExp ex("^\\w[-\\w.+]*@([A-Za-z0-9][-A-Za-z0-9]+\\.)+[A-Za-z]{2,14}$");
    if(!ex.exactMatch(email)) {
        showErrorInfo(3, Lge->getLangageValue("Register/emailErr"));//显示错误提示
        return false;
    }
    return true;
}

int RegistWidet::checkSecondWidget()
{
    const QString email = p_confirmEmailEdit->text().trimmed();
    if(email.isEmpty()) {
        showErrorInfo(4, Lge->getLangageValue("Register/emailEmy"));//显示错误提示
        return -1;
    }
    QRegExp ex("^\\w[-\\w.+]*@([A-Za-z0-9][-A-Za-z0-9]+\\.)+[A-Za-z]{2,14}$");
    if(!ex.exactMatch(email)) {
        showErrorInfo(4, Lge->getLangageValue("Register/emailErr"));//显示错误提示
        return -1;
    }
    //检查是否与第一遍输入相同
    if(email != p_addrEdit->text().trimmed())
        return 0;
    return 1;
}

void RegistWidet::checkThirdWidget()
{
    const QString code = p_codeEdit->text().trimmed();
    if(code.isEmpty()) {
        showErrorInfo(5, Lge->getLangageValue("Register/regEmy"));//显示错误提示
        return;
    }
    //检查校验码是否正确
    if(!Global->checkRegistCode(code)) {
        showErrorInfo(5, Lge->getLangageValue("Register/regErr"));//显示错误提示
        return;
    }

    Global->setRegistCode(code);
    //菜单里隐藏选项
    Menu->hideRegistMenu();

    //显示注册成功弹出框
    OpenFileDialog->openTooltipDialog(Lge->getLangageValue("Register/regSou"));

    //注册成功
    OpenFileDialog->closeDialog();
}

void RegistWidet::startAnimal(const int &endIndex) //2>1  1>2  2>3
{
    if(endIndex < 1 || endIndex > 3) return;

    QWidget *startW = NULL;
    QWidget *endW = NULL;
    QRect startWEndRect;
    QRect endWEndRect;
    if(endIndex == 1) {
        startW = p_secondWidget;
        endW   = p_firstWidget;

        startWEndRect = p_thirdWidget->geometry();
        endWEndRect   = startW->geometry();
    } else if(endIndex == 2) {
        startW = p_firstWidget;
        endW   = p_secondWidget;

        endWEndRect   = startW->geometry();
        startWEndRect = QRect(0-startW->width(),
                              startW->y(),
                              startW->width(),
                              startW->height());
    } else if(endIndex == 3) {
        startW = p_secondWidget;
        endW   = p_thirdWidget;

        startWEndRect = p_firstWidget->geometry();
        endWEndRect   = startW->geometry();
    }

    p_startAnimal->setTargetObject(startW);
    p_startAnimal->setStartValue(startW->geometry());
    p_startAnimal->setEndValue(startWEndRect);

    p_endAnimal->setTargetObject(endW);
    p_endAnimal->setStartValue(endW->geometry());
    p_endAnimal->setEndValue(endWEndRect);
    p_animalGroup->start();
}
