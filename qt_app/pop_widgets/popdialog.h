#ifndef POPDIALOG_H
#define POPDIALOG_H

#include <QDialog>

namespace Ui {
class popDialog;
}

class popDialog : public QDialog
{
    Q_OBJECT

public:
    explicit popDialog(QString text,QWidget *parent = nullptr);
    ~popDialog();

private:
    Ui::popDialog *ui;
};

#endif // POPDIALOG_H
