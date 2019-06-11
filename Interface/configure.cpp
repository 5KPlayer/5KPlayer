#include "configure.h"
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include "globalarg.h"

Configure::Configure(QObject *parent) : QObject(parent)
{
    const QString iniFile = getConfigPath();
    p_set = new QSettings(iniFile, QSettings::IniFormat);
    p_set->setIniCodec("utf-8");

    _localSaveVideoFormat = p_set->value("Format/Video").toString();
    _localSaveAudioFormat = p_set->value("Format/Music").toString();
}

QString Configure::getUserVideoFormat()
{
    return _videoFormat + " " + _localSaveVideoFormat;
}

QString Configure::getUserAudioFormat()
{
    return _audioFormat + " " + _localSaveAudioFormat;
}

void Configure::saveUserVideoFormat()
{
    p_set->beginGroup("Format");
    p_set->setValue("Video", _localSaveVideoFormat);
    p_set->endGroup();
}

void Configure::saveUserAudioFormat()
{
    p_set->beginGroup("Format");
    p_set->setValue("Music", _localSaveAudioFormat);
    p_set->endGroup();
}

void Configure::addVideoFormat(const QString &videoFormat)
{
    if(_videoFormat.contains(videoFormat.toLower()) ||
       _localSaveVideoFormat.contains(videoFormat.toLower()) ||
       Global->mirrorFiles().contains(videoFormat.toLower()))
        return;

    _localSaveVideoFormat.append(" *.").append(videoFormat.toLower());
    saveUserVideoFormat();
}

void Configure::addAudioFormat(const QString &audioFormat)
{
    if(_audioFormat.contains(audioFormat.toLower()) ||
       _localSaveAudioFormat.contains(audioFormat.toLower()) ||
       Global->mirrorFiles().contains(audioFormat.toLower()))
        return;

    _localSaveAudioFormat.append(" *.").append(audioFormat.toLower());
    saveUserAudioFormat();
}

QString Configure::getConfigPath() const
{
    QString configPaht = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir temp;
    if(!temp.exists(configPaht))
       temp.mkdir(configPaht);

    configPaht.append("/Config.ini");

    return configPaht;
}

QVariant Configure::getConfigVar(const QString &Key)
{
    return p_set->value(Key);
}

void Configure::setConfigVar(const QString &Key, const QVariant &value)
{
    p_set->setValue(Key, value);
}
