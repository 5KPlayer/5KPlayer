#ifndef FILERELATIONWIDGET_H
#define FILERELATIONWIDGET_H

#include "updateinterface.h"

class DYLabel;
class DYCheckBox;
class FileRelation;

class FileRelationWidget : public UpdateInterfaceWidget
{
    Q_OBJECT
public:
    explicit FileRelationWidget(QWidget *parent = 0);

    void updateLanguage(){}
    void updateUI(const qreal &scaleRatio);

private slots:
    void effClicked();//立即生效按钮点击
    void okClicked();

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    void initDYLabel(DYLabel **label, const QString &str);
    void initDYCheckBox(DYCheckBox **box, const QString &str);

private:
    DYLabel *_selectAllBtn;

    //DYCheckBox *_autoBootBox;//开机启动
    DYCheckBox *_assocFileBox;//自动关联文件

    DYLabel *_immediateEffectBtn;//立即生效
    DYLabel *_okBtn;

    QList<DYCheckBox *> boxList;
    QStringList suffixList;

    qreal _ratio;

    FileRelation *_relation;

    QStringList m_regList;//已经关联的list
};

#endif // FILERELATIONWIDGET_H
