#ifndef LOAD_FUND_GROUP_H
#define LOAD_FUND_GROUP_H

#include <QWidget>

namespace Ui {
class load_fund_group;
}

class load_fund_group : public QWidget
{
    Q_OBJECT

public:
    explicit load_fund_group(QWidget *parent = nullptr);
    ~load_fund_group();

private slots:
    void on_yes_btn_clicked();

private:
    Ui::load_fund_group *ui;
};

#endif // LOAD_FUND_GROUP_H
