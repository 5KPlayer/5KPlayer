#ifndef PREVIEW_H
#define PREVIEW_H

#include <QDialog>

class QLabel;

class Preview : public QDialog
{
    Q_OBJECT
public:
    explicit Preview(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    bool showImage(const QByteArray &, const int &, const int &, const int &);

private:
    QLabel *p_image;
    QLabel *p_timeLabel;
};

#endif // PREVIEW_H
