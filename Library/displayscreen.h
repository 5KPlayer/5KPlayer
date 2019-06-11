#ifndef DISPLAYSCREEN_H
#define DISPLAYSCREEN_H

#include <QObject>
#include "singleton.h"
/*
 * 以前用于获取当前时间的视频图片，现在放到previewThead这个线程中了
 *  这个类现在是用于检查是否支持指定的解码器解码
 *
*/
#define DisScreen displayScreen::getInstance()
class displayScreen : public QObject, public Singleton<displayScreen>
{
    Q_OBJECT
    friend class Singleton<displayScreen>;
    friend class QSharedPointer<displayScreen>;

public:
    explicit displayScreen(QObject *parent = 0);

    int supQsvDecoder(const QString &file, const QString &codeName);
};

#endif // DISPLAYSCREEN_H
