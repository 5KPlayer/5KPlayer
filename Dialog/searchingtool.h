#ifndef SEARCHINGTOOL_H
#define SEARCHINGTOOL_H

#include <QWidget>
#include "updateinterface.h"
class QListWidgetItem;
class QListWidget;
class SearchingTool : public QWidget,public UpdateInterface
{
    Q_OBJECT
public:
    explicit SearchingTool(QWidget *parent = 0);

    void updateLanguage(){;}
    void updateUI(const qreal &scaleRatio);

    void showUp(QWidget* focusWidget, QStringList strs);
    void setWidget(QWidget* focusWidget, QStringList strs);

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void onItemClicked(QListWidgetItem* item);

signals:
    void doClicked(const QString& str);

private:
    QListWidget* m_lstItems;
    int m_maxNum;
    QWidget* m_focusWidget;
    QStringList m_strs;
    double _scaleRatio;
};

#endif // SEARCHINGTOOL_H
