#ifndef TRANSEVENTLABELBTN_H
#define TRANSEVENTLABELBTN_H

#include <QLabel>
/*!
 * \brief The TransEventLabelBtn class
 * 用来设置一个label的style的按钮
 */
class TransEventLabelBtn : public QLabel
{
    Q_OBJECT
public:
    explicit TransEventLabelBtn(QWidget *parent = 0, const QString &style = "background:transparent");

    void setnorStyle(const QString &style);
    void setEnterStyle(const QString & style);
    void setmouseHand(const bool &hand);

signals:
    void clicked(bool);

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

#endif // TRANSEVENTLABELBTN_H
