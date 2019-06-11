#ifndef SETTINGDOWNLOADWIDGET_H
#define SETTINGDOWNLOADWIDGET_H

#include "updateinterface.h"
#include <QMap>
class QLabel;
class ComboBox;
class QLineEdit;
class QCheckBox;
class SvgButton;
class DYCheckBox;
class SettingDownloadWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SettingDownloadWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);
    int  getDownPosForKey(QString str);
    QStringList getDownKeys();

protected:
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void DownLoadChange(const QString &downPath);
    void DownLoadSelect();
    void DownLoadOpen();
    void proxyCheckChange(bool check);
    void proxyTextChange(const QString &proxyText);

private:
    int _hMargin;

    QLabel* _title;
    QLabel* _titleLine;
    QLabel* _titleLine2;

    QLabel *_videoFormatLabel;
    ComboBox *_videoFormatComboBox;

    QLabel *_videoResolutionLabel;
    ComboBox *_videoResolutionComboBox;

    DYCheckBox*_proxyCheckBox;
    QLineEdit *_proxyEdit;

    QLabel*             _downloadFolderLabel;
    QLineEdit*          _downloadFolderEdit;
    SvgButton*          _downloadSelect;
    SvgButton*          _downloadOpen;
    double              _scaleRatio;

    QMap<QString,int> _downPosMap;
};

#endif // SETTINGDOWNLOADWIDGET_H
