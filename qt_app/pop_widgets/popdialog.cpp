#include "popdialog.h"
#include "ui_popdialog.h"

popDialog::popDialog(QString text,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::popDialog)
{
    ui->setupUi(this);
    ui->plainTextEdit->setPlainText(text);
}

popDialog::~popDialog()
{
    delete ui;
}
