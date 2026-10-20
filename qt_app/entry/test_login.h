#ifndef TEST_LOGIN_H
#define TEST_LOGIN_H

#include <QWidget>

namespace Ui {
class test_login;
}

class test_login : public QWidget
{
    Q_OBJECT

public:
    explicit test_login(QWidget *parent = nullptr);
    ~test_login();

private:
    Ui::test_login *ui;
};

#endif // TEST_LOGIN_H
