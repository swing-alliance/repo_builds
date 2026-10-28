#include "addgroup.h"
#include "ui_addgroup.h"
#include<io_medi/buf_medi.h>
#include<io_medi/db_medi.h>
#include<QMessageBox>
#include<QIcon>
#include<qshortcut>
addgroup::addgroup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::addgroup)
{
    ui->setupUi(this);
    QShortcut* shortcut_enter = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(shortcut_enter, &QShortcut::activated, ui->yes_btn, &QPushButton::click);

    setWindowTitle("添加分组");
    setWindowIcon(QIcon("src/infinite.png"));

}

addgroup::~addgroup()
{
    delete ui;
}

void addgroup::on_yes_btn_clicked()
{
    QString group_name = ui->lineEdit_g_name->text().trimmed();
    QString group_description = ui->lineEdit_g_description->text().trimmed();
    if (group_name.isEmpty())
    {
        QMessageBox::warning(this, "提示", "分组名称不能为空");
        return;
    }
    std::string sql =
        "INSERT INTO group_info (group_name, group_members) VALUES ('"
        + group_name.toStdString()
        + "', NULL)";
    io_medi::db_exec_stream(sql, [](const std::string&, int){});
    this->deleteLater();
    this->close();
}



void addgroup::on_no_btn_clicked()
{
    this->deleteLater();
    this->close();
}

