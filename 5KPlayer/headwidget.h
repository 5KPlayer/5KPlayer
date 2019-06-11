#ifndef HEADWIDGET_H
#define HEADWIDGET_H

#include "transeventwidget.h"
#include "updateinterface.h"

class SvgButton;
class TextButton;

class HeadWidget : public TransEventWidget, public UpdateInterface
{
    Q_OBJECT
public:
    explicit HeadWidget(QWidget *parent = 0);
    ~HeadWidget(){}

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    bool isShowPlayerWidget() const;
    bool isShowLibraryWidget() const;

    void enableRotate(const bool &enable);

    void changeAirplayState(const bool &b);

public slots:
    void clickPlayerW();
    void clickLibraryW();
    void showControl(bool isShow);

    void clickLeftitem(QString item);

private slots:
    void showMenus();
    void showAudioMenus();
    void showSubtitleMenus();
    void showChapterMenus();
    void showAirPlayList();

    void leftRotate();
    void rightRotate();

    void updateShowMenusState();
    void InitLanguage();

private:
    void initChangeControl();
    void initVideoControl();
    void initWidgetControl();

protected:
    virtual void resizeEvent(QResizeEvent *);

signals:
    void showPlayerW();  //按下了Player 按钮
    void showLibraryW(); //按下了Library按钮

private:
    TextButton *_playerBtn;
    TextButton *_libraryBtn;

    SvgButton *_leftRotateBtn;
    SvgButton *_rightRotateBtn;
    SvgButton *_chapterBtn;
    SvgButton *_subtitleBtn;
    SvgButton *_audioTrackBtn;
    //SvgButton *_airBtn;
    SvgButton *_minimalBtn;
    SvgButton *_zoomBtn;
    SvgButton *_closeBtn;
    SvgButton *_setBtn;

    bool _closeClicked;
};

#endif // HEADWIDGET_H
