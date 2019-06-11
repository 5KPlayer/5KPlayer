#ifndef SVGLABBUTTON_H
#define SVGLABBUTTON_H

/**********************
 * 主要用在Youtubeformat界面的按钮
 **********************/


#include "updateinterface.h"
class QLabel;
class QSvgWidget;

class SvgLabButton : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit SvgLabButton(QWidget *parent = 0);

    void setInfo(const QString &svgFile, const QString &text);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);
    enum SvgState {Svg_Nor,
                   Svg_Hover,
                   Svg_Down};
private:
    void upUI();
    void upSvgState(SvgState state);
protected:
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;

signals:
    void clicked();

private:
    QSvgWidget *_svgImage;
    QLabel *    _textLabel;

    bool    _mouseEnter;
    bool    _firstInfo;

    int     _svgWidth;
    int     _svgHeight;
    double  _scaleRatio;

    QString _svgNorImage;
    QString _svghoverImage;
    QString _svgdownImage;

    QString _labText;
    QString _btnStyle;
    QString _enterColor;
    QString _leaveColor;
    QString _pressColor;
};

#endif // SVGLABBUTTON_H
