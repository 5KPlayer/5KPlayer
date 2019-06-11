#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include "playermainwidget.h"
#include "globalarg.h"
#include "openfile.h"
#include "localserver.h"
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
bool checkOnly();
#endif

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication app(argc, argv);
    app.setApplicationName("5KPlayer");
    app.setApplicationDisplayName("5KPlayer");

    QFileInfo fi(argv[0]);
    if(fi.baseName() != "5KPlayer") //禁止修改exe的名字
        return -1;

#if QT_NO_DEBUG_ //Release运行
    qInstallMessageHandler(myMessageOutput);
#endif

    QString file("");

    int nArgs = 0;
    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if(nArgs > 1)
    {
        for(int i=1;i<nArgs; i++)
        {
            QString arg = QString::fromStdWString( szArglist[i] ).toLower();

            if(arg == "-auto"  || arg == "/auto" || arg == "auto" ||
                    arg == "exit_9" || arg.indexOf(":/") > 0 || arg.indexOf(":\\") > 0 ||
                    arg.startsWith("//") || arg.startsWith("\\\\")) // 局域网文件
            {
                file = arg;
                break;
            }
        }
    }
    const QString exefile = QString::fromStdWString( szArglist[0] );
    Global->_cur_path = exefile.left(exefile.length() - 13);

    bool b_first = true;

#ifdef Q_OS_WIN
    b_first = checkOnly();
#endif

    LocalServer localServer;

    if(b_first) {
        localServer.createServer();
    } else {
        localServer.connectToServer(file.toUtf8().data());
        return 0;
    }

    if(file == "exit_9")//如果是第一次启动  得退出
        return 0;

    QObject::connect(&localServer, SIGNAL(receiveMsg(QString)),
                     OpenFileDialog, SLOT(openLocalSocketFile(QString)));


    if(file == "-auto" || file == "/auto" || file == "auto")
        PMW->showControl(false);
    else
        PMW->showControl(true);
    app.processEvents();//防止出现播放文件时  界面未同步

    //防止启动时有注册框   quit时崩溃
    QTimer::singleShot(100, [file](){
        PMW->initMediaplayer(file);
    });

    return app.exec();
}

#ifdef Q_OS_WIN
bool checkOnly()
{
    //  创建互斥量
    HANDLE m_hMutex  =  CreateMutex(NULL, FALSE,  "5KPlayer - Digiarty" );
    //  检查错误代码
    if (GetLastError()  ==  ERROR_ALREADY_EXISTS)  {
        //  如果已有互斥量存在则释放句柄并复位互斥量
        CloseHandle(m_hMutex);
        m_hMutex  =  NULL;
        //  程序退出
        return  false;
    }
    else
        return true;
}
#endif

void myMessageOutput(QtMsgType t, const QMessageLogContext &, const QString &msg)
{
    if(t != QtMsgType::QtDebugMsg)
        return;

    static QFile fi("E:/5KPlayer.log");

    if(!fi.isOpen()) {
        fi.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    }
    if(fi.isOpen()) {
        fi.write(QDateTime::currentDateTime()
                 .toString("[yyyy-MM-dd hh:mm:ss.zzz] - ").toLocal8Bit());
        fi.write(msg.toLocal8Bit());
        fi.write("\n");
        fi.flush();
    }
}
