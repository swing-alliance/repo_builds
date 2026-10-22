#include "chartwidget.h"
#include "fund_card.h"
#include "fund_pannel_widget.h"
#include "ui_fund_pannel_widget.h"
#include <QApplication>
#include <QList>
#include <QVBoxLayout>
#include <model/fund_model.h>
#include<QLineEdit>


fund_pannel_widget::fund_pannel_widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fund_pannel_widget)
{
    ui->setupUi(this);
    g_fund_manager = new model::datas_fund_manager();
    ui->info_label->setText("基本信息");

    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (!this->g_fund_manager) return;
        QString search_text = text.trimmed().replace(u'（', u'(').replace(u'）', u')');
        QList<fund_card*> all_cards = this->findChildren<fund_card*>();
        QList<fund_card*> visible_cards;
        for (auto card : all_cards) {
            if (!card->the_fund_entity) continue;
            auto &fm = *(card->the_fund_entity);
            QString code = QString::fromStdString(fm.fund_code);
            QString name = QString::fromStdString(fm.fund_name);
            bool match = search_text.isEmpty() ||
                         code.contains(search_text, Qt::CaseInsensitive) ||
                         name.contains(search_text, Qt::CaseInsensitive);
            if (match) {
                visible_cards.append(card);
            }
        }
        this->setUpdatesEnabled(false);
        for (auto card : all_cards) {
            ui->verticalcontainer->removeWidget(card);
            card->hide();
        }
        for (auto card : visible_cards) {
            card->show();
        }
        for (auto card : visible_cards) {
            ui->verticalcontainer->addWidget(card);
        }
        this->setUpdatesEnabled(true);
        this->update();
    });
}

fund_pannel_widget::~fund_pannel_widget()
{
    // 关闭图表窗口，防止野指针，不再delete chart_widget（WA_DeleteOnClose自动释放）
    if(chart_widget)
    {
        chart_widget->close();
        chart_widget = nullptr;
    }
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
    while ((child = ui->verticalcontainer->takeAt(0)) != nullptr) {
        if (QWidget *widget = child->widget()) {
            widget->deleteLater();
        }
        delete child;
    }
    qDebug()<<this->g_fund_manager->funds_list[1].fund_name<<"空?";
    for(model::fund_model &fm : this->g_fund_manager->funds_list){
        if(fm.visiable==true && fm.fund_valid==true){
            fund_card *card = new fund_card(&fm,this);
            card->align_info_ui();
            connect(card, &fund_card::dir_chart, this, &fund_pannel_widget::on_card_dir_chart);
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
    QList<fund_card*> visible_cards;
    for (auto card : all_cards) {
        if (!card->the_fund_entity) continue;
        auto &fm = *(card->the_fund_entity);
        if (fm.visiable && fm.fund_valid) {
            visible_cards.append(card);
        }
    }
    for (auto card : all_cards) {
        ui->verticalcontainer->removeWidget(card);
        card->hide();
    }
    for (auto card : visible_cards) {
        card->show();
    }
    for (auto card : visible_cards) {
        ui->verticalcontainer->addWidget(card);
    }
    this->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
    this->update();
}

// void fund_pannel_widget::on_card_dir_chart(model::fund_model* fund_ptr)
// {
//     if(!fund_ptr) return;
//     chart_widget = new ChartWidget(fund_ptr,QString::fromStdString(fund_ptr->fund_name),QString::fromStdString(fund_ptr->fund_code) ,nullptr);
//     connect(chart_widget,&ChartWidget::pop_here,this,[this](){this->on_pop_chart_here();});
//     chart_widget->show();
// }
