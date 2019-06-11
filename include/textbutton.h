#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include <QLabel>
#include "updateinterface.h"

class TextButton : public QLabel, public UpdateInterface
{
    Q_OBJECT
public:
    enum Position{Left,Center,Right}; //位置
    explicit TextButton(const QString &text, const int &w, const int &h,
                        QWidget *parent = Q_NULLPTR,
                        Qt::WindowFlags f = Qt::WindowFlags());
    ~TextButton() {}

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

    void setPosition(const Position &);

    bool pressed() const
    {
        return _isPressed;
    }

public slots:
    void click();  //主动点击
    void recover();//恢复

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

signals:
    void clicked();

private:
    int _radius;
    bool _isPressed;

    Position _curPosition;

    QString _pressedStyleStr;
    QString _releaseStyleStr;

    QString _leftStyleStr;
    QString _rightStyleStr;
};

#endif // TEXTBUTTON_H
