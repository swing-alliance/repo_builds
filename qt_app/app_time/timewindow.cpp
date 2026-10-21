#include "timewindow.h"
#include "ui_timewindow.h"
#include <core/apptime.h>
#include<QString>
#include<QRadioButton>
#include<ctime>

TimeWindow::TimeWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TimeWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QString now_str=QString::fromStdString(core::getnow_string());
    QDateTime current_dt = QDateTime::fromString(now_str, "yyyy-MM-dd HH:mm:ss");
    if (current_dt.isValid()) {
        ui->dateTimeEdit->setDateTime(current_dt);
    } else {
        qDebug() << "时间字符串转换 QDateTime 失败，原始字符串为:" << now_str;
    }
    connect(ui->no_btn,&QPushButton::clicked,this,[this](){
        this->close();
    });
    connect(ui->yes_btn, &QPushButton::clicked, this, [this](){
        if (ui->recover_btn->isChecked()) {
            time_t current_offset = core::get_time_offset();
            core::usetime_offset(-current_offset); // 或者直接根据你的底层接口看是置0还是减去当前偏移
            qDebug() << "Reset time offset to 0";
        } else {
            QString t_str = this->ui->dateTimeEdit->text();
            time_t t = core::intf_time(t_str.toStdString());
            time_t now = core::getnow();
            time_t offset = t - now;
            core::usetime_offset(offset);
            qDebug() << "Set custom offset:" << offset;
        }

        qDebug() << QString::fromStdString(core::getnow_string());
        emit time_updated();
        this->close();
    });
}

TimeWindow::~TimeWindow()
{
    delete ui;
}
