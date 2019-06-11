#include "language.h"
#include "globalarg.h"
#include <QFile>
#include <QSettings>
#include <QApplication>
#include <QDebug>

Language::Language(QObject *parent)
    : QObject(parent)
{
    _configIniRead = NULL;
    initLangage();
}

void Language::initLangage(const QString &file)
{
    QString curLanguage;
    if(file.isEmpty()) {
        curLanguage = Global->getLanguage();
    } else {
        curLanguage = file;
    }

    if(_configIniRead) {
        delete _configIniRead;
        _configIniRead = NULL;
    }

    _configIniRead = new QSettings(QString(":/Language/%1.ini").arg(curLanguage), QSettings::IniFormat, this);
    _configIniRead->setIniCodec("utf-8");
}

QString Language::getLangageValue(const QString &key) const
{
    return _configIniRead->value(key).toString();
}
