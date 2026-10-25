#include "v_pack_pannel.h"
#include "ui_v_pack_pannel.h"
#include <QTimer>
#include<vector>
#include <QLayout>
v_pack_pannel::v_pack_pannel(v_sim::v_sim_pack* sim_pack,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::v_pack_pannel)
{
    ui->setupUi(this);
    ui->ac_id->setText(QString::fromStdString(sim_pack->v_account.ac_id));
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, [=]() {
        ui->account_balance->setText(QString::number(sim_pack->v_account.ac_val, 'f', 2));
        std::vector<std::string> describe_vector = sim_pack->v_global_tracker.export_float_val();
        QLayoutItem *child;
        while ((child = ui->v_pack_layout->takeAt(0)) != nullptr) {
            if (child->widget()) {
                delete child->widget();
            }
            delete child;
        }
        if (describe_vector.empty()) {
            QLabel *lbl = new QLabel("暂无持仓", this);
            lbl->setStyleSheet("color: gray;");
            ui->v_pack_layout->addWidget(lbl);
        } else {
            for (const std::string& desc : describe_vector) {
                QLabel *lbl = new QLabel(QString::fromStdString(desc), this);
                lbl->setStyleSheet("padding: 3px;"); // 可选：加点间距美化
                ui->v_pack_layout->addWidget(lbl);
            }
        }
    });
}

v_pack_pannel::~v_pack_pannel()
{
    delete ui;
}
