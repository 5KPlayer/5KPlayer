#ifndef DISPLAYASPECTRATIO_H
#define DISPLAYASPECTRATIO_H

#include <QString>
#include "dystructu.h"

class DisplayAspectRatio
{
public:
    explicit DisplayAspectRatio();
    ~DisplayAspectRatio();
    int getmediainfo(const QString &fileName, MEDIAINFO* media);
private:
    QString ffmpeg(const QString &);
    int gcd(const int &, const int &) const;//计算最大公约数
    QString char2Qstring(char* value);          // 判断字符的编码，进行转换
};

#endif // DISPLAYASPECTRATIO_H
