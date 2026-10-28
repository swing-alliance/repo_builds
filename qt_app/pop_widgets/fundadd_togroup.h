#ifndef FUNDADD_TOGROUP_H
#define FUNDADD_TOGROUP_H

#include <QWidget>

namespace Ui {
class fundadd_togroup;
}

class fundadd_togroup : public QWidget
{
    Q_OBJECT

public:
    explicit fundadd_togroup(QString fund_code,std::vector<QString> fund_codes={} ,QWidget *parent = nullptr);
    ~fundadd_togroup();
    Ui::fundadd_togroup *ui;

private slots:

    void on_yes_btn_clicked();

    void on_no_btn_clicked();

private:
    std::vector<QString>fund_codes;
    QString fund_code;

};

#endif // FUNDADD_TOGROUP_H
