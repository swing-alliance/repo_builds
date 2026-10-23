#include "fund_card.h"
#include "ui_fund_card.h"
#include <QString>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>
#include <QContextMenuEvent>
void fund_card::align_info_ui()
{
    if(this->the_fund_entity != nullptr){
        ui->code_label->setText(QString::fromStdString(this->the_fund_entity->fund_code));
        ui->name_label->setText(QString::fromStdString(this->the_fund_entity->fund_name));
    }
}
fund_card::fund_card(model::fund_model *entity,QString fund_code, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fund_card)
    , the_fund_entity(entity) // 初始化时直接赋指针一次，排序后失效
    ,fund_code(fund_code)
{
    ui->setupUi(this);
    this->setStyleSheet(
        "QWidget#fund_card:hover {"
        "    border: 3px solid black;"
        "    border-radius: 5px;"
        "}"
        );
    this->setContextMenuPolicy(Qt::DefaultContextMenu);
}

fund_card::~fund_card()
{
    delete ui;
}

void fund_card::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void fund_card::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QAction *actionRedirect = menu.addAction("转到图");
    QAction *actionMark = menu.addAction("标记");
    QAction * v_buy=menu.addAction("虚拟买");
    QAction * v_sell=menu.addAction("虚拟卖");
    //menu.addSeparator(); // 分割线
    QAction *selectedAction = menu.exec(event->globalPos());
    this->style()->unpolish(this);
    this->style()->polish(this);
    this->update();
    if (selectedAction == actionRedirect) {
        emit dir_chart(this->the_fund_entity);
    } else if (selectedAction == actionMark) {
        // TODO
    }else if(selectedAction==v_buy){

    }else if(selectedAction==v_sell){

    }
}

