#include "test_login.h"
#include "ui_test_login.h"

test_login::test_login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::test_login)
{
    ui->setupUi(this);
}

test_login::~test_login()
{
    delete ui;
}
