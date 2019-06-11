#ifndef DOWNBUTTON_H
#define DOWNBUTTON_H

#include <QWidget>
#include <QLabel>
class DownButton : public QLabel
{
    Q_OBJECT
public:
    explicit DownButton(QWidget *parent = 0);

    void setAngle(double angle);
signals:
    void profinish();
    void clicked();

private:
    void Init();
    void gradientArc(QPainter *painter, int radius, int startAngle, int angleLength);
protected:
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseMoveEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void paintEvent(QPaintEvent *);
private:
    double     _angle;
    QColor     _color;
};

#endif // DOWNBUTTON_H
