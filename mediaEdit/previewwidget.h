#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include "previewlabel.h"
#include "rullerwidget.h"
#include "dystructu.h"
#include <QSvgWidget>
#include "updateinterface.h"
#include <QPixmap>
#include <QTimer>

class PreviewWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit PreviewWidget(QWidget *parent = 0);
    ~PreviewWidget();

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);

    void setLeftPos(QPoint pos);
    void setRightPos(QPoint pos);
    void setPreviewLabelPos();
    void setLeftPreviewPos();
    void setRightPreviewPos();
    void setPreviewLabelSize(QSize size);
    void setRullerPos(int value);
    int64_t getPtsFromPos(QPoint pos);
    int64_t getLeftMidPts();
    int64_t getRightMidPts();
    QPoint getLeftMidPos(bool source = false);
    QPoint getRightMidPos(bool source = false);
    void setLeftMidPts(int64_t pts);
    void setRightMidPts(int64_t pts);
    void addRullerScale();
    void reduceRullerScale();
    QPoint getRullerPos();
    void setLimitParams(int maxFrameCount, int minFrameCount, QSize itemSize, int itemFixedWidth);
    void setItemShowMode(int mode);
    int64_t getCurrentPts();
signals:
    void leftButtonmoved(QPoint pos);
    void rightButtonMoved(QPoint pos);
    void requestMoveToLeft();
    void requestMoveToRight();
    void leftMidPositionChanged(QPoint pos);
    void rightMidPositionChanged(QPoint pos);
    void requestScaleChange(int pcs);
    void requestScaleChangeData();
    void buttonDragrelease();
public slots:
    void setLeftPreviewImage(FRAME frame);
    void setRightPreviewImage(FRAME frame);
    void moveRullerToLeftPts(int64_t pts);
    void moveRullerToRightPts(int64_t pts);
    void setTotalTime(int64_t time);
    void setRullerWidth(int width);
    int getRullerWidth();
    void setLeftPreviewTime(int64_t pts);
    void setRightPreviewTime(int64_t pts);
    void drawCurrentPos(int64_t pts, QPoint currentPos);
    void setLeftPreviewNodata();
    void setRightPreviewNodata();
    void setcurrentItemPosAndPts(int64_t pts,int64_t duration,QPoint left,QPoint right);
private:
    QSvgWidget *leftButton;
    QSvgWidget *rightButton;
    PreviewLabel *leftLabel;
    PreviewLabel *rightLabel;
    bool leftpressed = false;
    bool rightpressed = false;
    RullerWidget *timeLine;
    bool isMouseIn = false;
    QPoint currentPos;
    bool isButtonDown = false;
    bool isScaleChanged = false;
    int _minFrameCount;
    int _maxFrameCount;
    QSize _itemSize;
    qreal _screenRatio;
    QPixmap _hoverCursor;
    QPixmap _downCursor;
    QPixmap _hoverPix;
    QPixmap _downPix;
    int _itemShowMode = 0;
    int _itemFixedWidth = 0;
    qreal _correctTime = 0;
};

#endif // PREVIEWWIDGET_H
