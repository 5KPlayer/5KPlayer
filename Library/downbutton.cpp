#include "downbutton.h"
#include <QPainter>
#include <QMouseEvent>
DownButton::DownButton(QWidget *parent)
    : QLabel(parent)
{
    Init();
    this->setStyleSheet("background:transparent");
}

void DownButton::setAngle(double angle)
{
    _angle = angle*3.6;
    repaint();
}

void DownButton::Init()
{
    _angle = 0;
    _color = QColor(94, 94, 98);
}

void DownButton::gradientArc(QPainter *painter, int radius, int startAngle, int angleLength)
{
    QRadialGradient gradient(0, 0, radius);
    gradient.setColorAt(0, Qt::white);
    painter->setBrush(gradient);

    // << 1（左移1位）相当于radius*2 即：150*2=300
    //QRectF(-150, -150, 300, 300)
    QRectF rect(-radius, -radius, radius << 1, radius << 1);
    QPainterPath path;
    path.arcTo(rect, startAngle, -angleLength);

    painter->setPen(Qt::NoPen);

    painter->drawPath(path);
}

void DownButton::mousePressEvent(QMouseEvent *ev)
{
    ev->accept();
}

void DownButton::mouseMoveEvent(QMouseEvent *ev)
{
    ev->accept();
}

void DownButton::mouseReleaseEvent(QMouseEvent *ev)
{
    ev->accept();
    if(this->rect().contains(ev->pos()))
        emit clicked();
}

void DownButton::enterEvent(QEvent *)
{
    _color = QColor(70, 70, 72);
    this->repaint();
}

void DownButton::leaveEvent(QEvent *)
{
    _color = QColor(94, 94, 98);
    this->repaint();
}

void DownButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int radius = this->width()/2-2;

    // >> 1（右移1位）相当于width() / 2
    painter.translate(width() >> 1, height() >> 1);

    // 外边框
    QLinearGradient lg1(0, -radius, 0, radius);
    lg1.setColorAt(1.0, _color);
    painter.setBrush(lg1);
    painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);

    /**
     * 参数二：半径
     * 参数三：开始的角度
     * 参数四：指扫取的角度-顺时针（360度 / 8 = 45度）
    **/
    gradientArc(&painter, radius, 90,  _angle);
}
