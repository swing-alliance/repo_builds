#include "delgroup.h"
#include "ui_delgroup.h"
#include<io_medi/buf_medi.h>
#include<io_medi/db_medi.h>
delgroup::delgroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::delgroup)
{
    ui->setupUi(this);
    std::string sql="select group_name from group_info";
    std::vector<std::string> names_v;
    io_medi::db_exec_stream(sql,[&](const std::string& col_val, int col_num){
        names_v.push_back(io_medi::buf_split_line(col_val,0,'|'));
    });
    for(auto name:names_v){ui->comboBox->addItem(QString::fromStdString(name));}


}

delgroup::~delgroup()
{
    delete ui;
}

void delgroup::on_yes_btn_clicked()
{
    QString to_del=ui->comboBox->currentText();
    std::string name = to_del.toStdString();
    std::string sql = "DELETE FROM group_info WHERE group_name = '" + name + "'";
    io_medi::db_exec_stream(sql,[](const std::string&, int){});
    this->deleteLater();
    this->close();
}


void delgroup::on_no_btn_clicked()
{
    this->deleteLater();
    this->close();
}

