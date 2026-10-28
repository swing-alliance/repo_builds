#ifndef DELGROUP_H
#define DELGROUP_H

#include <QWidget>

namespace Ui {
class delgroup;
}

class delgroup : public QWidget
{
    Q_OBJECT

public:
    explicit delgroup(QWidget *parent = nullptr);
    ~delgroup();

private slots:
    void on_yes_btn_clicked();

    void on_no_btn_clicked();

private:
    Ui::delgroup *ui;
};

#endif // DELGROUP_H
