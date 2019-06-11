#ifndef TRANSEVENTICO_H
#define TRANSEVENTICO_H

#include <QSvgWidget>
/*!
 * \brief The TransEventIco class
 * 显示svg图标，能透过鼠标事件
 */
class TransEventIco : public QSvgWidget
{
    Q_OBJECT
public:
    explicit TransEventIco(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

};

#endif // TRANSEVENTICO_H
