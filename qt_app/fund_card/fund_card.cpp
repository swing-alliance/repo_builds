#include "fund_card.h"
#include "ui_fund_card.h"
#include <QString>
#include <QStyleOption>
#include <QPainter>
void fund_card::align_info_ui()
{
    if(this->the_fund_entity != nullptr){
        ui->code_label->setText(QString::fromStdString(this->the_fund_entity->fund_code));
        ui->name_label->setText(QString::fromStdString(this->the_fund_entity->fund_name));
    }
}
fund_card::fund_card(model::fund_model *entity, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fund_card)
    , the_fund_entity(entity) // 初始化时直接赋指针
{
    ui->setupUi(this);
    this->setStyleSheet(
        "QWidget#fund_card:hover {"
        "    border: 3px solid black;"
        "    border-radius: 5px;"
        "}"
        );

}

fund_card::~fund_card()
{
    delete ui;
}

void fund_card::paintEvent(QPaintEvent *event)
{
    // 让自定义 QWidget 能够支持 QSS 的 border 和 background
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


