#ifndef MEDIAADDWIDGET_H
#define MEDIAADDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include "updateinterface.h"
#include "dylabel.h"
#include "dystructu.h"
class mediaAddWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit mediaAddWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void setAddfilePath(const QStringList &pathList);

private:
    void Init();
    void upUI();

signals:
    void sendfilePath(const QStringList &filePath);
    void stopanalyze();

public slots:
    void clickCancel();
    void loadfinish();
    void upLoadWidget(const QList<MEDIAINFO> &mediaList);

private:
    QLabel         *_fileThumbnail; // 完成加载文件缩略图
    QLabel         *_load;          // load label
    QLabel         *_loadpath;      // 完成load的路径
    QLabel         *_progrressLabel;// 完成load数
    QProgressBar   *_progrress;     // 完成load进度条
    DYLabel        *_cancel;        // 取消load按钮
    double          _scaleRatio;
    int             _fileCount;     // 文件总数
    int             _loadFileCount; // 已经加载文件数
};

#endif // MEDIAADDWIDGET_H
