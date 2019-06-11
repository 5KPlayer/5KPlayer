#include "colorbutton.h"
#include <QLabel>
#include <QColor>
#include "combobox.h"
#include "globalarg.h"
#include <QStandardItemModel>

ColorButton::ColorButton(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    _comboBox = new ComboBox(this);
    _comboBox->setVisible(false);
    connect(_comboBox, SIGNAL(currentIndexChanged(QString)), SLOT(showColor(QString)));

    _colorW = new QWidget(this);
    _colorW->setVisible(true);

    const QStringList colorNames = QColor::colorNames();
    QStandardItemModel *model = new QStandardItemModel(this);
    foreach (const QString name, colorNames) {
        QStandardItem *item = new QStandardItem(name);
        item->setToolTip(name);
        model->appendRow(item);
    }
    _comboBox->setModel(model);

    for(int i =0;i<_comboBox->count();++i) {
        QPixmap pix(24*3, 12*3);
        pix.fill(QColor(colorNames.at(i)));
        _comboBox->setItemIcon(i, QIcon(pix));
    }

//    foreach (const QString name, colorNames) {
//        QPixmap pix(28, 16);
//        pix.fill(QColor(name));
//        _comboBox->addItem(QIcon(pix), name);
//    }
    this->setStyleSheet("border:2px solid rgb(115,117,128);");
    updateUI(1.0);
}

void ColorButton::setColor(const QString &str)
{
    _comboBox->setCurrentText(str);
    _colorW->setStyleSheet(QString("background-color: %1;").arg(str));
}

void ColorButton::updateUI(const qreal &scaleRatio)
{
    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);
    font.setWeight(57);
    //font.setBold(true);

    _comboBox->setFont(font);
    _comboBox->setIconSize(QSize(24 * scaleRatio, 12 * scaleRatio));
    _comboBox->upStyleSheet(scaleRatio);
}

void ColorButton::showColor(const QString &txt)
{
    emit valueChanged(txt);
    _colorW->setStyleSheet(QString("background-color: %1;").arg(txt));
    _comboBox->hidePopup();
}

void ColorButton::mousePressEvent(QMouseEvent *)
{
    _comboBox->showPopup();
}

void ColorButton::resizeEvent(QResizeEvent *)
{
    _comboBox->setGeometry(this->rect());
    _colorW->setGeometry(this->rect());
}
