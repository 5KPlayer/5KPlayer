#ifndef EDITCURWIDGET_H
#define EDITCURWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>
#include "picturebrowserwidget.h"
#include "viewportwidget.h"
#include "previewwidget.h"
#include <QMap>
#include "decodethread.h"
#include "decodethreadmanager.h"
#include "QVector"
#include <QScroller>
#include <QSlider>
#include "updateinterface.h"
#include <QTimer>
#include <QTime>

class EditCutWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
    Q_PROPERTY(ItemShowMode showMode READ showMode WRITE setShowMode NOTIFY showModeChanged)
public:
    enum ItemShowMode{
        StackedMode,
        TiledMode,
    };
    Q_ENUM(ItemShowMode)
    explicit EditCutWidget(QString filePath, int64_t curTime, QWidget *parent = 0);
    ~EditCutWidget();
    virtual void updateLanguage();
    virtual void updateUI(const qreal &scaleRatio);
    void upUI();
    void dragSizeChange(bool b);

    ItemShowMode showMode();
    void setShowMode(ItemShowMode mode);
protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool event(QEvent *event);
    void keyPressEvent(QKeyEvent *event);
signals:
    void scrollReachedEnd();
    void scrollReachedStart();
    void sendSelPts(double startPts,double endPts);
    void showModeChanged();
    void timeLineIsChangeScale();
public slots:
    void onScrollReachedEnd();
    void onScrollReachedStart();
    void onGetFrames(int index, FRAME frame);
    void onScrollAreaValueChanged(int value);
    void onGetMediaInfo(MediaEditInfo info);
private:
    QScrollArea *_scrollContent;    //滚动区域
    PictureBrowserWidget *_browser; //图片容器
    ViewportWidget *_viewport;      //视窗
    PreviewWidget *_preview;        //预览窗口
    QSlider *_scrollBar;         //滚动条
    MediaEditInfo _mediaInfo;       //媒体信息
    QString _filePath;          //文件路径
    int64_t _curTime;           //传入的当前播放时间
    int64_t _maxFrameCount;     //能显示的最多的图片张数
    int64_t _minFrameCount;     //最少需要显示的图片张数
    DecodeThreadManager *_threadManager;    //解码线程管理器
    QMutex _mutex;
    QSize _itemSize;            //每个item的大小
    QSize _pictureSize;
    int _itemFixedWidth;
    QScroller *scroller;
    int64_t _leftMidPts = -1;
    int64_t _rightMidPts = -1;
    void init();
    double _scaleRatio;
    int _currentPicNum = 0;
    QTimer *_initItemTimer;
    QTimer *_initPreviewTimer;
    bool _isScaleChanged = false;
    ItemShowMode _showMode;
    //效率测试代码
//    QSet<int> testList;
//    QTime testTime;
};

#endif // EDITCURWIDGET_H
