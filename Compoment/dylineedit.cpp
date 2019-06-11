#include "dylineedit.h"

dyLineEdit::dyLineEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

void dyLineEdit::focusOutEvent(QFocusEvent *ev)
{
    Q_UNUSED(ev)
    emit foucsOut();
}
