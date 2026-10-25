#include "chartwidget.h"
#include "fund_card.h"
#include "fund_pannel_widget.h"
#include "ui_fund_pannel_widget.h"
#include <QApplication>
#include <QList>
#include <QVBoxLayout>
#include <model/fund_model.h>
#include <QLineEdit>
#include "v_pack_pannel.h"
#include <core/paths.h>

fund_pannel_widget::fund_pannel_widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fund_pannel_widget)
{
    ui->setupUi(this);
    g_fund_manager = new model::datas_fund_manager();
    ui->info_label->setText("基本信息");

    connect(this, &fund_pannel_widget::init_fund_v_pack, this, [this](){
        this->init_v_sim_pack();
    });

    // 【修改点】：这里通过直接设置 visible 过滤，不再做 remove/add 操作，从而完美保护排序
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (!this->g_fund_manager) return;
        QString search_text = text.trimmed().replace(u'（', u'(').replace(u'）', u')');

        QList<fund_card*> all_cards = this->findChildren<fund_card*>();

        this->setUpdatesEnabled(false);
        for (auto card : all_cards) {
            if (!card->the_fund_entity) {
                card->setVisible(false);
                continue;
            }

            auto &fm = *(card->the_fund_entity);

            // 基础条件：必须是本身可见且有效的基金
            bool base_valid = fm.visiable && fm.fund_valid;

            // 搜索条件：代号或名称匹配
            QString code = QString::fromStdString(fm.fund_code);
            QString name = QString::fromStdString(fm.fund_name);
            bool matches_search = search_text.isEmpty() ||
                                  code.contains(search_text, Qt::CaseInsensitive) ||
                                  name.contains(search_text, Qt::CaseInsensitive);

            // 同时满足基础条件和搜索条件才显示，否则隐藏（不改变 Layout 布局顺序）
            card->setVisible(base_valid && matches_search);
        }
        this->setUpdatesEnabled(true);
        this->update();
    });

    connect(this, &fund_pannel_widget::need_refresh_cards_entity, this, [this](){
        this->refresh_cards_entity();
    });   // 刷新 card 和实体的地址
}

fund_pannel_widget::~fund_pannel_widget()
{
    if(chart_widget)
    {
        chart_widget->close();
        chart_widget = nullptr;
    }
    delete g_fund_manager;
    delete ui;
}

void fund_pannel_widget::init_v_sim_pack()
{
    if(this->g_fund_manager == nullptr){
        this->g_fund_manager = new model::datas_fund_manager;
        this->g_fund_manager->std_load_data(core::funds_row_dir);
    }
    if(v_sim_pack == nullptr){
        v_sim_pack = new v_sim::v_sim_pack;
        v_sim_pack->v_account.ac_id = "user";
        v_sim_pack->v_account.ac_val = 10000;
        v_sim_pack->v_global_tracker.fund_datasource = *(this->g_fund_manager); // 底层设计引起的拷贝
    }
    if(v_pannel == nullptr){
        v_pannel = new v_pack_pannel(this->v_sim_pack);
        ui->v_pack_layout->addWidget(v_pannel);
    }
}

void fund_pannel_widget::refresh_cards_entity() { // 排序后的实体底层 cpp 类，card 会指向错乱
    if (!this->g_fund_manager) return;
    QList<fund_card*> all_cards = this->findChildren<fund_card*>();
    for(auto &card : all_cards){
        bool found = false;
        for(auto &fm : this->g_fund_manager->funds_list){
            if(fm.fund_code == card->fund_code){
                card->the_fund_entity = &fm;
                found = true;
                break;
            }
        }
        if (!found) {
            card->the_fund_entity = nullptr;
            card->hide();
            qDebug() << "刷新实体出错";
        }
    }
}

void fund_pannel_widget::do_v_buy(float val, QString fund_code)
{
    if(this->v_sim_pack == nullptr){ return; }
    bool ok = this->v_sim_pack->v_account.v_buy_fund(v_sim_pack->v_global_tracker, fund_code.toStdString(), val);
    if(!ok){ qDebug() << "虚拟操作失败"; }
    v_sim_pack->v_global_tracker.v_export_logs();
}

void fund_pannel_widget::do_v_sell(float ratio, QString fund_code)
{
    if(this->v_sim_pack == nullptr){ return; }
    bool ok = this->v_sim_pack->v_account.v_sell_fund(v_sim_pack->v_global_tracker, fund_code.toStdString(), ratio);
    if(!ok){ qDebug() << "虚拟操作失败"; }
    v_sim_pack->v_global_tracker.v_export_logs();
}

void fund_pannel_widget::connect_funds_ui()
{
    if (!g_fund_manager) return;
    this->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->setUpdatesEnabled(false);

    QLayoutItem *child;
    while ((child = ui->verticalcontainer->takeAt(0)) != nullptr) {
        if (QWidget *widget = child->widget()) {
            widget->deleteLater();
        }
        delete child;
    }
    qDebug() << this->g_fund_manager->funds_list[1].fund_name << "空?";
    for(model::fund_model &fm : this->g_fund_manager->funds_list){
        if(fm.visiable == true && fm.fund_valid == true){
            fund_card *card = new fund_card(&fm, QString::fromStdString(fm.fund_code), this);
            card->align_info_ui();
            connect(card, &fund_card::dir_chart, this, &fund_pannel_widget::on_card_dir_chart);
            connect(card, &fund_card::buy_me, this, [this](float val, QString fund_code) { this->do_v_buy(val, fund_code); });
            connect(card, &fund_card::sell_me, this, [this](float ratio, QString fund_code) { this->do_v_sell(ratio, fund_code); });
            ui->verticalcontainer->addWidget(card);
        }
    }
    this->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
}

void fund_pannel_widget::refresh_funds_ui()
{
    if (!this->g_fund_manager) return;
    this->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->setUpdatesEnabled(false);

    for (model::fund_model &fm : this->g_fund_manager->funds_list) {
        auto pair = fm.get_split_fund_model(1);
        if (pair.first == -1 || pair.second == -1) {
            fm.visiable = false;
        } else {
            fm.visiable = true;
        }
    }

    QList<fund_card*> all_cards = this->findChildren<fund_card*>();
    for (auto card : all_cards) {
        ui->verticalcontainer->removeWidget(card);
        card->hide();
    }

    for (model::fund_model &fm : this->g_fund_manager->funds_list) {
        fund_card* target_card = nullptr;
        for (auto card : all_cards) {
            if (!card->the_fund_entity) continue;
            if (card->the_fund_entity->fund_code == fm.fund_code) {
                target_card = card;
                break;
            }
        }
        if (target_card) {
            target_card->the_fund_entity = &fm;
            if (fm.visiable && fm.fund_valid) {
                target_card->show();
                ui->verticalcontainer->addWidget(target_card);
            }
        }
    }
    this->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
    this->update();
}