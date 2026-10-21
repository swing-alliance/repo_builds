#include "fund_pannel_widget.h"
#include "ui_fund_pannel_widget.h"
#include <model/fund_model.h>
#include <QApplication>
#include <QVBoxLayout>
#include "fund_card.h"
#include<QLineEdit>
#include <QList>
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
            fm.visiable = match;
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
        fund_card *card = new fund_card(&fm,this);
        card->align_info_ui(); // 确保渲染文本
        ui->verticalcontainer->addWidget(card);
    }
    this->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
    this->setEnabled(true);
}