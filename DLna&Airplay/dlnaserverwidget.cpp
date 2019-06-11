#include "dlnaserverwidget.h"

#include "controlmanager.h"
#include "dlnactrlpoint.h"
#include "dlnalistwidget.h"
#include "dylabel.h"
#include "globalarg.h"
#include "configure.h"
#include "dlnamanager.h"
#include "openfile.h"
#include "svgbutton.h"
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include <QFileDialog>
#include <QMouseEvent>

#define BtnWidth 120
DLnaServerWidget::DLnaServerWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    init();
}

void DLnaServerWidget::updateLanguage()
{
    upTextInfo();
    _dlnaListW->updateLanguage();
}

void DLnaServerWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    _addFolderBtn->updateUI(_scaleRatio);
    _addFileBtn->updateUI(_scaleRatio);
    _dlnaListW->updateUI(_scaleRatio);
    upUI();
}

void DLnaServerWidget::init()
{
    _firstLab   = new QLabel(this);
    _secondLab  = new QLabel(this);
    _addFolderBtn   = new SvgButton(this);
    _addFileBtn     = new SvgButton(this);
    _firstLab->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _firstLab->setStyleSheet("Color:rgb(185,186,192)");
    _secondLab->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _secondLab->setStyleSheet("Color:rgb(185,186,192)");

    _addFolderBtn->initSize(BtnWidth,BtnWidth);
    _addFolderBtn->setImagePrefixName("add_folder");
    _addFolderBtn->setSvgText("Add Folder");
    _addFolderBtn->setSvgTextColor(QColor(244,244,244),QColor(217,235,250));

    _addFileBtn->initSize(BtnWidth,BtnWidth);
    _addFileBtn->setImagePrefixName("add_files");
    _addFileBtn->setSvgText("Add File");
    _addFileBtn->setSvgTextColor(QColor(244,244,244),QColor(217,235,250));

    _devListLab = new QLabel(this);
    _line1      = new QLabel(this);
    _line2      = new QLabel(this);

    _dlnaListW  = new dlnaListWidget(this);
    _devListLab->setStyleSheet("Color:rgb(185,186,192)");
    _devListLab->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _line1->setStyleSheet("background-color: rgb(31, 31, 35);");
    _line2->setStyleSheet("background-color: rgb(46, 46, 51);");

    connect(_addFolderBtn,SIGNAL(clicked()),SLOT(addFolderBtnClick()));
    connect(_addFileBtn,SIGNAL(clicked()),SLOT(addFileBtnClick()));
    connect(CONTROL,SIGNAL(sendUPDevList(QList<deviceInfo>,bool)),SLOT(checkDevSupInfo(QList<deviceInfo>,bool)));

    _prePath    = "";
    _scaleRatio = 1.0;
    _mouseRight = false;
    QList<deviceInfo> tempList;
    tempList.clear();
    checkDevSupInfo(tempList,false);
}

void DLnaServerWidget::upUI()
{
    int left = 20*_scaleRatio;
    int textYPos = (this->height()/3 - 70*_scaleRatio)/2;
    _firstLab->setGeometry(left,textYPos,this->width()-left,20*_scaleRatio);
    _secondLab->setGeometry(left,textYPos+30*_scaleRatio,this->width()-left,40*_scaleRatio);
    int labYPos = this->height()/3*2;
    _devListLab->setGeometry(left,labYPos,this->width()-40*_scaleRatio,20*_scaleRatio);
    _line1->setGeometry(left,labYPos+24*_scaleRatio,this->width()-40*_scaleRatio,1*_scaleRatio);
    _line2->setGeometry(left,labYPos+25*_scaleRatio,this->width()-40*_scaleRatio,1*_scaleRatio);
    _dlnaListW->setGeometry(left,labYPos+28*_scaleRatio,this->width()-40*_scaleRatio,this->height()/3-32*_scaleRatio);
    int btnWidth= BtnWidth*_scaleRatio;
    int btnLeft = (this->width() - btnWidth*2)/3;
    int btnTop  = (this->height() - btnWidth)/ 2;

    _addFolderBtn->resetMove(btnLeft,btnTop);
    _addFileBtn->resetMove(btnLeft*2+btnWidth,btnTop);
    upTextInfo();
}

void DLnaServerWidget::upTextInfo()
{
    QFont font;
    font.setPixelSize(14*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    _firstLab->setFont(font);
    _secondLab->setFont(font);
    _devListLab->setFont(font);
    _firstLab->setText(DLNA->getVirTextInfo(_firstLab,Lge->getLangageValue("DLNA/serlab1")));
    _secondLab->setText(DLNA->getCurShowText(_firstLab,Lge->getLangageValue("DLNA/serlab2")));
    _devListLab->setText(DLNA->getVirTextInfo(_firstLab,Lge->getLangageValue("DLNA/devices")));
    _addFileBtn->setSvgText(Lge->getLangageValue("DLNA/file"));
    _addFolderBtn->setSvgText(Lge->getLangageValue("DLNA/floder"));
}

QString DLnaServerWidget::getCurShowText(QLabel *lab, QString text)
{
    int labWidth = lab->width();
    QString showText = "";
    // 英语要单词分拆
    if(Global->getLanguage() == "en" || Global->getLanguage() == "zh") {
        QStringList textList = text.split(" ");
        QString temp = "";
        for(int i=0;i<textList.count();i++) {
            temp.append(textList.at(i)).append(" ");
            int tempWidth = lab->fontMetrics().width(temp);
            if(tempWidth > labWidth) {
                temp = "";
                showText.append("\n");
            }
            showText.append(textList.at(i)).append(" ");
        }
    } else if(Global->getLanguage() == "jp"){
        QString temp = "";
        int index = 0;
        for(int i=0;i<text.length();i++) {
            temp.append(text.at(i));
            int tempWidth = lab->fontMetrics().width(temp);
            if(tempWidth > labWidth) {
                index++;
                if(index == 2) {
                    showText.remove(i-2,2);
                    showText.append("...");
                    break;
                } else {
                    temp = "";
                    showText.append("\n");
                }
            }
            showText.append(text.at(i));
        }
    }
    return showText;
}

void DLnaServerWidget::checkDevSupInfo(QList<deviceInfo> devList,bool isAdd)
{
    _dlnaListW->upWidget(isAdd);
}

void DLnaServerWidget::addFolderBtnClick()
{
    QString path = QFileDialog::getExistingDirectory(NULL,Lge->getLangageValue("OpenFile/opfolder"),"",QFileDialog::ReadOnly);
    if(!DLNA->pathIsExitsData(path))
        DLNA->addOrDelFolder(path,true);
}

void DLnaServerWidget::addFileBtnClick()
{
    QString openPath;
    if(_prePath == "") {
        openPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    } else {
        openPath = _prePath;
    }

    QStringList fileList;

    fileList = OpenFileDialog->getFileNames(Lge->getLangageValue("OpenFile/opmedia"),
                                            openPath,
                                            Lge->getLangageValue("OpenFile/allfile") + ";;" +
                                            Lge->getLangageValue("OpenFile/videoFilt") + "(" +
                                            Config->getUserVideoFormat() + ");;" +
                                            Lge->getLangageValue("OpenFile/musicFilt") + "(" +
                                            Config->getUserAudioFormat() + ");");

    if (fileList.isEmpty())
        return;

    QString file = fileList.at(0);
    QFileInfo fileinfo(file);
    _prePath = fileinfo.path();

    QStringList virPaths = DLNA->pathsIsExitsData(fileList);
    if(virPaths.count() > 0)
        OpenFileDialog->addDLnaDialog(virPaths);
}

void DLnaServerWidget::mousePressEvent(QMouseEvent *e)
{
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
    if(mouseEvent->button() == Qt::RightButton &&
            this->rect().contains(e->pos()))
    {
        _mouseRight = true;
        e->accept();
    } else {
        _mouseRight = false;
        e->ignore();
    }
}

void DLnaServerWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(_mouseRight) {
        e->accept();
    } else {
        e->ignore();
    }
}

void DLnaServerWidget::resizeEvent(QResizeEvent *event)
{
    upUI();
}
