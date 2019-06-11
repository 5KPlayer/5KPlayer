#include "gpuhwcheck.h"
#include <QProcess>
#include <QApplication>
#include <QFileInfo>
#include <globalarg.h>

GpuHwCheck::GpuHwCheck(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<GPUHWINFO>>("QList<GPUHWINFO>");
}

void GpuHwCheck::setGpuInfos(const QStringList &gpuinfos)
{
    _gpuInfos = gpuinfos;
}

QString GpuHwCheck::getfilePath(const QString &file)
{
    return Global->_cur_path + "/video" + file;
}

int GpuHwCheck::checked(QProcess *p_pro, QStringList &args)
{
    if(!QFileInfo::exists(args[0])) {
        return 0;
    }
    p_pro->setArguments(args);
    p_pro->start();
    p_pro->waitForFinished();
    int exitCode = p_pro->exitCode();

    args.clear();
    return exitCode;
}

void GpuHwCheck::run()
{
    QList<GPUHWINFO> gpuhws;
    gpuhws.clear();

    int count = _gpuInfos.size();
    const QString program = Global->_cur_path + "/CheckDecoder.exe";

    for(int i=0;i<count;++i) {
        GPUHWINFO gpu;
        gpu.GpuName = _gpuInfos.at(i);

        if(gpu.GpuName.toLower().startsWith("intel")) {
            for(int j=0;j<4;++j) {
                QString file = getfilePath(filePath[j]);
                _hwSup hw;
                hw.codec = QString(qsvHWcodec[j]);
                QProcess process;
                process.setWorkingDirectory(Global->_cur_path);
                process.setProgram(program);
                QStringList args;
                args << file << qsvHWcodec[j];
                if(checked(&process, args) == 0)
                    hw.supHw = 1;
                else
                    hw.supHw = 0;
                gpu.supHWcodec.append(hw);
            }
            gpuhws.append(gpu);
        } else if(gpu.GpuName.toLower().startsWith("nvidia")) {
            for(int j=0;j<4;++j) {
                QString file = getfilePath(filePath[j]);
                _hwSup hw;
                hw.codec = QString(cuvidHWcodec[j]);
                QProcess process;
                process.setWorkingDirectory(Global->_cur_path);
                process.setProgram(program);
                QStringList args;
                args << file << cuvidHWcodec[j];

                if(checked(&process, args) == 0)
                    hw.supHw = 1;
                else
                    hw.supHw = 0;
                gpu.supHWcodec.append(hw);
            }
            gpuhws.append(gpu);
        }
    }
    emit sendGpuHwInfo(gpuhws);
}
