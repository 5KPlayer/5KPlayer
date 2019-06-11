#include "dvdplaywidget.h"
#include <QLabel>
#include "dylabel.h"
#include "openfile.h"
#include "dymediaplayer.h"
#include "combobox.h"
#include "dycheckbox.h"
#include <QDialog>
#include <QLineEdit>
#include <QFileInfo>
#include <QDir>
#include "svgbutton.h"
#include "librarydata.h"
#include "dydvdinfo.h"
#include "globalarg.h"
#include "textbutton.h"
#include "globalarg.h"
#include "adlogic.h"

DVDPlayWidget::DVDPlayWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    initLabel(&_label1,   Lge->getLangageValue("DVD/souDvd"));
    initLabel(&_label2,   Lge->getLangageValue("DVD/DvdDri"));
    initLabel(&_label2_1, Lge->getLangageValue("DVD/DvdFile"));
    initLabel(&_label3,   Lge->getLangageValue("DVD/Dvdrepper"));
    initLabel(&_label4,   Lge->getLangageValue("DVD/fastest"));
    initLabel(&_label5,   Lge->getLangageValue("DVD/supAir"));
    initLabel(&_loadLabel,Lge->getLangageValue("DVD/load"));
    initLabel(&_dvdOpenType,Lge->getLangageValue("DVD/openType"));
    _loadLabel->setStyleSheet("color: red");

    _dvdPxiamap = new QLabel(this);
    _dvdPxiamap->setScaledContents(true);//
#ifdef Q_OS_WIN
    _dvdPxiamap->setPixmap(QPixmap(":/res/ripnav.png"));
#else
    _dvdPxiamap->setPixmap(QPixmap(":/res/ripnav_mac.png"));
#endif
    _dvdPxiamap->resize(188, 164);
    _dvdPxiamap->setVisible(false);

    _autoType = new DYCheckBox(this,Lge->getLangageValue("DVD/autoType"));
    _udfType  = new DYCheckBox(this,Lge->getLangageValue("DVD/udfType"));
    _isoType  = new DYCheckBox(this,Lge->getLangageValue("DVD/isoType"));

    _autoType->resize(110,30);
    _udfType->resize(110,30);
    _isoType->resize(110,30);
    switch (Global->dvdOpenType()) {
    case 0:
        _autoType->setChecked(true);
        break;
    case 1:
        _udfType->setChecked(true);
        break;
    case 2:
        _isoType->setChecked(true);
        break;
    default:
        break;
    }

    _label4Ico = new QLabel(this);
    _label5Ico = new QLabel(this);

    _label4Ico->setPixmap(QPixmap(":/res/png/right.png"));
    _label5Ico->setPixmap(QPixmap(":/res/png/right.png"));
    _label2_1->setVisible(false);

    initDYLabel(&_okBtn,    Lge->getLangageValue("Main/play"));
    initDYLabel(&_cancelBtn,Lge->getLangageValue("Main/cancel"));
    initDYLabel(&_scanBtn,  Lge->getLangageValue("DVD/scan"));

    _okBtn->setBackClolr("rgb(0,140,220)","rgb(51,191,242)","rgb(0,175,239)");
    _scanBtn->setVisible(false);

    _downloadBtn = new SvgButton(this);
    _downloadBtn->initSize(144, 39);
    _downloadBtn->setImagePrefixName("free_download");
    _downloadBtn->setSvgTextCenter(true);
    //_downloadBtn->setSvgTextFlag(Qt::AlignVCenter);
    _downloadBtn->setSvgTextXpos(26*Global->_screenRatio);
    _downloadBtn->setSvgText(Lge->getLangageValue("DVD/download"));

    _combo = new ComboBox(this);
    _combo->resize(340, 24);
    _combo->setEditable(false);

    _lineEdit = new QLineEdit(this);
    _lineEdit->resize(340, 24);
    _lineEdit->setVisible(false);
    _lineEdit->setReadOnly(true);

    _DVD_DISC  = new TextButton(Lge->getLangageValue("DVD/disc"), 270, 48, this);
    _DVD_Image = new TextButton(Lge->getLangageValue("DVD/imageF"), 270, 48, this);
    _DVD_Image->setPosition(TextButton::Right);

    connect(_DVD_DISC,  SIGNAL(clicked()), SLOT(showDVDDisc()));
    connect(_DVD_DISC,  SIGNAL(clicked()), _DVD_Image, SLOT(recover()));

    connect(_DVD_Image, SIGNAL(clicked()), SLOT(showDVDImageFile()));
    connect(_DVD_Image, SIGNAL(clicked()), _DVD_DISC, SLOT(recover()));

    _dvdInfo = new DYDVDInfo(this);
    connect(_dvdInfo, SIGNAL(findStarted()), SLOT(startFind()));
    connect(_dvdInfo, SIGNAL(findEnded()), SLOT(endFind()));

    connect(_okBtn, SIGNAL(clicked()), SLOT(playDVD()));
    connect(_cancelBtn, SIGNAL(clicked()), SLOT(cancel()));
    connect(_scanBtn, SIGNAL(clicked()),   SLOT(showDVDScanDialog()));

    connect(_downloadBtn, SIGNAL(clicked()), OpenFileDialog, SLOT(openFreeDownload()));

    connect(_autoType,SIGNAL(checkChange(bool)),SLOT(autoTypeChange(bool)));
    connect(_udfType,SIGNAL(checkChange(bool)),SLOT(udfTypeChange(bool)));
    connect(_isoType,SIGNAL(checkChange(bool)),SLOT(isoTypeChange(bool)));

    _DVD_DISC->click();
    _dvdInfo->start();

    updateUI(Global->_screenRatio);
}

void DVDPlayWidget::updateUI(const qreal &scaleRatio)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);
    font.setBold(true);

    _space = 24 * scaleRatio;
    _adHeight = 92 * scaleRatio;

    _label1->setFont(font);
    _label1->adjustSize();

    _label2->setFont(font);
    _label2->adjustSize();

    _label2_1->setFont(font);
    _label2_1->adjustSize();

    _label3->setFont(font);
    _label3->adjustSize();

    _label4->setFont(font);
    _label4->adjustSize();

    _label5->setFont(font);
    _label5->adjustSize();

    _dvdOpenType->setFont(font);
    _dvdOpenType->adjustSize();

    _loadLabel->setFont(font);
    _loadLabel->adjustSize();

    _dvdPxiamap->resize(188*scaleRatio, 164*scaleRatio);

    _downloadBtn->setSvgTextXpos(24*scaleRatio);
    _downloadBtn->updateUI(scaleRatio);

    _okBtn->setFont(font);
    _okBtn->resize(80 * scaleRatio, 24 * scaleRatio);

    _cancelBtn->setFont(font);
    _cancelBtn->resize(80 * scaleRatio, 24 * scaleRatio);

    _scanBtn->setFont(font);
    _scanBtn->resize(80 * scaleRatio, 24 * scaleRatio);

    _combo->resize(340 * scaleRatio, 24 * scaleRatio);
    _combo->setFont(font);

    _combo->upStyleSheet(scaleRatio);

    _lineEdit->resize(340 * scaleRatio, 24 * scaleRatio);
    _lineEdit->setFont(font);
    _lineEdit->setStyleSheet(Global->lineEditStyleSheet(4 * scaleRatio));

    _DVD_DISC->updateUI(scaleRatio);
    _DVD_Image->updateUI(scaleRatio);

    _autoType->updateUI(scaleRatio);
    _udfType->updateUI(scaleRatio);
    _isoType->updateUI(scaleRatio);
    _autoType->resize(110*scaleRatio,30*scaleRatio);
    _udfType->resize(110*scaleRatio,30*scaleRatio);
    _isoType->resize(110*scaleRatio,30*scaleRatio);
}

void DVDPlayWidget::playDVD()
{
    if(_DVD_Image->pressed()) {
        if(!_lineEdit->text().isEmpty()) {
            OpenFileDialog->openFile(_lineEdit->text());
            qobject_cast<QDialog *>(this->parent())->accept();

        }
        return;
    }
    // DVD DISC
    const int index = _combo->currentIndex();
    if(index < 0)
        return;

    //判断目录
    const QString path = _dvdInfo->findDVD(index);
    QDir dir(path);
    QStringList dirList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    bool isDVD = false;
    foreach (const QString dirName, dirList) {
        if(dirName.toUpper() == "VIDEO_TS") {
            isDVD = true;
            break;
        }
    }

    if(isDVD) {
        OpenFileDialog->setPlayType(OPenFile::DVD);
        MediaPlayer->playFile("dvd:///" + path, false);
        //单击播放 关闭当前窗口
        qobject_cast<QDialog *>(this->parent())->accept();
    }

}

void DVDPlayWidget::cancel()
{
    //单击播放 关闭当前窗口
    qobject_cast<QDialog *>(this->parent())->accept();
}

void DVDPlayWidget::startFind()
{
    _combo->setVisible(false);
    _loadLabel->setVisible(true);
    //_okBtn->setVisible(false);
}

void DVDPlayWidget::endFind()
{
    _combo->addItems(_dvdInfo->dvdList());
    _combo->setVisible(true);
    _loadLabel->setVisible(false);

    //if(_combo->count() > 0)
    //    _okBtn->setVisible(true);
}

void DVDPlayWidget::showDVDDisc()
{
    _label2->setVisible(true);
    _combo->setVisible(true);

    _label2_1->setVisible(false);
    _lineEdit->setVisible(false);
    _scanBtn->setVisible(false);
}

void DVDPlayWidget::showDVDImageFile()
{
    _label2->setVisible(false);
    _combo->setVisible(false);

    _label2_1->setVisible(true);
    _lineEdit->setVisible(true);
    _scanBtn->setVisible(true);
}

void DVDPlayWidget::showDVDScanDialog()
{
    const QString file = OpenFileDialog->openDvdFileDialog();
    if(file.isEmpty())
        return;

    _lineEdit->setText(file);
}

void DVDPlayWidget::autoTypeChange(bool change)
{
    if(change) {
        _autoType->setChecked(true);
        _udfType->setChecked(false);
        _isoType->setChecked(false);
        Global->setDvdOpenType(0);
    }
}

void DVDPlayWidget::udfTypeChange(bool change)
{
    if(change) {
        _autoType->setChecked(false);
        _udfType->setChecked(true);
        _isoType->setChecked(false);
        Global->setDvdOpenType(1);
    }
}

void DVDPlayWidget::isoTypeChange(bool change)
{
    if(change) {
        _autoType->setChecked(false);
        _udfType->setChecked(false);
        _isoType->setChecked(true);
        Global->setDvdOpenType(2);
    }
}

void DVDPlayWidget::resizeEvent(QResizeEvent *)
{
    const int w = this->width();
    const int h = this->height();

    //按钮
    _DVD_DISC->move(0,0);
    _DVD_DISC->setFixedWidth(w / 2);
    _DVD_Image->move(_DVD_DISC->width(), 0);
    _DVD_Image->setFixedWidth(w / 2);


    //
    _label1->move(_space, _DVD_DISC->y() + _DVD_DISC->height() + _space);
    _label2->move(_space*2, _label1->y() + _label1->height() + _space);
    _label2_1->move(_label2->pos());
    _loadLabel->move(_label2->x() + _label2->width() + _space / 4, _label2->y());

    _scanBtn->move(w - _scanBtn->width() - _space,
                   _label2->y() - (_combo->height()-_label2->height())/2);

    _combo->move(_label2->x() + _label2->width() + _space / 4, _scanBtn->y());
    const int w1 = _scanBtn->x() - _combo->x() - _space / 4;
    if(w1 > 8)
        _combo->setFixedWidth(_scanBtn->x() - _combo->x() - _space / 4);
    _lineEdit->setGeometry(_combo->geometry());



    _okBtn->move(_combo->x() + _combo->width() - _okBtn->width(),
                     _combo->y() + _combo->height() + _space);

    _cancelBtn->move(_okBtn->x() - _cancelBtn->width() - _space, _okBtn->y());

    // DVD OenType
    _dvdOpenType->move(_space,_cancelBtn->y()+_cancelBtn->height() + 4*Global->_screenRatio);
    _autoType->move(_space*2,_dvdOpenType->y()+_dvdOpenType->height()+4*Global->_screenRatio);
    _udfType->move(_autoType->x()+_autoType->width()+20*Global->_screenRatio,_autoType->y());
    _isoType->move(_udfType->x()+_udfType->width()+20*Global->_screenRatio,_autoType->y());

    //广告位置
    _label3->move(w - _label3->width() - _space/2, h - _adHeight);
    _label4->move(_label3->x() + _space, _label3->y() + _label3->height() + _space/2);
    _label5->move(_label4->x(), _label4->y() + _label4->height() + _space/2);
    _label4Ico->setGeometry(_label4->x()-14*Global->_screenRatio,_label4->y()+(_label4->height()-8*Global->_screenRatio)/2,10*Global->_screenRatio,8*Global->_screenRatio);
    _label5Ico->setGeometry(_label5->x()-14*Global->_screenRatio,_label5->y()+(_label5->height()-8*Global->_screenRatio)/2,10*Global->_screenRatio,8*Global->_screenRatio);

    _downloadBtn->move(_label4->x() + _label4->width() + _space*2, _label4->y());

    if((_label3->x() > _dvdPxiamap->width() + 8*Global->_screenRatio) &&
       (h - _dvdPxiamap->height() - 4*Global->_screenRatio > _autoType->y()+_autoType->height())) {
        _dvdPxiamap->move(_label3->x() - _dvdPxiamap->width() - 4*Global->_screenRatio,
                          h - _dvdPxiamap->height() - 4*Global->_screenRatio);
        _dvdPxiamap->setVisible(true);
    } else {
        _dvdPxiamap->setVisible(false);
    }
}

void DVDPlayWidget::initLabel(QLabel **label, const QString &str)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12);
    font.setBold(true);

    (*label) = new QLabel(str, this);
    (*label)->setFont(font);
    (*label)->setPalette(p1);
    (*label)->adjustSize();
}

void DVDPlayWidget::initDYLabel(DYLabel **label, const QString &str)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12);
    font.setBold(true);

    (*label) = new DYLabel(this, true);
    (*label)->setFont(font);
    (*label)->setText(str);
    (*label)->resize(80, 24);
    (*label)->setAlignment(Qt::AlignCenter);
}
