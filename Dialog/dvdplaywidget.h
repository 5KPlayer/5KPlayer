#ifndef DVDPLAYWIDGET_H
#define DVDPLAYWIDGET_H

#include "updateinterface.h"

class QLabel;
class DYLabel;
class ComboBox;
class QLineEdit;
class DYDVDInfo;
class SvgButton;
class TextButton;
class DYCheckBox;

class DVDPlayWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit DVDPlayWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private slots:
    void playDVD();
    void cancel();
    void startFind();
    void endFind();

    void showDVDDisc();
    void showDVDImageFile();
    void showDVDScanDialog();

    void autoTypeChange(bool change);
    void udfTypeChange(bool change);
    void isoTypeChange(bool change);
protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    void initLabel(QLabel **label,const QString &str);
    void initDYLabel(DYLabel **label,const QString &str);

private:
    QLabel *_label1;
    QLabel *_label2;
    QLabel *_label2_1;
    QLabel *_label3;
    QLabel *_label4;
    QLabel *_label5;
    QLabel *_loadLabel;
    QLabel* _label4Ico;
    QLabel* _label5Ico;
    QLabel* _dvdPxiamap;
    QLabel* _dvdOpenType;

    DYLabel *_okBtn;
    DYLabel *_cancelBtn;
    DYLabel *_scanBtn;

    SvgButton *_downloadBtn;

    ComboBox *_combo;
    QLineEdit *_lineEdit;
    DYDVDInfo *_dvdInfo;

    TextButton *_DVD_DISC;
    TextButton *_DVD_Image;

    DYCheckBox *_autoType;
    DYCheckBox *_udfType;
    DYCheckBox *_isoType;

    int _space;
    int _adHeight;//广告高度
};

#endif // DVDPLAYWIDGET_H
