#include "filerelation.h"
#include <QSettings>

// mp4 iso mp3等关联

FileRelation::FileRelation(const QString &path, QObject *parent) : QObject(parent),
    m_appName("5KPlayer"),
    m_backupName(m_appName + ".backup"),
    m_fileRelationName(m_appName + ".xxx"),
    m_appPathName(path + "\\" + m_appName),
    m_command(QString("\"%1.exe\"").arg(m_appPathName) + " \"%1\""),
    m_defaultIcon(QString("\"%1.exe\",0").arg(m_appPathName))
{
}

void FileRelation::registDvd()
{
    //第一步 创建DVD的参数
    install();

    // user:   HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers
    // local: HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers

    //第二步 创建 DVD 播放信息
    const QString userPath("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows"
                           "\\CurrentVersion\\Explorer\\AutoplayHandlers\\");

    const QString path2 = userPath + "Handlers\\" + m_appName + "DVDMovieOnArrival";

    setValue(path2, "Action",       m_appName + " Play DVD");
    setValue(path2, "DefaultIcon",  m_defaultIcon);
    setValue(path2, "InvokeProgID", m_fileRelationName);
    setValue(path2, "InvokeVerb",   "Open");
    setValue(path2, "Provider",     m_appName + " media player");

    //第三步 将DVD播放信息 加入到系统列表
    const QString path3 = userPath + "EventHandlers\\PlayDVDMovieOnArrival";
    setValue(path3, m_appName + "DVDMovieOnArrival", "");
}

void FileRelation::unregistDvd()
{
    const QString userPath("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows"
                           "\\CurrentVersion\\Explorer\\AutoplayHandlers\\");
    //第一步 删除 DVD 播放信息
    removeKey(userPath  + "Handlers",
              m_appName + "DVDMovieOnArrival");

    //第二步 移除DVD播放列表
    removeKey(userPath  + "EventHandlers\\PlayDVDMovieOnArrival",
              m_appName + "DVDMovieOnArrival");
}

bool FileRelation::isRegistDvd()
{
    const QString userPath("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows"
                           "\\CurrentVersion\\Explorer\\AutoplayHandlers\\");

    const QString path1 = userPath + "EventHandlers\\PlayDVDMovieOnArrival";

    QSettings set1(path1, QSettings::NativeFormat);
    if(!set1.contains(m_appName + "DVDMovieOnArrival"))
        return false;

    const QString path2 = userPath + "Handlers\\" + m_appName + "DVDMovieOnArrival";

    QSettings set2(path2, QSettings::NativeFormat);
    if(!set2.contains("InvokeProgID"))
        return false;

    return true;
}

void FileRelation::addRightMenu(const QString &suffix)
{
    if(suffix.toLower() == "dvd") {
        setValue("HKEY_CLASSES_ROOT\\DVD\\shell\\PlayWith" + m_appName,
                 ".", "Play with " + m_appName);

        setValue("HKEY_CLASSES_ROOT\\DVD\\shell\\PlayWith" + m_appName + "\\command",
                 ".", m_command);
        return;
    }
    const QString path = "HKEY_CLASSES_ROOT\\SystemFileAssociations\\" + suffix + "\\Shell\\" + m_appName;
    setValue(path, "Icon", m_defaultIcon);
    setValue(path + "\\Command", ".", m_command);
}

void FileRelation::removeRightMenu(const QString &suffix)
{
    if(suffix.toLower() == "dvd") {
        removeKey("HKEY_CLASSES_ROOT\\DVD\\shell", "PlayWith" + m_appName);
        return;
    }
    removeKey("HKEY_CLASSES_ROOT\\SystemFileAssociations\\" + suffix + "\\Shell", m_appName);
}

void FileRelation::install()
{
    static int i = 0;
    if(i > 0) return;
    ++i;

    const QString path = "HKEY_CLASSES_ROOT\\" + m_fileRelationName;
    //  setValue(path, ".", m_appName);
    setValue(path + "\\DefaultIcon", ".", m_defaultIcon);
    //  setValue(path + "\\shell\\open", ".", m_appName);
    setValue(path + "\\shell\\open\\command", ".", m_command);
    setValue(path + "\\shell\\open", "FriendlyAppName", m_appName);
}

void FileRelation::uninstall()
{
    //反注销所有关联
    removeKey("HKEY_CLASSES_ROOT", m_fileRelationName);
}

void FileRelation::registe(const QString &suffix)
{
    install();

    const QString subpath = "HKEY_CLASSES_ROOT\\" + suffix;

    // 第1步 默认HKEY_CLASSES_ROOT下的文件关联
    backupFileRelation(subpath, ".");//先备份 在设置
    setValue(subpath, ".", m_fileRelationName);

    //第2步  默认HKEY_CURRENT_USER下的文件关联
    const QString mKey2 = "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows"
                          "\\CurrentVersion\\Explorer\\FileExts\\" + suffix;

    // HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion
    // \\Explorer\\FileExts\\.xxx\\OpenWithProgids 添加 5KPlayer.xxx
    // 添加右键可播放列表
    setValue(mKey2 + "\\OpenWithProgids", m_fileRelationName, "");

    // HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion
    // \\Explorer\\FileExts\\.xxx  [Progid = 5KPlayer.xxx]

    //设置默认关联
    backupFileRelation(mKey2, "Progid");
    setValue(mKey2, "Progid", m_fileRelationName);

    //第3步 删除系统关联 (无效,只能手动删除)
    const QString mKey3 = "SOFTWARE\\Microsoft\\Windows"
                          "\\Roaming\\OpenWith\\FileExts\\" + suffix;

    removeKeyW(mKey2.mid(18), "UserChoice");
    removeKeyW(mKey3, "UserChoice");
}

bool FileRelation::isFileRelation(const QString &suffix)
{
    const QString subpath1 = "HKEY_CLASSES_ROOT\\" + suffix;
    const QString v1 = value(subpath1, ".").toString();

    const QString subpath2 = "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows"
                             "\\CurrentVersion\\Explorer\\FileExts\\" + suffix;
    const QString v2 = value(subpath2, "Progid").toString();

    if(v1 == v2 && v1 == m_fileRelationName)
        return true;

    return false;
}

void FileRelation::unregiste(const QString &suffix)
{
    //第一步  还原默认设置
    restoreFileRelation("HKEY_CLASSES_ROOT\\" + suffix, ".");

    const QString path = "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows"
                         "\\CurrentVersion\\Explorer\\FileExts\\" + suffix;

    restoreFileRelation(path, "ProgId");

    //删除右键中的可播放列表
    removeKey(path + "\\OpenWithProgids", m_fileRelationName);
}

//备份注册表
void FileRelation::backupFileRelation(const QString &path, const QString &key)
{
    const QVariant v = value(path, key);
    if(v.toString() == m_fileRelationName)
        return;

    setValue(path, m_backupName, v.toString());
}

//还原注册表
void FileRelation::restoreFileRelation(const QString &path, const QString &key)
{
    QSettings set(path, QSettings::NativeFormat);
    if(set.value(key).toString() != m_fileRelationName) {
        return;
    }

    if(set.contains(m_backupName)) {
        const QVariant v = set.value(m_backupName);
        set.setValue(key, v);
        set.remove(m_backupName);
    }
}

QVariant FileRelation::value(const QString &path, const QString &key)
{
    QSettings set(path, QSettings::NativeFormat);
    if(set.contains(key))
        return set.value(key);
    return QVariant();
}

void FileRelation::setValue(const QString &path, const QString &key, const QString &value)
{
    QSettings set(path, QSettings::NativeFormat);
    set.setValue(key, value);
}

void FileRelation::removeKey(const QString &path, const QString &key)
{
    QSettings set(path, QSettings::NativeFormat);
    //if(set.contains(key))
    set.remove(key);
}

#include <windows.h>
void FileRelation::immediateEffect()
{
    ::SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
    //::SendMessage((HWND)0x08000000, 0x0000, 0, 0);
}

void FileRelation::removeKeyW(const QString &path, const QString &key)
{
    HKEY hKey;

    LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER,
         path.toStdString().c_str(),
        0, KEY_QUERY_VALUE|KEY_WRITE, &hKey );
    if( lRet == ERROR_SUCCESS )
    {
        RegDeleteKey(hKey,  key.toStdString().c_str());
        RegCloseKey(hKey);
    }
}
