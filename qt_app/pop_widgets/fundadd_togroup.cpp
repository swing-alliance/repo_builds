#include "fundadd_togroup.h"
#include "ui_fundadd_togroup.h"
#include "QListWidgetItem"
#include<io_medi/db_medi.h>
#include<io_medi/buf_medi.h>
#include<core/paths.h>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
fundadd_togroup::fundadd_togroup(QString fund_code,std::vector<QString> fund_codes,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fundadd_togroup)
{
    ui->setupUi(this);
    this->fund_code=fund_code;
    this->fund_codes=fund_codes;
    ui->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listWidget->clear();
    std::string sql="select group_name from group_info";
    std::vector<std::string> names_v;
    io_medi::db_exec_stream(sql,[&](const std::string& col_val, int col_num){
        names_v.push_back(io_medi::buf_split_line(col_val,0,'|'));
    });
    for(auto name:names_v){ui->listWidget->addItem(QString::fromStdString(name));}

}

fundadd_togroup::~fundadd_togroup()
{
    delete ui;
}



void fundadd_togroup::on_yes_btn_clicked()
{
    auto item_list = ui->listWidget->selectedItems();
    for(auto item:item_list){
        if(fund_codes.size()!=0){
            for(auto& fdcd:this->fund_codes){
                QString gname = item->text().replace("'", "''");
                QString sql = QString("select group_members from group_info where group_name='%1'").arg(gname);
                std::string group_members;
                io_medi::db_exec_stream(sql.toStdString(), [&](const std::string& col_val, int col_num){
                    group_members = col_val;
                });
                std::string relate_path=fdcd.toStdString()+".csv";
                std::string integrate_members;
                QString raw = QString::fromStdString(group_members).trimmed();
                QJsonArray arr;
                if (!raw.isEmpty() && raw != "null" && raw != "[]")
                {
                    QJsonParseError err;
                    QJsonDocument doc = QJsonDocument::fromJson(raw.toUtf8(), &err);
                    if (err.error == QJsonParseError::NoError && doc.isArray())
                    {
                        arr = doc.array();
                    }
                }
                QString path = QString::fromStdString(relate_path);
                bool found = false;
                for (auto v : arr)
                {
                    if (v.isString() && v.toString() == path)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found) arr.append(path);
                integrate_members = QJsonDocument(arr).toJson(QJsonDocument::Compact).toStdString();
                auto escape = [](QString s) -> QString {
                    return s.replace("'", "''");
                };
                QString sql_rewrite = "UPDATE group_info SET group_members='%1' WHERE group_name='%2'";
                QString json_val = escape(QString::fromStdString(integrate_members));
                QString group_name_val = escape(item->text());
                QString final_sql_rewrite = sql_rewrite.arg(json_val).arg(group_name_val);
                qDebug() << "执行SQL：" << final_sql_rewrite;
                std::string sql_utf8(final_sql_rewrite.toUtf8().constData());
                io_medi::db_exec_stream(sql_utf8, [](const std::string&, int) {});
            }
        }else{
            QString gname = item->text().replace("'", "''");
            QString sql = QString("select group_members from group_info where group_name='%1'").arg(gname);
            std::string group_members;
            io_medi::db_exec_stream(sql.toStdString(), [&](const std::string& col_val, int col_num){
                group_members = col_val;
            });
            std::string relate_path=fund_code.toStdString()+".csv";
            std::string integrate_members;
            QString raw = QString::fromStdString(group_members).trimmed();
            QJsonArray arr;
            if (!raw.isEmpty() && raw != "null" && raw != "[]")
            {
                QJsonParseError err;
                QJsonDocument doc = QJsonDocument::fromJson(raw.toUtf8(), &err);
                if (err.error == QJsonParseError::NoError && doc.isArray())
                {
                    arr = doc.array();
                }
            }
            QString path = QString::fromStdString(relate_path);
            bool found = false;
            for (auto v : arr)
            {
                if (v.isString() && v.toString() == path)
                {
                    found = true;
                    break;
                }
            }
            if (!found) arr.append(path);
            integrate_members = QJsonDocument(arr).toJson(QJsonDocument::Compact).toStdString();
            auto escape = [](QString s) -> QString {
                return s.replace("'", "''");
            };
            QString sql_rewrite = "UPDATE group_info SET group_members='%1' WHERE group_name='%2'";
            QString json_val = escape(QString::fromStdString(integrate_members));
            QString group_name_val = escape(item->text());
            QString final_sql_rewrite = sql_rewrite.arg(json_val).arg(group_name_val);
            qDebug() << "执行SQL：" << final_sql_rewrite;
            std::string sql_utf8(final_sql_rewrite.toUtf8().constData());
            io_medi::db_exec_stream(sql_utf8, [](const std::string&, int) {});
        }

    }
    this->deleteLater();
    this->close();
}


void fundadd_togroup::on_no_btn_clicked()
{    this->deleteLater();
    this->close();

}

