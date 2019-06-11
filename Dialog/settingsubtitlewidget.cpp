#include "settingsubtitlewidget.h"
#include "globalarg.h"
#include "colorbutton.h"
#include <QFontDatabase>
#include <QSpinBox>
#include <QListView>
#include <QScrollBar>
#include <QStandardItemModel>
#include "combobox.h"

SettingSubtitleWidget::SettingSubtitleWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    Init();
}

void SettingSubtitleWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

int SettingSubtitleWidget::getSubPosForKey(QString str)
{
    return _subPosMap.value(str);
}

QStringList SettingSubtitleWidget::getSubKeys()
{
    return _subPosMap.keys();
}

void SettingSubtitleWidget::Init()
{
    _title              = new TransEventLabel(this);
    _titleLine          = new TransEventLabel(this);
    _titleLine2         = new TransEventLabel(this);

    _encoding           = new TransEventLabel(this);
    _encodingBox        = new ComboBox(this);
    _font               = new TransEventLabel(this);
    _fontBox            = new ComboBox(this);
    _fontSize           = new TransEventLabel(this);
    _fontSizeBox        = new ComboBox(this);
    _autofuzzy          = new TransEventLabel(this);
    _autofuzzyBox       = new ComboBox(this);
    _textColor          = new TransEventLabel(this);
    _textColorBtn       = new ColorButton(this);
    _textOpacity        = new TransEventLabel(this);
    _textOpacityValue   = new QSpinBox(this);

    _backColor          = new TransEventLabel(this);
    _backColorBtn       = new ColorButton(this);
    _backOpacity        = new TransEventLabel(this);
    _backOpacityValue   = new QSpinBox(this);

    _title->setStyleSheet("color:rgb(255,255,255)");
    _title->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    _title->setText(Lge->getLangageValue("SetMain/sub"));
    _titleLine->setStyleSheet("background-color: rgb(20, 20, 21);");
    _titleLine2->setStyleSheet("background-color: rgb(71, 71, 75);");

    _encoding->setStyleSheet("color:rgb(255,255,255)");
    _encoding->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _encoding->setText(Lge->getLangageValue("SetSub/encod"));

    _font->setStyleSheet("color:rgb(255,255,255)");
    _font->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _font->setText(Lge->getLangageValue("SetSub/font"));

    _fontSize->setStyleSheet("color:rgb(255,255,255)");
    _fontSize->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _fontSize->setText(Lge->getLangageValue("SetSub/fontsize"));

    _autofuzzy->setStyleSheet("color:rgb(255,255,255)");
    _autofuzzy->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _autofuzzy->setText(Lge->getLangageValue("SetSub/fuzzy"));

    _textColor->setStyleSheet("color:rgb(255,255,255)");
    _textColor->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _textColor->setText(Lge->getLangageValue("SetSub/textClr"));

    _textOpacity->setStyleSheet("color:rgb(255,255,255)");
    _textOpacity->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _textOpacity->setText(Lge->getLangageValue("SetSub/opacity"));

    _backColor->setStyleSheet("color:rgb(255,255,255)");
    _backColor->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _backColor->setText(Lge->getLangageValue("SetSub/backClr"));

    _backOpacity->setStyleSheet("color:rgb(255,255,255)");
    _backOpacity->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _backOpacity->setText(Lge->getLangageValue("SetSub/opacity"));

    _textOpacityValue->setMinimum(0);
    _textOpacityValue->setMaximum(255);
    _textOpacityValue->setStyleSheet("color: rgb(255, 255, 255);");
    _backOpacityValue->setMinimum(0);
    _backOpacityValue->setMaximum(255);
    _backOpacityValue->setStyleSheet("color: rgb(255, 255, 255);");
    _scaleRatio = 1.0;

    QStringList encodeList;
    encodeList << "system"       << "UTF-8"
               << "UTF-16"       << "UTF-16BE"
               << "UTF-16LE"     << "GB18030"
               << "ISO-8859-15"  << "Windows-1252"
               << "IBM850"       << "ISO-8859-2"
               << "Windows-1250" << "ISO-8859-3"
               << "ISO-8859-10"  << "Windows-1251"
               << "KOI8-R"       << "KOI8-U"
               << "ISO-8859-6"   << "Windows-1256"
               << "ISO-8859-7"   << "Windows-1253"
               << "ISO-8859-8"   << "Windows-1255"
               << "ISO-8859-9"   << "Windows-1254"
               << "ISO-8859-11"  << "Windows-874"
               << "ISO-8859-13"  << "Windows-1257"
               << "ISO-8859-14"  << "ISO-8859-16"
               << "ISO-2022-CN-EXT" << "EUC-CN"
               << "ISO-2022-JP-2"   << "EUC-JP"
               << "Shift_JIS"       << "CP949"
               << "ISO-2022-KR"     << "Big5"
               << "ISO-2022-TW"     << "Big5-HKSCS"
               << "VISCII"          << "Windows-1258";
    _encodingBox->addItems(encodeList);

    QFontDatabase fontdatabase;
    _fontBox->addItems(fontdatabase.families());

    QString fontsizeList = Lge->getLangageValue("SetSub/fontList");
    _fontSizeBox->addItems(fontsizeList.split("|"));

    QString autofuzzy = Lge->getLangageValue("SetSub/fuzzyList");
    QStringList autofuzzyList = autofuzzy.split("|");
    QStandardItemModel *model = new QStandardItemModel();//添加提示tootip
    QLabel label;
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * Global->_screenRatio);
    label.setFont(font);
    for(int i = 0; i < autofuzzyList.count(); ++i){
        QStandardItem *item = new QStandardItem(autofuzzyList.at(i));
        item->setToolTip(autofuzzyList.at(i));
        model->appendRow(item);
    }
    _autofuzzyBox->setModel(model);

    _encodingBox->setCurrentText(Global->subtitleEncode());
    _fontBox->setCurrentText(Global->subtitleFont());
    _fontSizeBox->setCurrentIndex(Global->subtitleSize());
    _autofuzzyBox->setCurrentIndex(Global->autodetectFuzzy());

    connect(_encodingBox, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
            [=](const QString &text){
        Global->setSubtitleEncode(text);
    });
    connect(_fontBox, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
            [=](const QString &text){
        Global->setSubtitleFont(text);
    });
    connect(_fontSizeBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){
        Global->setSubtitleSize(CGlobal::Subtitle_FontSize(index));
    });
    connect(_autofuzzyBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){
        Global->setAutodetectFuzzy(index);
    });

    _textOpacityValue->setValue(Global->textOpacity());
    _textColorBtn->setColor(Global->textColor());
    _backOpacityValue->setValue(Global->bgOpacity());
    _backColorBtn->setColor(Global->bgColor());

    connect(_textColorBtn,SIGNAL(valueChanged(QString)),SLOT(textColorChange(QString)));
    connect(_backColorBtn,SIGNAL(valueChanged(QString)),SLOT(backColorChange(QString)));
    connect(_textOpacityValue,SIGNAL(valueChanged(int)),SLOT(textOpacityChange(int)));
    connect(_backOpacityValue,SIGNAL(valueChanged(int)),SLOT(backOpacityChange(int)));
    upUI();
}

void SettingSubtitleWidget::upUI()
{
    _subPosMap.clear();
    double height = 20 *_scaleRatio;
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * _scaleRatio);
    //font.setWeight(63);
    font.setBold(true);
    _encoding->setFont(font);
    _encodingBox->setFont(font);

    _font->setFont(font);
    _fontBox->setFont(font);

    _fontSize->setFont(font);
    _fontSizeBox->setFont(font);

    _autofuzzy->setFont(font);
    _autofuzzyBox->setFont(font);

    _textColor->setFont(font);
    _backColor->setFont(font);
    _textOpacityValue->setFont(font);
    _textOpacity->setFont(font);
    _backOpacity->setFont(font);
    _backOpacityValue->setFont(font);

    font.setPixelSize(14 * _scaleRatio);
    _title->setFont(font);

    double xPos = 4*_scaleRatio;

    int titleW  = _title->fontMetrics().width(_title->text());
    _title->setGeometry(xPos,       5*_scaleRatio,titleW,height);
    int lineW   = this->width()- (_title->x() + _title->width() + 28*_scaleRatio);
    _titleLine->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,15*_scaleRatio,lineW, 1*_scaleRatio);
    _titleLine2->setGeometry(_title->x() + _title->width() + 8*_scaleRatio,16*_scaleRatio,lineW, 1*_scaleRatio);

    _encoding->setGeometry(xPos,40*_scaleRatio,210*_scaleRatio,height);
    _encodingBox->setGeometry(xPos+218*_scaleRatio,40*_scaleRatio,280*_scaleRatio,height);
    _encodingBox->upStyleSheet(_scaleRatio);
    _subPosMap.insert(Lge->getLangageValue("SetMain/sub") + ":" + LangNoColon("SetSub/encod"),40*_scaleRatio);

    _font->setGeometry(xPos,75*_scaleRatio,210*_scaleRatio,height);
    _fontBox->setGeometry(xPos+218*_scaleRatio,75*_scaleRatio,280*_scaleRatio,height);
    _fontBox->upStyleSheet(_scaleRatio);
    _subPosMap.insert(Lge->getLangageValue("SetMain/sub") + ":" + LangNoColon("SetSub/font"),75*_scaleRatio);

    _fontSize->setGeometry(xPos,110*_scaleRatio,210*_scaleRatio,height);
    _fontSizeBox->setGeometry(xPos+218*_scaleRatio,110*_scaleRatio,280*_scaleRatio,height);
    _fontSizeBox->upStyleSheet(_scaleRatio);
    _subPosMap.insert(Lge->getLangageValue("SetMain/sub") + ":" + LangNoColon("SetSub/fontsize"),110*_scaleRatio);

    _autofuzzy->setGeometry(xPos,145*_scaleRatio,210*_scaleRatio,height);
    _autofuzzyBox->setGeometry(xPos+218*_scaleRatio,145*_scaleRatio,280*_scaleRatio,height);
    _autofuzzyBox->upStyleSheet(_scaleRatio);
    _subPosMap.insert(Lge->getLangageValue("SetMain/sub") + ":" + LangNoColon("SetSub/fuzzy"),145*_scaleRatio);

    _textColor->setGeometry(xPos,                       190*_scaleRatio,210*_scaleRatio,height);
    _textColorBtn->updateUI(_scaleRatio);
    _textColorBtn->setGeometry(xPos+218*_scaleRatio,    190*_scaleRatio,104*_scaleRatio,height);
    _textOpacity->setGeometry(xPos+328*_scaleRatio,     190*_scaleRatio,76*_scaleRatio,height);
    _textOpacityValue->setGeometry(xPos+418*_scaleRatio,190*_scaleRatio,80*_scaleRatio,height);
    _subPosMap.insert(Lge->getLangageValue("SetMain/sub") + ":" + LangNoColon("SetSub/textClr"),190*_scaleRatio);

    _backColor->setGeometry(xPos,                       225*_scaleRatio,210*_scaleRatio,height);
    _backColorBtn->updateUI(_scaleRatio);
    _backColorBtn->setGeometry(xPos+218*_scaleRatio,    225*_scaleRatio,104*_scaleRatio,height);
    _backOpacity->setGeometry(xPos+328*_scaleRatio,     225*_scaleRatio,76*_scaleRatio,height);
    _backOpacityValue->setGeometry(xPos+418*_scaleRatio,225*_scaleRatio,80*_scaleRatio,height);
    _subPosMap.insert(Lge->getLangageValue("SetMain/sub") + ":" + LangNoColon("SetSub/backClr"),225*_scaleRatio);
}

void SettingSubtitleWidget::textOpacityChange(int value)
{
    Global->setTextOpacity(value);
}

void SettingSubtitleWidget::textColorChange(const QString &color)
{
    Global->setTextColor(color);
}

void SettingSubtitleWidget::backOpacityChange(int value)
{
    Global->setBgOpacity(value);
}

void SettingSubtitleWidget::backColorChange(const QString &color)
{
    Global->setBgColor(color);
}

void SettingSubtitleWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
