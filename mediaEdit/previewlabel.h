#ifndef PREVIEWLABEL_H
#define PREVIEWLABEL_H

#include <QWidget>
#include <QLabel>
#include <QImage>

class PreviewLabel : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewLabel(QWidget *parent = 0);
    ~PreviewLabel();
    void resizeEvent(QResizeEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void setLabelSize(QSize size);
    void setPixLabelNodata();
signals:
    void previewLabelDoubleClicked();

public slots:
    void setPixmap(QImage image);
    void setPts(int64_t pts);

private:
    QLabel *pixlabel;
    QLabel *timeLabel;
    int minLabelWidth = 0;
    QString ptsToString(int64_t pts);
    QImage image;
};

#endif // PREVIEWLABEL_H
