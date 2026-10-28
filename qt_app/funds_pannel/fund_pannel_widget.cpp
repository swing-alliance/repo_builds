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
#include <QTimer>

fund_pannel_widget::fund_pannel_widget(QWidget *parent,QString title_str)
    : QWidget(parent)
    , ui(new Ui::fund_pannel_widget)
{

    ui->setupUi(this);
    setWindowTitle("控制台");
    setWindowIcon(QIcon(":/../src/infinite.png"));
    ui->info_label->setText(title_str);
    g_fund_manager = new model::datas_fund_manager();
    ui->info_label->setText(title_str);

    connect(this, &fund_pannel_widget::init_fund_v_pack, this, [this](){
        this->init_v_sim_pack();
    });

    // 初始化防抖定时器（100ms 延迟，快速删除/输入时会不断重置，直达最终结果）
    m_search_timer = new QTimer(this);
    m_search_timer->setSingleShot(true);
    m_search_timer->setInterval(100);

    // 防抖超时后执行最终的高性能过滤与批量重排
    connect(m_search_timer, &QTimer::timeout, this, [this]() {
        if (!this->g_fund_manager) return;
        QString search_text = ui->lineEdit->text().trimmed().replace(u'（', u'(').replace(u'）', u')');
        QHash<std::string, fund_card*> code_to_card;
        for (auto card : this->findChildren<fund_card*>()) {
            if (card) {
                std::string code = card->fund_code.toStdString();
                if (code.empty() && card->the_fund_entity) {
                    code = card->the_fund_entity->fund_code;
                }
                if (!code.empty()) {
                    code_to_card[code] = card;
                }
            }
        }
        QList<fund_card*> visible_cards;
        QList<fund_card*> hidden_cards;
        for (auto &fm : this->g_fund_manager->funds_list) {
            auto it = code_to_card.find(fm.fund_code);
            if (it == code_to_card.end()) continue;
            fund_card *card = it.value();
            card->the_fund_entity = &fm; // 实时对齐指针，防止地址错位
            bool base_valid = fm.visiable && fm.fund_valid;
            QString code = QString::fromStdString(fm.fund_code);
            QString name = QString::fromStdString(fm.fund_name);
            bool matches_search = search_text.isEmpty() ||
                                  code.contains(search_text, Qt::CaseInsensitive) ||
                                  name.contains(search_text, Qt::CaseInsensitive);

            if (base_valid && matches_search) {
                visible_cards.append(card);
            } else {
                hidden_cards.append(card);
            }
        }
        this->setUpdatesEnabled(false);
        for (auto card : hidden_cards) {
            card->hide();
        }
        QLayoutItem *item;
        while ((item = ui->verticalcontainer->itemAt(0)) != nullptr) {
            ui->verticalcontainer->removeItem(item);
            delete item;
        }
        for (auto card : visible_cards) {
            card->show();
            ui->verticalcontainer->addWidget(card);
        }
        this->setUpdatesEnabled(true);
        this->update();
    });

    // 文本变化时仅启动防抖定时器，打字或连续删除时不会卡顿，停下或最终完成时瞬间输出结果
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_search_timer->start();
    });

    connect(this, &fund_pannel_widget::need_refresh_cards_entity, this, [this](){
        this->refresh_cards_entity();
    });
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

void fund_pannel_widget::batch_add_togroup(std::vector<QString> fund_codes){

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
        v_sim_pack->v_global_tracker.fund_datasource = *(this->g_fund_manager);
    }
    if(v_pannel == nullptr){
        v_pannel = new v_pack_pannel(this->v_sim_pack);
        ui->v_pack_layout->addWidget(v_pannel);
    }
}

void fund_pannel_widget::refresh_cards_entity() {
    if (!this->g_fund_manager) return;
    QList<fund_card*> all_cards = this->findChildren<fund_card*>();
    for(auto &card : all_cards){
        if (!card) continue;
        bool found = false;
        for(auto &fm : this->g_fund_manager->funds_list){
            if(fm.fund_code == card->fund_code.toStdString() ||
                (card->the_fund_entity && fm.fund_code == card->the_fund_entity->fund_code)){
                card->the_fund_entity = &fm;
                found = true;
                break;
            }
        }
        if (!found) {
            card->the_fund_entity = nullptr;
            card->hide();
            qDebug() << "刷新实体出错：未找到对应的基金代码" << card->fund_code;
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
    refresh_cards_entity();
    QList<fund_card*> all_cards = this->findChildren<fund_card*>();
    QHash<std::string, fund_card*> code_to_card;
    for (auto card : all_cards) {
        if (card) {
            std::string code = card->fund_code.toStdString();
            if (code.empty() && card->the_fund_entity) {
                code = card->the_fund_entity->fund_code;
            }
            if (!code.empty()) {
                code_to_card[code] = card;
            }
        }
    }

    QLayoutItem *item;
    while ((item = ui->verticalcontainer->itemAt(0)) != nullptr) {
        ui->verticalcontainer->removeItem(item);
        delete item;
    }
    QList<fund_card*> visible_cards;
    for (model::fund_model &fm : this->g_fund_manager->funds_list) {
        auto it = code_to_card.find(fm.fund_code);
        if (it == code_to_card.end()) continue;
        fund_card* target_card = it.value();
        target_card->the_fund_entity = &fm;

        if (fm.visiable && fm.fund_valid) {
            visible_cards.append(target_card);
        } else {
            target_card->hide();
        }
    }

    for (auto card : visible_cards) {
        card->show();
        ui->verticalcontainer->addWidget(card);
    }

    this->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
    this->update();
    m_search_timer->start();
}