#ifndef CPAUSEWIDGET_H
#define CPAUSEWIDGET_H

#include <QPixmap>
#include "updateinterface.h"

class SvgButton;
class QTimer;

class CPauseWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit CPauseWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private slots:
    void timerOut();

protected:
    void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

private:
    QPixmap _pixmap;
    QPixmap _pixmap2;
    QTimer *_timer;

    SvgButton *_playBtn;

    int _angle;
};

#endif // CPAUSEWIDGET_H
