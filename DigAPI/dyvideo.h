#ifndef DYVIDEO_H
#define DYVIDEO_H
#include <QObject>
#include <QMap>
#include "dyenums.h"

class DYMediaPlayer;

class DYVideo : public QObject
{
    Q_OBJECT
public:
    explicit DYVideo(DYMediaPlayer *player);
    ~DYVideo(){setAdjustEnable(false);}

public:

    //字幕
    QMap<int, QString> subtitles() const;
    int subtitle() const;
    int subtitleCount() const;
    void setSubtitleFile(const QString &subtitle);
    int getSpuDelay();
    void setSpuDelay(const int &);

    //章节
    QMap<int, QString> chapters() const;
    int chapter() const;
    int chapterCount() const;

    //title
    QMap<int, QString> titles() const;
    int title() const;
    int titleCount() const;

    //轨道
    int track() const;
    int trackCount() const;
    QMap<int, QString> tracks() const;

    //屏幕截图
    bool takeSnapshot(const QString &path, const int &w, const int &h) const;

    //LOGO
    void hideLogo();
    void showLogo(const QString &file,
                  int x,
                  int y,
                  int opacity = 255);

    //Marquee
    void hideMarquee();
    void showMarquee(const QString &text,
                     int x,
                     int y,
                     int timeout = 0,
                     int opacity = 255,
                     int size = -1,
                     int color = 0xFFFFFF,
                     int refresh = 0);

    bool getVoutSize();

    //是否启动调整
    void setAdjustEnable(const bool &);
    void setContrast(const float &);
    void setBrightness(const float &);
    void setHue(const float &);
    void setSaturation(const float &);
    void setGamma(const float &);
    float getContrast();
    float getBrightness();
    float getHue();
    float getSaturation();
    float getGamma();

signals:
    void changeRatio();

public slots:
    /*!
     * \brief setvideoTrack
     * \param track
     * 设置视频轨道
     */
    void setvideoTrack(int track);

    /*!
     * \brief setAspeceRatio
     * \param ratio
     * 设置显示宽高比
     */
    void setAspeceRatio(const QString &ratio);
    /*!
        \brief Set new crop filter geometry.
        \param ratio new crop filter geometry or NULL to reset to default (Vlc::Ratio)
        设置窗口截取
    */
    void setCropGeometry(const QString &ratio);

    /*!
        \brief Set new scale ratio.
        \param scale scale ratio or NULL to reset to default (DYVlc::Scale)
        缩放比例
    */
    void setScale(const DYVlc::Scale &scale);
    /*!
        \brief Enable or disable deinterlace filter.
        \param filter type of deinterlace filter or NULL to reset to default (Vlc::Deinterlacing)
    */
    void setDeinterlace(const DYVlc::Deinterlacing &filter);
    void setDeinterlace(const QString &filter);

    /*!
        \brief Set current video subtitle.
        \param subtitle new video subtitle (int)
        设置字幕
    */
    void setSubtitle(int i_subtitle);

    void setChapter(int i_chapter);
    void setTitle(int i_title);

private:
    DYMediaPlayer *m_pParent;
};

#endif // DIGVIDEO_H
