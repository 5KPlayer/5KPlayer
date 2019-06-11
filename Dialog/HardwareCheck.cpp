#include "HardwareCheck.h"

#include <QApplication>
#include <QEventLoop>
#include <QDebug>
#include <QProcess>
#include <QDir>

HardwareCheck::HardwareCheck(QObject *parent)
    : QThread(parent)
{
    b_isStoped = false;
}

void HardwareCheck::checkHardware()
{
    start();
}

void HardwareCheck::stopHardware()
{
    b_isStoped = true;
}

void HardwareCheck::run()
{
    QProcess process;
    QString drivers = "DisplayDrivers.exe";
    QString intel = "IntelCheck.exe";
    QString nvidia = "NvidiaCheck.exe";
    QString amd = "AMDCheck.exe";
    QByteArray baDrivers;

    /*No buffer*/
    process.setWorkingDirectory(QApplication::applicationDirPath());
    setvbuf(stdout, (char *)NULL, _IONBF, 0);

    /*Display Drivers*/
    process.start(drivers);
    if (!process.waitForFinished(10 * 1000)) {
        process.close();
        msleep(1000);
        emit doShowInfo(QVariant(false));
        return;
    } else {
        baDrivers = process.readAllStandardOutput();
    }
    process.close();
    qDebug() << "output: " << baDrivers;
    //baDrivers.append("pro:Intel\tname:intel 3350\tver:25.21.14.1735\r\n");
    emit doShowInfo(baDrivers);
    msleep(20);

    QString data = QString::fromUtf8(baDrivers);
    QStringList items = data.split("\r\n", QString::SkipEmptyParts);
    for (int i = 0; i < items.count(); ++i) {
        QString item = items.at(i).trimmed();
        if (item.startsWith("pro:I", Qt::CaseInsensitive)) {
            /*Intel Check*/
            process.start(intel);
            while(process.state() != QProcess::NotRunning) {
                if(b_isStoped) {
                    process.close();
                    process.waitForFinished();
                    break;
                }
                if (!process.waitForFinished(100))
                    continue;
                QByteArray error = process.readAllStandardError();
                if (!error.trimmed().isEmpty()) {
                    emit doShowInfo(QVariant("IError"));
                    break;
                }
                QString output = QString::fromUtf8(process.readAllStandardOutput());
                emit doShowInfo(QVariant("I" + output));
                break;
            }
            process.close();
            msleep(20);
        } else if (item.startsWith("pro:N", Qt::CaseInsensitive)) {
            /*Nvidia Check*/
            process.start(nvidia);
            while(process.state() != QProcess::NotRunning) {
                if(b_isStoped) {
                    process.close();
                    process.waitForFinished();
                    break;
                }
                if (!process.waitForFinished(100))
                    continue;
                QByteArray error = process.readAllStandardError();
                if (!error.trimmed().isEmpty()) {
                    emit doShowInfo(QVariant("NError"));
                    break;
                }
                QString output = QString::fromUtf8(process.readAllStandardOutput());
                emit doShowInfo(QVariant("N" + output));
                break;
            }
            process.close();
            msleep(20);
        } else if (item.startsWith("pro:A", Qt::CaseInsensitive)) {
            /*AMD Check*/
            process.start(amd);
            while(process.state() != QProcess::NotRunning) {
                if(b_isStoped) {
                    process.close();
                    process.waitForFinished();
                    break;
                }
                if (!process.waitForFinished(100))
                    continue;
                QByteArray error = process.readAllStandardError();
                if (!error.trimmed().isEmpty()) {
                    emit doShowInfo(QVariant("AError"));
                    break;
                }
                QString output = QString::fromUtf8(process.readAllStandardOutput());
                emit doShowInfo(QVariant("A" + output));
                break;
            }
            process.close();
            msleep(20);
        }
    }

    emit doShowInfo(QVariant(true));
    qDebug() << "Check Finished.";
}

