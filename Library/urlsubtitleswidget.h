#ifndef URLSUBTITLESWIDGET_H
#define URLSUBTITLESWIDGET_H

#include <QCheckBox>
#include "dylabel.h"

#include "transeventwidget.h"
#include "updateinterface.h"
#include "dystructu.h"
#include "dycheckbox.h"
/*!
 * \brief The URLSubtitlesWidget class
 * Youtube 的详细信息中的字幕界面，其他包含当前媒体的所有字幕信息
 */
class URLSubtitlesWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit URLSubtitlesWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setSubTitle(const QStringList &subData, const QStringList &checkList);
    QStringList getSubTitle();

    QStringList getCheckSubTitle();
    void clearSubTitle();
signals:

public slots:
    void checkBoxClick(bool check);
    void leftBtnClick();
    void rightBtnClick();
private:
    void Init();
    void upUI();

    void upSubWidget();
    int  getCheckWidth(const QString &checkText);
    void hideAllCheckBox();
    void addtoSubtitle(int pos);
    void removetoSubtitle(int pos);
    QString languagetoString(const QString &language);
protected:
    void resizeEvent(QResizeEvent *);
private:
    int             _devValue;
    int             _showCount;
    int             _curShowCount;
    double          _scaleRatio;
    QStringList     _checkSubtitle;
    DYCheckBox     *_checkBox[10];
    QStringList     _subList;
    DYLabel        *_leftBtn;
    DYLabel        *_rightBtn;
};

#endif // URLSUBTITLESWIDGET_H
