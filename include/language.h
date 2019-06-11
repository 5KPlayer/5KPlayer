#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QObject>
#include "singleton.h"

class QSettings;

#define Lge Language::getInstance()
#define Lang(x) Lge->getLangageValue(x)
#define LangNoColon(x) Global->removeSetColon(Lge->getLangageValue(x))

class Language : public QObject, public Singleton<Language>
{
    Q_OBJECT
    friend class Singleton<Language>;
    friend class QSharedPointer<Language>;
public:
    void initLangage(const QString &file="");
    QString getLangageValue(const QString &key) const;

private:
    Q_DISABLE_COPY(Language)
    explicit Language(QObject *parent = 0);

    QSettings *_configIniRead;
};

#endif // LANGUAGE_H
