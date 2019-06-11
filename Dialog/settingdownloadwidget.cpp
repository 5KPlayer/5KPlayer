#include "settingdownloadwidget.h"
#include <QtWidgets>
#include "combobox.h"
#include "globalarg.h"
#include "svgbutton.h"
#include "dycheckbox.h"
#include "openfile.h"

SettingDownloadWidget::SettingDownloadWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    _title      = new QLabel(Lge->getLangageValue("SetMain/down"),this);
    _title->setPalette(p1);
    _title->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    _titleLine  = new QLabel(this);
    _titleLine2 = new QLabel(this);
    _titleLine->setStyleSheet("background-color: rgb(20, 20, 21);");
    _titleLine2->setStyleSheet("background-color: rgb(71, 71, 75);");


    _videoFormatLabel = new QLabel(Lge->getLangageValue("SetDown/format"), this);
    _videoFormatLabel->setPalette(p1);
    _videoFormatLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _videoFormatComboBox = new ComboBox(this);
    for(int i=0;i<Global->_urlFormat.size();++i) {
        _videoFormatComboBox->addItem(Global->_urlFormat.at(i));
    }

    _videoResolutionLabel = new QLabel(Lge->getLangageValue("SetDown/resolut"), this);
    _videoResolutionLabel->setPalette(p1);
    _videoResolutionLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _videoResolutionComboBox = new ComboBox(this);
    for(int i=0;i<Global->_urlResolution.size();++i) {
        _videoResolutionComboBox->addItem(QString(Global->_urlResolution.at(i)).append("P"));
    }

    _proxyCheckBox = new DYCheckBox(this,Lge->getLangageValue("SetDown/proxy"));
    _proxyCheckBox->setPalette(p1);
    _proxyCheckBox->setRightText(true);
    _proxyCheckBox->setTextFontBlod(true);
    _proxyEdit = new QLineEdit(this);

    _downloadFolderLabel = new QLabel(Lge->getLangageValue("SetDown/downpath"), this);
    _downloadFolderLabel->setPalette(p1);
    _downloadFolderLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _downloadFolderEdit = new QLineEdit(this);
    _downloadSelect     = new SvgButton(this);
    _downloadOpen       = new SvgButton(this);

    _downloadSelect->initSize(20,20);
    _downloadOpen->initSize(20,20);

    _proxyEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _downloadFolderEdit->setContextMenuPolicy(Qt::NoContextMenu);
    QString path = Global->downloadPath();
    path.replace("/", "\\");
    _downloadFolderEdit->setText(path);
    _downloadFolderEdit->setCursorPosition(0);

    _videoFormatComboBox->setCurrentIndex(Global->downloadFormat());
    _videoResolutionComboBox->setCurrentIndex(Global->downloadResolution());

    _proxyCheckBox->setChecked(Global->openProxy());
    _proxyEdit->setText(Global->proxyUrl());

    _proxyEdit->setEnabled(_proxyCheckBox->isChecked());

    connect(_videoFormatComboBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [=](int index){
        Global->setDownloadFormat(CGlobal::Download_Format(index));
    });
    connect(_videoResolutionComboBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [=](int index){
        Global->setDownloadResolution(CGlobal::Download_Resolution(index));
    });
    connect(_proxyCheckBox,SIGNAL(checkChange(bool)),SLOT(proxyCheckChange(bool)));
    connect(_downloadFolderEdit,SIGNAL(textChanged(QString)),SLOT(DownLoadChange(QString)));
    connect(_downloadSelect,SIGNAL(clicked()),SLOT(DownLoadSelect()));
    connect(_downloadOpen,SIGNAL(clicked()),SLOT(DownLoadOpen()));
    connect(_proxyEdit,SIGNAL(textChanged(QString)),SLOT(proxyTextChange(QString)));

    _downloadSelect->setImagePrefixName("browse");
    _downloadOpen->setImagePrefixName("file_open");
    _scaleRatio = Global->_screenRatio;
}

void SettingDownloadWidget::updateUI(const qreal &scaleRatio)
{
    _downloadSelect->updateUI(scaleRatio);
    _downloadOpen->updateUI(scaleRatio);

    _downPosMap.clear();
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setBold(true);
    font.setPixelSize(12 * scaleRatio);

    _scaleRatio = scaleRatio;
    _hMargin = 32 * scaleRatio;
    int height = 20*scaleRatio;

    _videoFormatLabel->setFont(font);
    _videoFormatComboBox->setFont(font);
    _videoResolutionLabel->setFont(font);
    _videoResolutionComboBox->setFont(font);
    _proxyCheckBox->setFont(font);
    _proxyEdit->setFont(font);
    _downloadFolderLabel->setFont(font);
    _downloadFolderEdit->setFont(font);

    font.setPixelSize(14 * scaleRatio);
    _title->setFont(font);

    double xpos = 4*_scaleRatio;

    int titleW  = _title->fontMetrics().width(_title->text());
    _title->setGeometry(xpos,       5*_scaleRatio,titleW,height);
    int lineW   = this->width()- (_title->x() + _title->width() + 28*_scaleRatio);
    _titleLine->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,15*_scaleRatio,lineW, 1*_scaleRatio);
    _titleLine2->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,16*_scaleRatio,lineW, 1*_scaleRatio);

    _videoFormatLabel->setGeometry(xpos, 40* scaleRatio,210 * scaleRatio,height);
    _videoFormatComboBox->setGeometry(xpos+_videoFormatLabel->width() + _hMargin/4, _videoFormatLabel->y(),220 * scaleRatio, height);
    _downPosMap.insert(Lge->getLangageValue("SetMain/down") + ":" + LangNoColon("SetDown/format"),_videoFormatLabel->y());

    _videoResolutionLabel->setGeometry(xpos, 75* scaleRatio,210 * scaleRatio, height);
    _videoResolutionComboBox->setGeometry(_videoFormatComboBox->x(), 75* scaleRatio,220 * scaleRatio, height);
    _downPosMap.insert(Lge->getLangageValue("SetMain/down") + ":" + LangNoColon("SetDown/resolut"),75* scaleRatio);

    _proxyEdit->setGeometry(_videoFormatComboBox->x(), 120* scaleRatio,280 * scaleRatio, height);
    _proxyCheckBox->setGeometry(_proxyEdit->x() - 60 * scaleRatio - _hMargin/4, 120* scaleRatio,60 * scaleRatio, height);
    _downPosMap.insert(Lge->getLangageValue("SetMain/down") + ":" + LangNoColon("SetDown/proxy"),120* scaleRatio);

    _downloadFolderLabel->setGeometry(xpos, 155* scaleRatio,210 * scaleRatio, height);
    _downloadFolderEdit->setGeometry(_videoFormatComboBox->x(), _downloadFolderLabel->y(),280 * scaleRatio, height);
    _downloadSelect->move(_downloadFolderEdit->x()+_downloadFolderEdit->width()+8*scaleRatio,_downloadFolderLabel->y());
    _downloadOpen->move(_downloadSelect->x()+_downloadSelect->width()+4*scaleRatio,_downloadFolderLabel->y());
    _downPosMap.insert(Lge->getLangageValue("SetMain/down") + ":" + LangNoColon("SetDown/downpath"),_downloadFolderLabel->y());

    _videoFormatComboBox->upStyleSheet(scaleRatio);
    _videoResolutionComboBox->upStyleSheet(scaleRatio);
    _proxyEdit->setStyleSheet( Global->lineEditStyleSheet(4 * scaleRatio));
    _downloadFolderEdit->setStyleSheet( Global->lineEditStyleSheet(4 * scaleRatio));
    _proxyCheckBox->updateUI(scaleRatio);
}

int SettingDownloadWidget::getDownPosForKey(QString str)
{
    return _downPosMap.value(str);
}

QStringList SettingDownloadWidget::getDownKeys()
{
    return _downPosMap.keys();
}

void SettingDownloadWidget::resizeEvent(QResizeEvent *)
{
    updateUI(_scaleRatio);
}

void SettingDownloadWidget::DownLoadChange(const QString &downPath)
{
    Global->setDownloadPath(downPath);
    _downloadFolderEdit->setCursorPosition(0);
}

void SettingDownloadWidget::DownLoadSelect()
{
    QString downPath = _downloadFolderEdit->text();
    const QString filePath = OpenFileDialog->getDir(Lge->getLangageValue("Folder/download"), downPath);
    if(!filePath.isEmpty())
        _downloadFolderEdit->setText(filePath);
}

void SettingDownloadWidget::DownLoadOpen()
{
    if(!_downloadFolderEdit->text().isEmpty()) {
        QString open = "file:///";
        open.append(QDir::toNativeSeparators(_downloadFolderEdit->text()));
        QDesktopServices::openUrl(QUrl(open, QUrl::TolerantMode));
    }
}

void SettingDownloadWidget::proxyCheckChange(bool check)
{
    _proxyEdit->setEnabled(check);
    Global->setOpenProxy(check);
}

void SettingDownloadWidget::proxyTextChange(const QString &proxyText)
{
    Global->setProxyUrl(proxyText);
}
