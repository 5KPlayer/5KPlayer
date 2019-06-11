#include "filerelationwidget.h"
#include <QtDebug>
#include <QProcess>
#include "dylabel.h"
#include "dycheckbox.h"
#include "globalarg.h"
#include "filerelation.h"
#include "configure.h"
#include <QFile>

FileRelationWidget::FileRelationWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    initDYLabel(&_selectAllBtn, Lge->getLangageValue("Relation/all"));
    initDYLabel(&_immediateEffectBtn, Lge->getLangageValue("Relation/apply"));
    initDYLabel(&_okBtn, Lge->getLangageValue("ToolTip/colse"));

    //initDYCheckBox(&_autoBootBox,  Lge->getLangageValue("Relation/boot"));
    initDYCheckBox(&_assocFileBox, Lge->getLangageValue("Relation/associate"));

    //_autoBootBox->setChecked(Global->isOpenBootUp());
    _assocFileBox->setChecked(Global->autoAssociation());

    QString path(Global->_cur_path);
    path.replace("/", "\\");
    _relation = new FileRelation(path, this);

    //读取固定的format
    QStringList videoList = Config->getFixedVideoFormat().split(" ", QString::SkipEmptyParts);
    videoList.replaceInStrings("*", "");
    qSort(videoList);

    QStringList musicList = Config->getFixedAudioFormat().split(" ", QString::SkipEmptyParts);
    musicList.replaceInStrings("*", "");
    qSort(musicList);

    //读取可变的format
    QStringList videoList2 = Config->getVarVideoFormat().split(" ", QString::SkipEmptyParts);
    videoList2.replaceInStrings("*", "");
    qSort(videoList2);

    QStringList musicList2 = Config->getVarAudioFormat().split(" ", QString::SkipEmptyParts);
    musicList2.replaceInStrings("*", "");
    qSort(musicList2);

    suffixList << videoList << musicList
               << ".iso" << "DVD"
               << videoList2 << musicList2;

    for(QString suffix : suffixList) {
        DYCheckBox *box;
        suffix.remove("(");
        suffix.remove(")");
        initDYCheckBox(&box, suffix);
        if(suffix != "DVD")
            box->setChecked(_relation->isFileRelation(suffix));
        else
            box->setChecked(_relation->isRegistDvd());//
        if(box->isChecked())
            m_regList << box->text();
        boxList << box;
    }

    updateUI(Global->_screenRatio);

    connect(_selectAllBtn, &DYLabel::clicked, [=](){
        for(DYCheckBox *box : boxList) {
            box->setChecked(true);
        }
    });
    connect(_immediateEffectBtn, SIGNAL(clicked()), SLOT(effClicked()));
    connect(_okBtn, SIGNAL(clicked()), SLOT(okClicked()));
}

void FileRelationWidget::updateUI(const qreal &scaleRatio)
{
    _ratio = scaleRatio;
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setBold(true);
    font.setPixelSize(12 * scaleRatio);

    _selectAllBtn->updateUI(scaleRatio);
    _selectAllBtn->setFont(font);
    _selectAllBtn->resize(120 * scaleRatio, 24 * scaleRatio);

    _assocFileBox->updateUI(scaleRatio);
    _assocFileBox->setFont(font);
    _assocFileBox->resize(200 * scaleRatio, 24 * scaleRatio);

    //_autoBootBox->updateUI(scaleRatio);
    //_autoBootBox->setFont(font);
    //_autoBootBox->resize(200 * scaleRatio, 24 * scaleRatio);

    _immediateEffectBtn->updateUI(scaleRatio);
    _immediateEffectBtn->setFont(font);
    _immediateEffectBtn->resize(80 * scaleRatio, 24 * scaleRatio);

    _okBtn->updateUI(scaleRatio);
    _okBtn->setFont(font);
    _okBtn->resize(80 * scaleRatio, 24 * scaleRatio);

    for(DYCheckBox *box : boxList) {
        box->updateUI(scaleRatio);
        box->setFont(font);
        box->resize(80 * scaleRatio, 24 * scaleRatio);
    }
}

void FileRelationWidget::effClicked()
{
    QStringList regList;
    QStringList unregList;
    for(DYCheckBox *box : boxList) {
        const QString suffix = box->text();
        if(box->isChecked())
            regList << suffix;
        else
            unregList << suffix;
    }
    bool b = false;//需要启动exe修改权限
    if(regList.count() == m_regList.count()) {
        for(QString name: m_regList) {
            if(regList.indexOf(name) == -1) {
                b = true;
                break;
            }
        }
    } else {
        b = true;
    }
    if(b) {
        QStringList args;
        if(regList.count() > 0) {
            args << "-reg"   << regList;
        }
        if(unregList.count() > 0)
            args << "-unreg" << unregList;

        if(QProcess::startDetached(Global->_cur_path + "/FileRelation.exe", args, "."))
        {
            //写文件  用于 FileRelation.exe -uninstall
            QString fileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            fileName.append("/uninstall.dat");
            QFile fi(fileName);
            if(fi.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                fi.write(regList.join(" ").toLatin1());
                fi.flush();
                fi.close();
            }
        }
    }

    //if(_autoBootBox->isChecked()) {
    //    Global->openBootUp();
    //} else {
    //    Global->closeBootUp();
    //}

    Global->setAutoAssociation(_assocFileBox->isChecked());
}

void FileRelationWidget::okClicked()
{
    this->parentWidget()->close();
}

void FileRelationWidget::resizeEvent(QResizeEvent *)
{
    const int margin = 12 * _ratio;
    const int count = boxList.count();
    int row = 0;
    int col = 0;
    int colNum = (this->width()-margin*2)/(80 * _ratio);
    double xPos = (this->width() - (colNum*80 * _ratio))/2;
    for(int i=0; i<count; i++) {
        row = i/colNum;//行
        col = i%colNum;//列
        boxList.at(i)->move(xPos + col * 80 * _ratio, row * 24 * _ratio);
    }
    _selectAllBtn->move(xPos, (row + 1.5) * 24 * _ratio);

    //_autoBootBox->move(xPos,  (row + 3) * 24 * _ratio);
    //_assocFileBox->move(xPos, (row + 4.5) * 24 * _ratio);
    _assocFileBox->move(xPos, (row + 3) * 24 * _ratio);

    _okBtn->move(this->width()  - _okBtn->width()  - xPos,
                 this->height() - _okBtn->height() - margin);

    _immediateEffectBtn->move(_okBtn->x() - _immediateEffectBtn->width() - margin,
                              _okBtn->y());
}

void FileRelationWidget::initDYLabel(DYLabel **label, const QString &str)
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

void FileRelationWidget::initDYCheckBox(DYCheckBox **box, const QString &str)
{
    QPalette p1;
    p1.setColor(QPalette::WindowText, Qt::white);

    (*box) = new DYCheckBox(this, str);
    (*box)->setPalette(p1);
}
