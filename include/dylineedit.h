#ifndef DYLINEEDIT_H
#define DYLINEEDIT_H

#include <QObject>
#include <QLineEdit>
class dyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    dyLineEdit(QWidget * parent = 0);
signals:
    void foucsOut();
protected:
    void focusOutEvent(QFocusEvent *);
};

#endif // DYLINEEDIT_H
