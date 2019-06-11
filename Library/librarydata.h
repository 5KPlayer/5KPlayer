#ifndef LIBRARYDATA_H
#define LIBRARYDATA_H

#include <QObject>
#include <QProcess>
#include "singleton.h"
#include "dystructu.h"
#include "qzeroconf.h"
class QThread;
class QtSql;
class AddMediaThead;
class DelMediaThead;

#define FOLDERDEPTH     5       // 遍历文件夹的深度
#define RECENTCOUNT     5       // 历史记录的条数

#define LibData LibraryData::getInstance()

class LibraryData : public QObject,public Singleton<LibraryData>
{
    Q_OBJECT
    friend class Singleton<LibraryData>;
    friend class QSharedPointer<LibraryData>;
public:

    void InitData();
    void InitMusic();

    /*!
     * \brief 得到视频媒体信息列表
     * \return
     */
    QList<MEDIAINFO> getmedaiData();
    /*!
     * \brief 得到音乐媒体信息列表
     * \return
     */
    QList<MEDIAINFO> getmusicData();
    /*!
     * \brief 得到文件夹信息列表
     * \return
     */
    QList<QString>   getitemNames();

    /*!
     * \brief 添加和删除文件夹
     * \param name
     * \return
     */
    bool addItem(const QString &name);
    void deItem(const QString &itemName);

    /*!
     * \brief 添加、删除、更新媒体
     * \param mediainfo
     */
    void addpathList(const QStringList &paths, const QString &itemName = "");
    bool addmedia(const QString &path, const QString &itemName = "", bool convert = false);
    void addDropMediaList(const QList<MEDIAINFO> &medialist);
    bool delmedia(const QList<MEDIAINFO> &medialist, bool isSelectAll, bool bup, bool ismove);
    void upmedia(const MEDIAINFO &mediainfo, bool upList = false);

    /*!
     * \brief 设置当前选中的文件夹信息
     * \param item
     */
    void setselectitem(const QString &item);
    QString getselectitem();

    /*!
     * \brief setplayItem
     * \param 设置当前的item为播放时的item，在播放上一个和下一个时，根据播放时的item来得到上一个和下一个的媒体
     */
    void setplayItem(const QString &item);

    QList<MEDIAINFO> getMediaforItem(const QString &item); // 得到指定文件夹中的媒体列表

    /*!
     * \brief 得到,清除和设置播放媒体
     * \return
     */

    MEDIAINFO getplayinfo();
    void clearPlayinfo();       // 清除播放媒体信息，
    QString getcurmediaPath();
    bool isHDCurVideo();
    bool setplaymedia(const MEDIAINFO &mediainfo);

    QString getCurMediaCodec();         // 获取当前播放媒体的codec

    /*!
     * \brief moves列表，里面需要去掉名字和路径相同的但所属目录不同的media
     * \return
     */
    QList<MEDIAINFO> movesMediaList(const QList<MEDIAINFO> &List);

    /*!
     * \brief removeItemForList
     * \param List
     * \param 从列表中移除一个项目
     * \return
     */
    QList<MEDIAINFO> removeItemForList(QList<MEDIAINFO> &List, const MEDIAINFO &mediainfo);

    enum MusicSort{
        Title = 0,
        Artist,
        Data
    };

    void setMusicSort(MusicSort t);
    MusicSort getMusicSort() {return _sortMode;}
    void SortMusic(QList<MEDIAINFO> *medialist);

    /*!
     * \brief getfileSize
     * \param 通过传入文件大小得到显示的字符串
     * \return
     */
    QString getfileSize(double fileSize, int digit);
    /*!
     * \brief getfileTime
     * \param 通过传入时间得到显示时间
     * \return
     */
    QString sectotime(int duration);
    /*!
     * \brief getaspectRatio
     * \param 得到媒体的宽高比
     * \return
     */
    QSize getaspectRatio(const MEDIAINFO &media);
    /*!
     * \brief setmusiclist
     * \param 音乐排序后更新data
     */
    void setmusiclist(const QList<MEDIAINFO> &musiclist);

    /*!
     * \brief getCurData
     * \return  获取当前的系统时间
     */
    double getCurData();

    /*!
     * \brief filePin
     * \param path
     * \return 文件的唯一标示
     */
    QString getfilePin(const QString &path);

    STREAMINFO getVideoStream(const MEDIAINFO &media);
    STREAMINFO getMusicStream(const MEDIAINFO &media);

    MEDIAINFO getnextmedia(bool isAuto);   //得到播放列表的下一个视频
    MEDIAINFO getprevmedia(bool isAuto);   //得到播放列表的上一个视频

    bool checkMediaExits(const QString &mediaPath);       //检查当前路径媒体是否存在
    void upmediaPlay(); //更新播放媒体
    void cleanUpList(); //清除媒体不存在的列表；

    void mediaListMove(const MEDIAINFO &media1, const MEDIAINFO &media2);

    QList<MEDIAINFO> getItemShowList();     //初始化时获取显示的列表
    MEDIAINFO        getNextShow(int showcount);         //鼠标向下滚动时，加载的媒体

    int  getMaxCount(){return _showMaxCount;}
    void setMaxCount(int maxCount, bool up); //设置当前页最大显示数量并更新

    enum FormatType{
        Video = 0,
        Audio
    };

    void clearShowItemName(){emit clearShowItem();} // 清除所有widget的当前的item重新加载数据

    /*!
     * \brief clearPicturefile
     * \param 判断文件是否是图片格式
     * \return
     */
    bool checkPicturefile(const QString &pathList);

    void clearFileInItemName(QStringList *pathList);

    /*!
     * \brief getFilePathonFolder
     * \param 获取文件夹中的文件路径
     * \return
     */
    QStringList getFilePathonFolder(const QString &folderpath);
    QStringList getSubFilePathOnFolder(const QString &folderpath,bool &autodetect);
    /*!
     * \brief setRootPath
     * \param rootpath
     * \return 设置文件夹的根目录，用来判断当前目录是根目录的多少层
     */
    void setRootPath(const QString &rootpath);
    /*!
     * \brief checkFolder
     * \param folderpath
     * \return 检查文件夹的目录层级是否大于定义的层级
     */
    bool        checkFolder(const QString &folderpath);
    /*!
     * \brief getValidFilePath
     * \param 获取路径列表中有效文件的路径
     * \return 有效文件路径：1、去掉所有文件后缀名是图片格式的文件路径
     *                    2、只包含用户使用的文件格式的文件路径
     */
    QStringList getValidFilePath(const QStringList &pathList);


    QString getVideoAspectRatio(const QString &FolderPath);    // 获取DVD目录下视频的宽高比

    QList<MEDIAINFO> getUrlMedia();
    void upUrlMediaList(const QList<MEDIAINFO> mediaList);

    void sortResolution(QList<STREAMINFO>* streamList); // 按照分辨率进行排序
    void sortFormat(QList<STREAMINFO>* videoformat);     // 按照seting的设置进行排序
    bool checkYtbDeploy();                           // 检查ytb的环境是否部署


    void addRecent(const QString &filePath);
    void clearRecent();
    QStringList getRecent();

    /*!
     * \brief addtomediaList
     * \param mediaList
     * \return true:不存在于medialist，
     *         false:存在于mediaList
     */
    bool   checkMediaisUp(QList<MEDIAINFO>*mediaList, const MEDIAINFO &media);
    /*!
     * \brief checkMediaisAirPlay
     * \param media
     * \return 检查媒体是否支持airplay，true支持，支持的格式为codec为H264或者后缀为MP4或者为音乐
     */
    bool   checkMediaisAirPlay(const MEDIAINFO &media);

    QList<NetInfo> getUdpList() {return _udpList;}
    QList<QZeroConfService*> getSevList() {return _bjSevList;}

    void airDisConnect();       // 断开连接，udpClient 需要清空
    void setUdpClient(QZeroConfService *netInfo); // 选中时记录选择的udpClient信息
    QZeroConfService getUdpClient();

    void setUdpServer(NetInfo netInfo); // 设置server信息
    void clearUdpServer();              // 断开连接时清除server信息
    NetInfo getUdpServer();

    void getlocalIp();                  // 获取本机的IP;
    bool checkIsLocalIp(const QString &Ip);    // 检查当前Ip地址是否是本机IP地址 是返回true

    void disScreenprotection();         // 禁用屏幕保护
    void recScreenprotection();         // 恢复屏幕保护
    //双击播放
    bool b_doubleClickPlay;

    bool b_isConCPT;        // 是否连接到其他计算机
    bool b_isCPTCon;        // 是否被其他计算机连接

    void setSearchText(QString searchText);  // 设置SearchEdit记录的值
    bool isConstItem(QString item);          // 是否是固定的列表

    QImage getSelPictures();
    bool   selectItemIsDrop();
private:
    Q_DISABLE_COPY(LibraryData)
    explicit LibraryData(QObject *parent = 0);

    void delGroup(const QString &itemName);    // 删除媒体列表中的一个文件夹的媒体

    QList<MEDIAINFO> getValidList(const QList<MEDIAINFO> &media); // 获取列表中的有效列表

    QString getpath(const MEDIAINFO &media);   // 获取媒体的路径

    //bool delFileToRecycleDlg(QStringList pathList);// 删除文件到回收站
    bool checkFormat(const QString &formatList, const QString &format);
    bool checkUrlFile(QString path);        // 检查是否是网络文件，网络文件不加入播放列表
signals:
    void upvideoList(bool bUp,bool bconvert,QString);
    void upplaymedia(MEDIAINFO);
    void upresize();
    void clearShowItem();               // 添加和删除文件后，需要重新加载
    void stopAnalyzeFile();             // 取消分析文件
    void analyzefinish();               // 分析完成
    void upAnalyzeInfo(QList<MEDIAINFO> mediaList); // 发送当前分析的信息

    void sendProcessValue(QString objName,double time); // 发送进度信息
    void upUdpClientList();               //发送更新当前udpList
    void upWidgetAirPlay(bool);           //发送airplay连接状态
public slots:
    void addmediaListSlt(int count, const QList<MEDIAINFO> &list);        // 添加文件返回消息的槽函数
    void searchChange(const QString &search);

    void addNetList(NetInfo udpInfo);
    void removeNet(QString udpIp);
    void addBjSev(QZeroConfService* service);
    void delBjSev(QZeroConfService* service);
protected:
    //void run();
private:
    double analyzeProess(QByteArray ba);
public:
    QList<MEDIAINFO> selmedialist;//选中媒体列表
private:
    QList<MEDIAINFO> _videoList;    // 视频媒体的列表
    QList<MEDIAINFO> _musicList;    // 音乐媒体的列表
    QList<MEDIAINFO> _urlMedia;     // 网络媒体列表
    QList<MEDIAINFO> _mediaList;     // 显示媒体列表
    int              _addListPos;   // 插入数据的位置
    int              _showMaxCount; // 当前页面最大显示媒体数量
    QString          _searchInfo;   // 选择edit的信息
    QList<QString>   _itemList;     // 新增加的文件夹名字列表
    MEDIAINFO        _curplaymedia; // 当前播放的媒体
    QString          _curSelectItem;  // 当前选中的文件夹名字
    QString          _curPlayItem;     // 播放的文件夹，这里根据这个文件夹来选择播放的上一个和下一个的文件
    MusicSort        _sortMode;     // 音乐排序模式
    QString          _rootPath;     // 遍历文件夹的根目录

    QList<NetInfo>      _udpList;       // 连接的udp列表

    NetInfo             _udpServer;     // 连接的udpserver信息
    QZeroConfService    _udpClient;     // 连接的udpClient信息

    QStringList         _recent;        // 历史记录列表

    QList<QZeroConfService*> _bjSevList;
    QStringList              _localIps;


    const QString   _fileUnit[5]{"B","KB","MB","GB","TB"};
    const QString   _pictureFormat = "*.bmp *.gif *.jpeg *.jpeg2000 *.jpg *.ico *.tiff *.psd *.png *.ogv *.svg *.pcx\
                                     *.dxf *.wmf *.emf *.fli *.flc *.eps *.tga *.pbm *.pgm *.ppm *.xbm *.xpm";
};

#endif // LIBRARYDATA_H
