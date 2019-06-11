#include "editcutwidget.h"
#include <QDebug>
#include <QBuffer>
#include <QMouseEvent>

#include <QWheelEvent>
#include <QThreadPool>
#include "decodethread.h"
#include <QKeyEvent>
#include <QTimer>
#include "globalarg.h"
#include "playermainwidget.h"
#include "librarydata.h"



EditCutWidget::EditCutWidget(QString filePath,int64_t curTime,QWidget *parent) : UpdateInterfaceWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    qRegisterMetaType<FRAME>("FRAME");
    qRegisterMetaType<MediaEditInfo>("MediaEditInfo");
    qRegisterMetaType<GetFramesThread*>("GetFramesThread");
    _filePath = filePath;
    _curTime = curTime;
    _maxFrameCount = 1000;  //默认最多能显示的图片张数为1000张
    _minFrameCount = 1;     //默认最少能显示的图片张数为1张
    _itemSize = QSize(66*Global->_screenRatio,66*Global->_screenRatio);
    _pictureSize = _itemSize;
    _scaleRatio = Global->_screenRatio;
    _currentPicNum = 100;
    _itemFixedWidth = 40 * _scaleRatio;
    _showMode = TiledMode;
    //解码线程数量控制:1-6个
    int maxThreadCount = 1;
    if(QThread::idealThreadCount() <= 1)
        maxThreadCount = 1;
    else if(QThread::idealThreadCount() < 6)
        maxThreadCount = QThread::idealThreadCount() - 1;
    else
        maxThreadCount = 6;
    CardType type = CardNone;
    for(int i = 0; i < 5; i++)
    {
        if(Global->hasDecHardware((CardType)i,"h264"))
        {
            type = (CardType)i;
            break;
        }
    }
    QString codec = LibData->getCurMediaCodec().toLower();
    bool useHardDecode = (codec == "h264") && LibData->isHDCurVideo() && (type != CardNone);     //是否使用硬件解码---------根据硬解选项，视频格式，视频大小得出
    if(useHardDecode)
        QThreadPool::globalInstance()->setMaxThreadCount(1);
    else
        QThreadPool::globalInstance()->setMaxThreadCount(maxThreadCount);
    switch (type) {
    case CardDXVA:
        Task::HWDeviceName = "dxva2";
        break;
    case CardIntel:
        Task::HWDeviceName = "qsv";
        break;
    case CardNvidia:
        Task::HWDeviceName = "cuda";
        break;
    case CardAMD:
        Task::HWDeviceName = "dxva2";
    default:
        Task::HWDeviceName = "";
        break;
    }
                     //默认使用dxva2进行硬件解码，后期需要根据用户的选择以及机器配置进行更改
    av_register_all();
//    Task::initFormatList(_filePath);
    init();
    connect(this,SIGNAL(scrollReachedEnd()),this,SLOT(onScrollReachedEnd()));
    connect(this,SIGNAL(scrollReachedStart()),this,SLOT(onScrollReachedStart()));
    //显示模式发生改变
    connect(this,&EditCutWidget::showModeChanged,[&]{
        _preview->setItemShowMode(_showMode);
        _browser->setItemShowMode(_showMode);
    });
}

EditCutWidget::~EditCutWidget()
{
    delete _threadManager;
    _threadManager = NULL;
    Task::cleanFormatList();
}

void EditCutWidget::updateLanguage()
{

}

void EditCutWidget::updateUI(const qreal &scaleRatio)
{
    _pictureSize = QSize(_pictureSize.width() * scaleRatio / _scaleRatio,_pictureSize.height() * scaleRatio / _scaleRatio);
    _itemSize.setWidth(_itemSize.width() * scaleRatio / _scaleRatio);
    _itemSize.setHeight(_pictureSize.height());
//    _itemFixedWidth = _itemFixedWidth * scaleRatio / _scaleRatio;
    _itemFixedWidth = _itemSize.width() / 3;
    int lastScrollValue = _scrollContent->horizontalScrollBar()->value();
    int lastBrowserWidth = _browser->width();
    _preview->updateUI(scaleRatio);
    _preview->setLimitParams(_maxFrameCount,_minFrameCount,_itemSize,_itemFixedWidth);
    _browser->setItemSize(_itemSize);
    _browser->setItemFixedWidth(_itemFixedWidth);
    _browser->setFixedWidth(_preview->getRullerWidth());
    _browser->updateUI(scaleRatio);
    _viewport->updateUI(scaleRatio);

    int newValue = (qreal)lastScrollValue / lastBrowserWidth * _browser->width();
    _scrollContent->horizontalScrollBar()->setValue(newValue);
    _scaleRatio = scaleRatio;
    upUI();
}

void EditCutWidget::resizeEvent(QResizeEvent *event)
{
    upUI();
}

void EditCutWidget::wheelEvent(QWheelEvent *event)
{

}

bool EditCutWidget::event(QEvent *event)
{
    return QWidget::event(event);
}

void EditCutWidget::keyPressEvent(QKeyEvent *event)
{
    //显示模式转换，测试代码
//    switch (event->key()) {
//    case Qt::Key_1:
//        setShowMode(StackedMode);
//        break;
//    case Qt::Key_2:
//        setShowMode(TiledMode);
//        break;
//    default:
//        break;
//    }
}

void EditCutWidget::onScrollReachedEnd()
{
    //scrollarea 拖动到最后，需要解码后面的数据
}

void EditCutWidget::onScrollReachedStart()
{
    //scrollarea 拖动到最前，需要解码前面的数据
}

void EditCutWidget::onGetFrames(int index, FRAME frame)
{
    _mutex.lock();
    _browser->setItemImage(index,frame);
    _mutex.unlock();
}

void EditCutWidget::onScrollAreaValueChanged(int value)
{
    _preview->setRullerPos(value);
    _preview->setLeftPos(_preview->getLeftMidPos());
    _preview->setRightPos(_preview->getRightMidPos());
    _preview->setLeftPos(_preview->getLeftMidPos());
    _viewport->drawLeftMask(_preview->getLeftMidPos());
    _viewport->drawRightMask(_preview->getRightMidPos());

    _scrollBar->setValue(value);
}

void EditCutWidget::onGetMediaInfo(MediaEditInfo info)
{
    qDebug() << info.totalTime << info.height << info.width << info.fps << info.rotate << info.isTurn;
    _mediaInfo = info;
    _maxFrameCount = _mediaInfo.totalTime / 1000 * 2;  //最大显示的图片数量为一秒两张
    //根据高度以及视频的原始宽高比来计算图片的宽度
    if(info.isTurn)
        _pictureSize.setWidth(_pictureSize.height() * info.height / info.width);  //竖视频
    else
        _pictureSize.setWidth(_pictureSize.height() * info.width / info.height);  //横视频
    //图片宽度大小限制
    _pictureSize.setWidth(_pictureSize.width() >= MAX_FRAME_WIDTH ? MAX_FRAME_WIDTH : _pictureSize.width());
    _pictureSize.setWidth(_pictureSize.width() <= MIN_FRAME_WIDTH ? MIN_FRAME_WIDTH : _pictureSize.width());
    Task::setPictureSize(_pictureSize);

    _itemSize.setWidth(_pictureSize.width() - _pictureSize.width() % 12 + 12);
    _itemFixedWidth = _itemSize.width() / 3;
    _preview->setTotalTime(_mediaInfo.totalTime);
    _preview->setRullerWidth(PMW->width() -  280 * _scaleRatio);
    _browser->setItemSize(_itemSize);
    _browser->setItemFixedWidth(_itemFixedWidth);
    if(_showMode == 0)
        _minFrameCount = this->width() / (int)(_itemSize.width() * pow(0.618,4));        //初始化时设置最少的图片张数，刚好铺满整个窗口
    else
        _minFrameCount = this->width() / _itemFixedWidth;
    _preview->setLimitParams(_maxFrameCount,_minFrameCount,_itemSize,_itemFixedWidth);
}

void EditCutWidget::init()
{
    _scrollContent = new QScrollArea(this);
    _scrollContent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scrollContent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scrollContent->setFrameShape(QFrame::NoFrame);
    _scrollContent->setStyleSheet("background:black;");
    scroller = QScroller::scroller(_scrollContent);

    scroller->grabGesture(_scrollContent,QScroller::LeftMouseButtonGesture);


    _preview = new PreviewWidget(this);

    _browser = new PictureBrowserWidget(this);

    _viewport = new ViewportWidget(this);

    _scrollBar = new QSlider(Qt::Horizontal,this);

    _threadManager = new DecodeThreadManager();

    _scrollContent->setWidget(_browser);


    connect(_scrollBar,&QSlider::sliderMoved,[&]{
        if(_scrollBar->isSliderDown())
            _browser->setItemRepaintable(false);
    });
    connect(_scrollBar,&QSlider::sliderReleased,[&]{
        _browser->setItemRepaintable(true);
    });
    connect(scroller,&QScroller::stateChanged,[&](QScroller::State state){
//        if(state == QScroller::Scrolling)
//            _browser->setItemRepaintable(false);
//        else if(state == QScroller::Inactive)
//            _browser->setItemRepaintable(true);
//        else if(state == QScroller::Dragging)
//            _browser->foldAllItems();
        if(state != QScroller::Inactive)
            _browser->setItemRepaintable(false);
        else if(state == QScroller::Pressed)
            return;
        else
            _browser->setItemRepaintable(true);
    });

    GetMediaInfoThread *getInfo = new GetMediaInfoThread(_filePath);
    //获取到媒体信息
    connect(getInfo,&GetMediaInfoThread::getMediaInfo,this,&EditCutWidget::onGetMediaInfo,Qt::DirectConnection);
    QThreadPool::globalInstance()->start(getInfo);
    QThreadPool::globalInstance()->waitForDone();

    _browser->setItemShowMode(_showMode);
    _preview->setItemShowMode(_showMode);

    _threadManager->setItemListSize(_currentPicNum);
    _initItemTimer = new QTimer(this);
    _initItemTimer->setSingleShot(true);
    _initItemTimer->setInterval(500);
    //第一次初始化容器
    connect(_initItemTimer,&QTimer::timeout,[&]{
        if(_showMode == 0)
            _currentPicNum = (double)this->width() / (int)(_itemSize.width() * pow(0.618,4));
        else
            _currentPicNum = qRound(((double)this->width()) / _itemFixedWidth);
        QList<PictureItemWidget*> itemList;
        int64_t durationTime = _mediaInfo.totalTime / _currentPicNum;
        int64_t startTime = 0;
        for(int i = 0; i < _currentPicNum; i++)
        {
            FRAME frame;
            frame.time_pts = startTime + i * durationTime;
            frame.frameDration = durationTime;
            PictureItemWidget *item = new PictureItemWidget(i,this);
            item->setInitData(frame);
            itemList << item;
            if(_showMode == 0)
                item->resize(_itemSize);
            else
                item->resize(_itemFixedWidth,_itemSize.height());
        }
        _browser->setFixedWidth(_preview->getRullerWidth());
        _browser->setItemList(itemList);
        _browser->setItemRepaintable(true);
        _scrollBar->setStyleSheet(QString("QSlider::horizontal{background:black;}"\
                              "QSlider::groove:horizontal{background:black;height:%1px;}"\
                              "QSlider::handle:horizontal{background:gray;height:%2px;width:%3px;}")
                                  .arg(QString::number(20 * _scaleRatio)).arg(QString::number(20 * _scaleRatio))
                                  .arg(QString::number(_scrollBar->width() - _scrollBar->maximum() <= 40 ? 40 * _scaleRatio : _scrollBar->width() - _scrollBar->maximum())));
    });
    _initItemTimer->start();

    connect(_scrollContent->horizontalScrollBar(),&QScrollBar::rangeChanged,[&](int min,int max){
        _scrollBar->setRange(min,max);

        _scrollBar->setStyleSheet(QString("QSlider::horizontal{background:black;}"\
                              "QSlider::groove:horizontal{background:black;height:%1px;}"\
                              "QSlider::handle:horizontal{background:gray;height:%2px;width:%3px;}")
                                  .arg(QString::number(20 * _scaleRatio)).arg(QString::number(20 * _scaleRatio))
                                  .arg(QString::number(_scrollBar->width() - _scrollBar->maximum() <= 40 ? 40 * _scaleRatio : _scrollBar->width() - _scrollBar->maximum())));
    });
    //不可见滚动条值发生变化
    connect(_scrollContent->horizontalScrollBar(),&QScrollBar::valueChanged,this,&EditCutWidget::onScrollAreaValueChanged);
    //下方滚动条发生移动
    connect(_scrollBar,&QSlider::sliderMoved,_scrollContent->horizontalScrollBar(),&QScrollBar::setSliderPosition);
    //当前展开Item的时间
    connect(_browser,&PictureBrowserWidget::currentItemPts,[&](int pts,QPoint currentPos){
        _preview->drawCurrentPos(pts,currentPos);
    });

    connect(_browser,&PictureBrowserWidget::currentItemPosAndPts,_preview,&PreviewWidget::setcurrentItemPosAndPts);
    //快捷键,左键双击设置截取起始时间
    connect(_browser,&PictureBrowserWidget::itemLeftButtonDoubleClick,[&](int64_t pts,QPoint point){
        if(_rightMidPts == -1)
            _leftMidPts = _preview->getPtsFromPos(point);
        else
        {
            if(_preview->getPtsFromPos(point) > _rightMidPts)
                return;
            else
                _leftMidPts = _preview->getPtsFromPos(point);
        }
        emit sendSelPts(_leftMidPts,_rightMidPts);
        _preview->setLeftMidPts(_preview->getPtsFromPos(point));
        _preview->setLeftPreviewTime(_preview->getPtsFromPos(point));
        GetOneFrameThread *task = new GetOneFrameThread(_preview->getPtsFromPos(point),_filePath);
        connect(task,&GetOneFrameThread::decodeOneFrame,_preview,&PreviewWidget::setLeftPreviewImage);
        _threadManager->push(task);

        _viewport->drawLeftMask(point);
    });
    //快捷键,右键双击设置截取结束时间
    connect(_browser,&PictureBrowserWidget::itemRightButtonDoubleClick,[&](int64_t pts,QPoint point){
        if(_leftMidPts == -1)
            _rightMidPts = _preview->getPtsFromPos(point);
        else
        {
            if(_preview->getPtsFromPos(point) < _leftMidPts)
                return;
            else
                _rightMidPts = _preview->getPtsFromPos(point);
        }
        emit sendSelPts(_leftMidPts,_rightMidPts);
        _preview->setRightMidPts(_preview->getPtsFromPos(point));
        _preview->setRightPreviewTime(_preview->getPtsFromPos(point));
        GetOneFrameThread *task = new GetOneFrameThread(_preview->getPtsFromPos(point),_filePath);
        connect(task,&GetOneFrameThread::decodeOneFrame,_preview,&PreviewWidget::setRightPreviewImage);
        _threadManager->push(task);

        _viewport->drawRightMask(point);
    });
    //Item请求图像数据
    connect(_browser,&PictureBrowserWidget::itemRequestData,[&](int64_t pts,int index){
        RequestOneFrameThread *task = new RequestOneFrameThread(pts,index,_filePath);
        connect(task,&RequestOneFrameThread::decodeFailed,[&](int index,FRAME frame){
            qDebug() << "--------------------------------" << "decode failed" << index;
//            _browser->decodeFailed(index,frame);
        });
        connect(task,&RequestOneFrameThread::decodeOneFrame,[&](int index,FRAME frame){
            _mutex.lock();
            _browser->setItemImage(index,frame);
            _mutex.unlock();
            //效率测试代码
//            testList.remove(index);
//            if(testList.isEmpty())
//            {
//                qDebug() << "decode frames coast" << testTime.elapsed() << "ms";
//            }
        });
       _threadManager->push(task);
//       if(testList.isEmpty())
//       {
//           testTime.restart();
//       }
//       testList.insert(index);

    });
    //Item数量发生变化
    connect(_browser,SIGNAL(itemListSizeChanged(int)),_threadManager,SLOT(setItemListSize(int)));

    //上方卡尺比例发生变化,根据卡尺的长度来计算需要加载的Item数量
    connect(_preview,&PreviewWidget::requestScaleChange,[&](int width){
        emit timeLineIsChangeScale();
        _browser->setFixedWidth(_preview->getRullerWidth());
        _scrollContent->horizontalScrollBar()->setValue(qAbs(_preview->getRullerPos().x()));
        int picNum = 0;
        if(_showMode == StackedMode)
            picNum = width / (int)(_itemSize.width() * pow(0.618,4));
        else
            picNum = qRound(((qreal)width) / _itemFixedWidth);
//        if(picNum > _maxFrameCount/* || picNum < _minFrameCount*/)
//        {
//            return;
//        }
        if(picNum > _currentPicNum)
        {
            _mutex.lock();
            _browser->addTail(picNum - _currentPicNum);
            _mutex.unlock();
            _currentPicNum = picNum;
        }
        else if(picNum < _currentPicNum)
        {
            _mutex.lock();
            _browser->removeTail(_currentPicNum - picNum);
            _mutex.unlock();
            _currentPicNum = picNum;
        }
        else
            return;
        _preview->setLeftPos(_preview->getLeftMidPos());
        _preview->setRightPos(_preview->getRightMidPos());
        _preview->setLeftPos(_preview->getLeftMidPos());
        _browser->setItemRepaintable(false);
        //测试代码
//        testList.clear();

    });
    //卡尺比例变化结束,重新赋值每个Item的时间
    connect(_preview,&PreviewWidget::requestScaleChangeData,[&]{
        if(_currentPicNum == 0 || _currentPicNum > _maxFrameCount || _currentPicNum < _minFrameCount)
            return;
        qreal durationTime = (qreal)_mediaInfo.totalTime / _currentPicNum;
        int64_t startTime = 0;
        for(int i = 0; i < _currentPicNum; i++)
        {
            FRAME frame;
            frame.time_pts = startTime + i * durationTime;
            frame.frameDration = durationTime;
            _browser->setItemInitData(i,frame);
        }
        _browser->setFocusItemWithPosition(_preview->getCurrentPts(),QPoint(0,0));
        _preview->setRullerWidth(_currentPicNum * _itemFixedWidth);
        _browser->setFixedWidth(_currentPicNum * _itemFixedWidth);
        _browser->setItemRepaintable(true);
    });
    //左边截取按钮移动
    connect(_preview,&PreviewWidget::leftButtonmoved,[&](QPoint pos){
        _browser->setFocusItemWithPosition(_preview->getPtsFromPos(pos),pos);
        int64_t pts = _preview->getPtsFromPos(pos);
        if(pts > _mediaInfo.totalTime)
            return;
        _preview->setLeftPreviewTime(pts);
        GetOneFrameThread *task = new GetOneFrameThread(pts,_filePath);
        connect(task,&GetOneFrameThread::decodeOneFrame,_preview,&PreviewWidget::setLeftPreviewImage);
        connect(task,&GetOneFrameThread::decodeFailed,[&]{
            _preview->setLeftPreviewNodata();
        });
        _threadManager->push(task);
        _leftMidPts = pts;
        if(_rightMidPts != -1)
            emit sendSelPts(_leftMidPts,_rightMidPts);
    });
    //右边截取按钮移动
    connect(_preview,&PreviewWidget::rightButtonMoved,[&](QPoint pos){
        _browser->setFocusItemWithPosition(_preview->getPtsFromPos(pos),pos);
        int64_t pts = _preview->getPtsFromPos(pos);
        if(pts > _mediaInfo.totalTime)
            return;
        _preview->setRightPreviewTime(pts);
        GetOneFrameThread *task = new GetOneFrameThread(pts,_filePath);
        connect(task,&GetOneFrameThread::decodeOneFrame,_preview,&PreviewWidget::setRightPreviewImage);
        connect(task,&GetOneFrameThread::decodeFailed,[&]{
            _preview->setRightPreviewNodata();
        });
        _threadManager->push(task);
        _rightMidPts = pts;
        if(_leftMidPts != -1)
            emit sendSelPts(_leftMidPts,_rightMidPts);
    });
    //截取按钮释放
    connect(_preview,&PreviewWidget::buttonDragrelease,[&]{
    });
    //移动视口到左边按钮对应的位置
    connect(_preview,&PreviewWidget::requestMoveToLeft,[&]{
        _scrollContent->horizontalScrollBar()->setValue(_preview->getLeftMidPos(true).x());
    });
    //移动视口到右边按钮对应的位置
    connect(_preview,&PreviewWidget::requestMoveToRight,[&]{
        _scrollContent->horizontalScrollBar()->setValue(_preview->getRightMidPos(true).x() - _scrollContent->width() <= 0 ? 0 : _preview->getRightMidPos(true).x() - _scrollContent->width());
    });
    //左边截取位置发生变化
    connect(_preview,&PreviewWidget::leftMidPositionChanged,[&](QPoint pos){
        _viewport->drawLeftMask(pos);
    });
    //右边截取位置发生变化
    connect(_preview,&PreviewWidget::rightMidPositionChanged,[&](QPoint pos){
        _viewport->drawRightMask(pos);
    });
    _scrollBar->setMinimum(0);
    _scrollBar->setMaximum(0);

    upUI();
    //初始化上方截取按钮和Preview的数据
    _initPreviewTimer = new QTimer(this);
    _initPreviewTimer->setSingleShot(true);
    _initPreviewTimer->setInterval(500);
    connect(_initPreviewTimer,&QTimer::timeout,[&]{
        _preview->setLeftMidPts(_curTime);
        _preview->setLeftPreviewTime(_curTime);
        _preview->setRightMidPts(_mediaInfo.totalTime);
        _preview->setRightPreviewTime(_mediaInfo.totalTime);
        _viewport->drawLeftMask(_preview->getLeftMidPos());
        _viewport->drawRightMask(_preview->getRightMidPos());

        GetOneFrameThread *task1 = new GetOneFrameThread(_curTime,_filePath);
        connect(task1,&GetOneFrameThread::decodeOneFrame,_preview,&PreviewWidget::setLeftPreviewImage);
        connect(task1,&GetOneFrameThread::decodeFailed,[&]{
            _preview->setLeftPreviewNodata();
        });
        _threadManager->push(task1,false);

        GetOneFrameThread *task2 = new GetOneFrameThread(_mediaInfo.totalTime,_filePath);
        connect(task2,&GetOneFrameThread::decodeOneFrame,_preview,&PreviewWidget::setRightPreviewImage);
        connect(task2,&GetOneFrameThread::decodeFailed,[&]{
            _preview->setRightPreviewNodata();
        });
        _threadManager->push(task2,false);
        _leftMidPts = _curTime;
        _rightMidPts = _mediaInfo.totalTime;
        emit sendSelPts(_leftMidPts,_rightMidPts);
    });
    _initPreviewTimer->start();

}

void EditCutWidget::upUI()
{
    _preview->setGeometry(0,0,this->geometry().width(),20 * _scaleRatio);
    _scrollContent->setGeometry(0,_preview->geometry().bottom() + 1,this->geometry().width(),67* _scaleRatio);
    _viewport->setGeometry(_scrollContent->geometry());
    _scrollBar->setGeometry(0,_scrollContent->geometry().bottom() + 1,this->geometry().width(),20* _scaleRatio);

    if(_preview->width() > _preview->getRullerWidth() && _currentPicNum < _maxFrameCount)
    {
        _preview->setRullerWidth(_preview->width());
        _preview->requestScaleChange(_preview->getRullerWidth());
        _preview->requestScaleChangeData();
    }

    _browser->setFixedHeight(_scrollContent->height() - 1);
    _preview->setPreviewLabelPos();
    _preview->setPreviewLabelSize(_pictureSize);

    _preview->setRightPos(_preview->getRightMidPos());
    _preview->setLeftPos(_preview->getLeftMidPos());
    _viewport->drawLeftMask(_preview->getLeftMidPos());
    _viewport->drawRightMask(_preview->getRightMidPos());
    _scrollBar->setStyleSheet(QString("QSlider::horizontal{background:black;}"\
                          "QSlider::groove:horizontal{background:black;height:%1px;}"\
                          "QSlider::handle:horizontal{background:gray;height:%2px;width:%3px;}")
                              .arg(QString::number(20 * _scaleRatio)).arg(QString::number(20 * _scaleRatio))
                              .arg(QString::number(_scrollBar->width() - _scrollBar->maximum() <= 40 ? 40 * _scaleRatio : _scrollBar->width() - _scrollBar->maximum())));
}

void EditCutWidget::dragSizeChange(bool b)
{

    if(b)
    {
        if(_preview->width() > _preview->getRullerWidth() && _currentPicNum < _maxFrameCount)
        {
            _preview->setRullerWidth(_preview->width());
            _preview->requestScaleChange(_preview->getRullerWidth());
            _isScaleChanged = true;
        }
    }
    else
    {
        if(_isScaleChanged)
        {
            _preview->requestScaleChangeData();
            _isScaleChanged = false;
        }
    }
}

EditCutWidget::ItemShowMode EditCutWidget::showMode()
{
    return _showMode;
}

void EditCutWidget::setShowMode(EditCutWidget::ItemShowMode mode)
{
    if(_showMode == mode)
        return;
    _showMode = mode;
    emit showModeChanged();
}

