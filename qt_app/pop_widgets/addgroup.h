#ifndef ADDGROUP_H
#define ADDGROUP_H

#include <QWidget>

namespace Ui {
class addgroup;
}

class addgroup : public QWidget
{
    Q_OBJECT

public:
    explicit addgroup(QWidget *parent = nullptr);
    ~addgroup();

private slots:
    void on_yes_btn_clicked();

    void on_no_btn_clicked();

private:
    Ui::addgroup *ui;
};

#endif // ADDGROUP_H
