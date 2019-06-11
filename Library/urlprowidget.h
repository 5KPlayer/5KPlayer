#ifndef URLPROWIDGET_H
#define URLPROWIDGET_H

#include "transeventwidget.h"
#include "updateinterface.h"
#include <QLabel>
#include <QMovie>

/*!
 * \brief The UrlproWidget class
 * 一个旋转中的进度显示界面
 */
class UrlproWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit UrlproWidget(QWidget *parent = 0);

    void updateLanguage() {}
    void updateUI(const qreal &scaleRatio);

signals:

public slots:
private:
    void Init();
    void upUI();
private:
    QLabel *_proGIF;
    QMovie *_proMovie;
    double  _scaleRatio;
};

#endif // URLPROWIDGET_H
