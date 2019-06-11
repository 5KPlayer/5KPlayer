#ifndef DLNAMANAGER_H
#define DLNAMANAGER_H

#include <QObject>
#include <QLabel>
#include "singleton.h"
#include "dystructu.h"
#include "devstruct.h"
struct CFileAtt;
#define DLNA DLNAManager::getInstance()
class DLNAManager : public QObject,public Singleton<DLNAManager>
{
    Q_OBJECT
    friend class Singleton<DLNAManager>;
    friend class QSharedPointer<DLNAManager>;
public:
    DLNAManager(QObject *parent = 0);
    ~DLNAManager();

    void addOrDelVirFolder(QString name,bool actState); // 添加删除虚拟文件夹 actState 删除
    void addOrDelFolder(QString path,bool actState);
    void addFilePaths(QStringList paths);

    QList<dlnaLib> getDLnaData();

    bool isShsreDLna(QString name);                     // 该目录是否已经加入dlna共享
    bool libInfoCompar(dlnaLib info,dlnaLib info1);     // 比较2个是否相同
    bool libInfoSupper(dlnaLib info);

    void removeDataInSel();
    void removeSelForData(dlnaLib info);
    bool pathIsExitsData(QString path);
    QStringList pathsIsExitsData(QStringList paths);

    void openFolderPath(QString path);
    void openFilePath(QString path);
    void openVirFolder(QString name);
    void initServer();

    QString getCurItem(QString item);
    void addVirFile(MEDIAINFO media, QString item);
    void delVirFile(MEDIAINFO media, QString item);
    void delVirFolder(QString name);

    QString getVirTextInfo(QLabel* lab,QString text);
    QString getCurShowText(QLabel* lab,QString text,QString spl = " ",int line = 2);
    void setShareMaxRows(int max, bool isUp);
    int  getShareMaxRows();
    void setShareMaxCols(int max);
    int  getShareMaxCols();
    void addDargLibForMedia(QList<MEDIAINFO> list);
    bool getDLnaDetailVisible();
private:
    CFileAtt* createVirDir(QString name);
    CFileAtt* createVirFile(MEDIAINFO media, CFileAtt *parent);
    void addVirFolder(QString name);
    void removeVirFolder(QString name);
    void initDlnaData();
    bool isExitDlnaData(dlnaLib lib);
private slots:
    void addDLnaListSlt(int count,QList<dlnaLib> dlnaList);
    void sendGetDeviceSupSlt(deviceInfo dev);
signals:
    void upShareWidget(bool isWUp);
    void upShareLeft(bool isAdd);
    void virFolderChange(dlnaLib dlnaInfo,bool actState);
    void virFolderDClick(QString name);
    void analyzefinish();
    void upAnalyzeInfo(const QList<dlnaLib> &dlnaList);
    void stopAnalyzeFile();
public:
    QList<dlnaLib> _selDlnaData;
private:
    QList<dlnaLib>      _dlnaData;
    QList<CFileAtt*>    _virFolderList;
    QString             _language;
    int                 _addLibPos;
    int                 _maxRows;
    int                 _maxCols;
};

#endif // DLNAMANAGER_H
