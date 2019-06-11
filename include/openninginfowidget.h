#ifndef OPENNINGINFOWIDGET_H
#define OPENNINGINFOWIDGET_H

#include <QWidget>
/*************
 * 在播放在线视频时，openning状态无法继续操作。目前由界面阻塞操作。
 *************/

class OpenningInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpenningInfoWidget(QWidget *parent = Q_NULLPTR);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *);
};

#endif // OPENNINGINFOWIDGET_H
