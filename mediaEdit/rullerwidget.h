#ifndef RULLERWIDGET_H
#define RULLERWIDGET_H

#include "updateinterface.h"
#define TIMEPERPIXEL 10

class RullerWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(qreal timePerPixel READ timePerPixel WRITE setTimePerPixel NOTIFY timePerPixelChanged)
    Q_PROPERTY(int64_t totalTime READ totalTime WRITE setTotalTime NOTIFY totalTimeChanged)
    Q_PROPERTY(qreal windowWidth READ windowWidth WRITE setWindowWidth NOTIFY windowWidthChanged)
public:
    explicit RullerWidget(QWidget *parent = 0);
    qreal scaleFactor() const;
    qreal timePerPixel() const;
    int64_t totalTime() const;
    qreal windowWidth() const;

    void setScaleFactor(qreal factor);
    void setTimePerPixel(qreal time);
    void setTotalTime(int64_t time);
    void setWindowWidth(qreal width);

    int64_t getPtsFromPos(QPoint pos);

    void drawCurrentLine(int64_t pts, QPoint currentPos);
    int64_t getCurrentPts();

    QPoint getLeftMidPos(bool source = false);
    QPoint getRightMidPos(bool source  = false);

    QPoint getRightLimitPos();

    void setLeftMidPos(QPoint pos,bool globalpos = true);
    void setRightMidPos(QPoint pos,bool globalpos = true);
    void setLeftMidPos(int64_t pts);
    void setRightMidPos(int64_t pts);
    void addScale();
    void reduceScale();
    void setScale(int value,QPoint p);
    void setMaxWindowWidth(int width);
    int getMaxWindowWidth();
    void updateUI(const qreal &scaleRatio);
    void updateLanguage(){}
    void setcurrentItemPosAndPts(int64_t pts, int64_t duration, QPoint left, QPoint right);

protected:
    void paintEvent(QPaintEvent *event);
signals:
    void scaleFactorChanged();
    void timePerPixelChanged();
    void totalTimeChanged();
    void windowWidthChanged();

    void leftMidPosChanged();
    void rightMidPosChanged();

public slots:

private:
    double _scaleFactor = 1.0;
    int _scale = 0;
    double _timePerPixel;
    int64_t _totalTime;
    qreal _width;
    int _currentPts;
    QPoint _leftMidPos;
    QPoint _rightMidPos;
    int64_t _leftMidPts;
    int64_t _rightMidPts;
    bool _leftMidPosInited = false;
    bool _rightMidPosInited = false;
    qreal _lineWidth;
    int _roundTime;
    qreal _correctTime;                 //时间修正
    int _maxWindowWidth;
    qreal _screenRatio;
    QPoint _currentPos;
    QPoint _currentItemLeft;
    QPoint _currentItemRight;
    int _currentItemPts = 0;
    int _currentItemDuration = 0;
};

#endif // RULLERWIDGET_H
