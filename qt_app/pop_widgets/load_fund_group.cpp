#include "load_fund_group.h"
#include "ui_load_fund_group.h"
#include<io_medi/db_medi.h>
#include<io_medi/buf_medi.h>
#include<core/paths.h>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include<QFile>
#include <QDir>
#include<QIcon>
#include<QShortcut>
load_fund_group::load_fund_group(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::load_fund_group)
{
    ui->setupUi(this);
    QShortcut* shortcut_enter = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(shortcut_enter, &QShortcut::activated, ui->yes_btn, &QPushButton::click);
    setWindowTitle("加载分组");
    setWindowIcon(QIcon("src/infinite.png"));
    std::string sql="select group_name from group_info";
    std::vector<std::string> names_v;
    io_medi::db_exec_stream(sql,[&](const std::string& col_val, int col_num){
        names_v.push_back(io_medi::buf_split_line(col_val,0,'|'));
    });
    for(auto name:names_v){ui->comboBox->addItem(QString::fromStdString(name));}
}

load_fund_group::~load_fund_group()
{
    delete ui;
}

void load_fund_group::on_yes_btn_clicked()
{
    std::vector<QString> to_cpy_to_temp_paths;
    QString group_name = ui->comboBox->currentText();
    QString sql_template = "select group_members from group_info where group_name='%1'";
    QString escaped = group_name.replace("'", "''");
    QString query_sql = sql_template.arg(escaped);
    io_medi::db_exec_stream(query_sql.toStdString(), [&](const std::string col_val, int col_num){
        if (col_val.empty())
        {
            qDebug() << "group_members 为空";
            return;}
        QJsonParseError parseErr;
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(col_val), &parseErr);
        if (parseErr.error != QJsonParseError::NoError || !doc.isArray())
        {
            qDebug() << "JSON解析失败：" << parseErr.errorString();
            return;}
        QJsonArray arr = doc.array();
        for (auto item : arr)
        {
            if (!item.isString()) continue;
            QString fname = item.toString();
            QString srcPath = QDir(QString::fromStdString(core::funds_row_dir)).filePath(fname);
            to_cpy_to_temp_paths.push_back(srcPath);}
    });
    QString temp_path = QString::fromStdString(core::datasource);
    QString targetDir = QDir(temp_path).filePath("temp_group_fund");
    QDir dir(targetDir);
    if (dir.exists())
    {
        dir.removeRecursively();
    }
    dir.mkpath(".");
    qDebug() << "目标临时目录：" << targetDir;
    for (const auto& src : to_cpy_to_temp_paths)
    {
        QFileInfo fi(src);
        QString dst = QDir(targetDir).filePath(fi.fileName());
        QFile dstFile(dst);
        if (dstFile.exists())
        {dstFile.remove();}
        if (QFile::copy(src, dst))
        {qDebug() << "拷贝成功 " << src << " → " << dst;}
        else
        {qDebug() << "拷贝失败 " << src;}
    }
    emit this->dir_grouped_pannel(group_name+"(分组)");
    this->deleteLater();
    this->close();
}

void load_fund_group::on_no_btn_clicked()
{
    this->deleteLater();
    this->close();
}

