#ifndef SETTINGSUBTITLEWIDGET_H
#define SETTINGSUBTITLEWIDGET_H

#include "updateinterface.h"
#include "transeventlabel.h"
#include <QMap>
class QSpinBox;
class ComboBox;
class ColorButton;

class SettingSubtitleWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SettingSubtitleWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    int  getSubPosForKey(QString str);
    QStringList getSubKeys();
private:
    void Init();
    void upUI();
private slots:
    void textOpacityChange(int value);
    void textColorChange(const QString &color);
    void backOpacityChange(int value);
    void backColorChange(const QString &color);
protected:
    virtual void resizeEvent(QResizeEvent *);
private:
    TransEventLabel* _title;
    TransEventLabel* _titleLine;
    TransEventLabel* _titleLine2;

    TransEventLabel* _encoding;
    ComboBox*        _encodingBox;
    TransEventLabel* _font;
    ComboBox*       _fontBox;
    TransEventLabel* _fontSize;
    ComboBox*       _fontSizeBox;
    TransEventLabel* _autofuzzy;
    ComboBox*       _autofuzzyBox;

    TransEventLabel* _textColor;
    ColorButton*     _textColorBtn;
    TransEventLabel* _textOpacity;
    QSpinBox*        _textOpacityValue;

    TransEventLabel* _backColor;
    ColorButton*     _backColorBtn;
    TransEventLabel* _backOpacity;
    QSpinBox*        _backOpacityValue;

    double           _scaleRatio;
    QMap<QString,int>_subPosMap;
};

#endif // SETTINGSUBTITLEWIDGET_H
