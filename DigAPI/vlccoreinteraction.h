#ifndef VLCCOREINTERACTION_H
#define VLCCOREINTERACTION_H

#include <QObject>
class DYMediaPlayer;

class VLCCoreInteraction
{
public:
    VLCCoreInteraction(DYMediaPlayer *player);

    void hflip();      //垂直镜像
    void vflip();      //水平镜像
    void closeFlip();  //关闭镜像

    void leftRotate(); //逆时针旋转+90
    void rightRotate();//顺时针旋转+90
    void closeRotate();//关闭旋转

    QString curState() const {return _state;}

    QString toUrl(const QString &s);

    void test();

private:
    Q_DISABLE_COPY(VLCCoreInteraction)

    void rotate(const int &);//角度旋转
    void flip(int type);     //镜像

private:
    int _rotateAngle = 0;
    QString _state = "";
};

#endif // VLCCOREINTERACTION_H
