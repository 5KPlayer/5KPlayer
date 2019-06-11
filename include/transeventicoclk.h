#ifndef TRANSEVENTICOCLK_H
#define TRANSEVENTICOCLK_H

#include <QSvgWidget>
/*!
 * \brief The TransEventIcoClk class
 * 用来显示svg图标，不能透过鼠标事件，能响应点击事件
 */
class TransEventIcoClk : public QSvgWidget
{
    Q_OBJECT
public:
    explicit TransEventIcoClk(QWidget *parent = 0,QString style = "background:transparent");

    void setnorStyle(const QString &style);
    void setEnterStyle(const QString &style);
    void setmouseHand(const bool &hand);

signals:
    void clicked();

private:
    bool _mousePress;
    bool _mouseHand;
    QString _enterStyle;
    QString _leaveStyle;

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
};

#endif // TRANSEVENTICOCLK_H
