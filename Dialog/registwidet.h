#ifndef REGISTWIDET_H
#define REGISTWIDET_H

#include "updateinterface.h"

class QLabel;
class QMovie;
class DYLabel;
class QLineEdit;
class ComboBox;
class DYCheckBox;
class QNetworkReply;
class QPropertyAnimation;
class QParallelAnimationGroup;

class RegistWidet : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit RegistWidet(QWidget *parent = 0);
    ~RegistWidet();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

protected:
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void sendEmail();
    void sendEmailFinish();
    void registCode();

private:
    void initLabel(QLabel **label,const QString &str, QWidget *parent);
    void initDYLabel(DYLabel **label,const QString &str, QWidget *parent);
    void initLineEdit(QLineEdit **edit,const QString &str, QWidget *parent);

    void showErrorInfo(const int &direct=0, const QString &msg="");

    bool checkFirstWidget();
    int  checkSecondWidget();//-1不需要跳转  0跳转到前一页/1跳转到下一页
    void checkThirdWidget();

    void startAnimal(const int &endIndex);

private:
    qreal i_ratio;

    QLabel *p_infoLabel; //描述

    QWidget *p_firstWidget;//第一界面
    //国家
    QLabel *p_countryLabel;
    ComboBox *p_countryBox;
    QLineEdit *p_countryEdit;

    //名字
    QLabel *p_nameLabel;
    QLineEdit *p_nameEdit;

    //邮箱
    QLabel *p_addrLabel;
    QLineEdit *p_addrEdit;

    //确定邮箱
    QWidget *p_secondWidget;//第二界面
    QLabel  *p_confirmEmailLabel;
    QLineEdit *p_confirmEmailEdit;

    //验证码
    QWidget *p_thirdWidget;//第三界面
    QLabel *p_codeInfoLabel;
    QLabel *p_codeLabel;
    QLineEdit *p_codeEdit;

#if 0
    DYCheckBox *p_checkbox;
#endif
    //加载
    QLabel *p_loadgif;
    QMovie *p_movie;

    DYLabel *p_okBtn;
    DYLabel *p_cancelBtn;

    //错误提示
    QLabel *p_errorLabel;

    int i_curWIndex;

    QNetworkReply *p_reply;

    //动画
    QPropertyAnimation *p_startAnimal;
    QPropertyAnimation *p_endAnimal;
    QParallelAnimationGroup *p_animalGroup;
};

#endif // REGISTWIDET_H
