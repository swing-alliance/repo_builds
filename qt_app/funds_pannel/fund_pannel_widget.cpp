#include "fund_pannel_widget.h"
#include "ui_fund_pannel_widget.h"
#include <model/fund_model.h>
#include <QApplication>
#include <QVBoxLayout>
#include "fund_card.h"

fund_pannel_widget::fund_pannel_widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fund_pannel_widget)
{
    ui->setupUi(this);
    g_fund_manager = new model::datas_fund_manager();

}

fund_pannel_widget::~fund_pannel_widget()
{
    delete g_fund_manager;
    delete ui;
}

void fund_pannel_widget::connect_funds_ui()
{
    if (!g_fund_manager) return;
    this->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->setUpdatesEnabled(false);
    QLayoutItem *child;
    while ((child = ui->verticalLayout->takeAt(0)) != nullptr) {
        if (QWidget *widget = child->widget()) {
            widget->deleteLater();
        }
        delete child;
    }
    qDebug()<<this->g_fund_manager->funds_list[1].fund_name<<"空?";
    for(model::fund_model &fm : this->g_fund_manager->funds_list){
        fund_card *card = new fund_card(&fm,this);
        card->align_info_ui(); // 确保渲染文本
        ui->verticalLayout->addWidget(card);
    }
    this->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
}