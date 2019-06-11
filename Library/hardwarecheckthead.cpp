#include "hardwarecheckthead.h"
#include "mfx/checkmfx.h"
#include "ffmpeg/displayscreen.h"
#include "globalarg.h"

#include <QFile>
#include <QDebug>

HardwareCheckThead::HardwareCheckThead(QObject *parent) : QThread(parent)
{

}

void HardwareCheckThead::setCheckType(QString hwName)
{
    _hwName = hwName;
}

void HardwareCheckThead::run()
{

    bool isSupport = false;
    if(_hwName == "QSV") {
        CheckMfx mfx;
        isSupport = mfx.checkMfxSupHw();
    } else if(_hwName == "NVIDIA") {
        QString filePath = Global->tempPath().append("/check.mp4");
        QFile::copy(QString(":/res/check.mp4"),filePath);
        isSupport = DisScreen->supQsvDecoder(filePath,"h264_cuvid") == 1;
    } else if(_hwName == "DXVA2") {
        isSupport = true;
    }
    sleep(1);

    emit sendHwSupport(isSupport);
}
