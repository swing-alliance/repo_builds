#include "timewindow.h"
#include "ui_timewindow.h"
#include <core/apptime.h>
#include <QString>
#include <QRadioButton>
#include <ctime>
#include <QDebug>

TimeWindow::TimeWindow(v_sim::v_sim_pack * v_pack_ptr, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TimeWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    // 初始化显示当前时间
    QString now_str = QString::fromStdString(core::getnow_string());
    QDateTime current_dt = QDateTime::fromString(now_str, "yyyy-MM-dd HH:mm:ss");
    if (current_dt.isValid()) {
        ui->dateTimeEdit->setDateTime(current_dt);
    } else {
        qDebug() << "时间字符串转换 QDateTime 失败，原始字符串为:" << now_str;
    }

    // 取消按钮
    connect(ui->no_btn, &QPushButton::clicked, this, [this](){
        this->close();
    });

    // 确定按钮
    connect(ui->yes_btn, &QPushButton::clicked, this, [this, v_pack_ptr](){
        // 1. 处理时间偏移恢复
        if (ui->recover_btn->isChecked()) {
            time_t current_offset = core::get_time_offset();
            core::usetime_offset(-current_offset);
            qDebug() << "Reset time offset to 0";
        }
        else {
            bool ok = false;
            int days = ui->lineEdit->text().toInt(&ok);
            if (!ok) {
                days = 0;
            }
            if (days > 0 && v_pack_ptr != nullptr) {
                qDebug() << "=== 开始执行多日跳转，天数 days =" << days;
                while (days > 0) {
                    int safety_1 = 0;
                    while (v_pack_ptr->v_global_tracker.check_is_trading_time()) {
                        core::usetime_offset(3600);
                        if (++safety_1 > 1000) break;
                    }
                    bool early_exit = false;
                    int safety_2 = 0;
                    while (v_pack_ptr->v_global_tracker.check_is_trading_time() == false) {
                        if (core::get_time_offset() > 0) {
                            core::clear_time_offset();
                            early_exit = true;
                            break;
                        }
                        core::usetime_offset(3600);
                        if (++safety_2 > 1000) {
                            early_exit = true;
                            break;
                        }
                    }
                    if (early_exit) break;
                    days--;
                }
                v_pack_ptr->v_global_tracker.v_check_to_do(v_pack_ptr->v_account);
                QString now_str = QString::fromStdString(core::getnow_string());
                QDateTime current_dt = QDateTime::fromString(now_str, "yyyy-MM-dd HH:mm:ss");
                if (current_dt.isValid()) {
                    ui->dateTimeEdit->setDateTime(current_dt);
                }
            }
            else {
                QString t_str = this->ui->dateTimeEdit->text();
                time_t t = core::intf_time(t_str.toStdString());
                time_t now = core::getnow();
                time_t offset = t - now;
                core::usetime_offset(offset);
                qDebug() << "Set custom offset by dateTimeEdit:" << offset;
            }
        }
        QString final_str = QString::fromStdString(core::getnow_string());
        qDebug() << "最终时间:" << final_str;
        emit time_updated();
        this->close();
    });
}

TimeWindow::~TimeWindow()
{
    delete ui;
}