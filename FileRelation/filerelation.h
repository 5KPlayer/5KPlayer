#ifndef FILERELATION_H
#define FILERELATION_H

#include <QObject>

//文件关联

class FileRelation : public QObject
{
    Q_OBJECT
public:
    explicit FileRelation(const QString &path, QObject *parent = 0);

    //关联文件
    void registe(const QString &suffix);
    void unregiste(const QString &suffix);
    bool isFileRelation(const QString &suffix);

    //关联DVD
    void registDvd();
    void unregistDvd();
    bool isRegistDvd();

    //添加/移除 右键菜单
    void addRightMenu(const QString &suffix);
    void removeRightMenu(const QString &suffix);

    //安装 / 卸载
    void install();
    void uninstall();

    // 立即生效
    void immediateEffect();

private:
    //备份
    void backupFileRelation(const QString &path, const QString &key);
    //还原
    void restoreFileRelation(const QString &path, const QString &key);

    QVariant value(const QString &path, const QString &key);
    void setValue(const QString &path, const QString &key, const QString &value);
    void removeKey(const QString &path, const QString &key);
    void removeKeyW(const QString &path, const QString &key);

private:
    const QString m_appName;
    const QString m_backupName;
    const QString m_fileRelationName;
    const QString m_appPathName;
    const QString m_command;
    const QString m_defaultIcon;

    QStringList m_videoSuffixList;
    QStringList m_musicSuffixList;
    QStringList m_dvdFileSuffixList;
};

#endif // FILERELATION_H
