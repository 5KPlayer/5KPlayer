#ifndef URLFORMATWIDGET_H
#define URLFORMATWIDGET_H

#include "transeventwidget.h"
#include "transeventlabel.h"
#include "updateinterface.h"
#include "dystructu.h"
#include "dycheckbox.h"

/*!
 * \brief The URLFormatWidget class
 * Youtube 详细信息中的所有能下载的格式列表中的子项类
 */
class URLFormatWidget : public TransEventWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit URLFormatWidget(QWidget *parent = 0);

    void updateLanguage();
    void updateUI(const qreal &scaleRatio);

    void setDataSouce(const STREAMINFO &formatinfo);
    STREAMINFO getDataSouce();

    void setCheckBox(bool check);
signals:
    void upFormat(STREAMINFO);
public slots:
    void checkBoxClick(bool check);
private:
    void Init();
    void UpUI();
protected:
    void resizeEvent(QResizeEvent *);
private:
    DYCheckBox      *_checkBox;
    TransEventLabel *_quality;
    TransEventLabel *_qualityInfo;
    TransEventLabel *_HDflag;
    TransEventLabel *_format;
    TransEventLabel *_formatInfo;
    TransEventLabel *_fileSize;
    TransEventLabel *_SizeInfo;
    STREAMINFO       _formatdata;

    double           _scaleRatio;
};

#endif // URLFORMATWIDGET_H
