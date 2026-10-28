#include "fund_card.h"
#include "ui_fund_card.h"
#include <QString>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>
#include <QContextMenuEvent>
#include "buy_transaction_widget.h"
#include "sell_transaction_widget.h"
#include <core/paths.h>
#include <io_medi/db_medi.h>
#include<io_medi/buf_medi.h>
#include "fundadd_togroup.h"
void fund_card::align_info_ui()
{
    ui->additional_mark_label->setText("");
    if(this->the_fund_entity != nullptr){
        ui->code_label->setText(QString::fromStdString(this->the_fund_entity->fund_code));
        ui->name_label->setText(QString::fromStdString(this->the_fund_entity->fund_name));
    }
    if(this->the_fund_entity->mark_flag==true){ui->additional_mark_label->setText("🚩");}
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
    // QAction *actionMark = menu.addAction("标记");
    QAction *add_togroup=menu.addAction("加到组");
    QAction *v_buy = menu.addAction("虚拟买");
    QAction *v_sell = menu.addAction("虚拟卖");
    QAction *selectedAction = menu.exec(event->globalPos());
    if (selectedAction == nullptr)
    {
        event->accept();
        return;}
    if (selectedAction == actionRedirect)
    {emit dir_chart(this->the_fund_entity);}
    else if (selectedAction == add_togroup){
        fundadd_togroup * fdad_togp=new fundadd_togroup(this->fund_code,{},nullptr);
        fdad_togp->setWindowFlags(Qt::Dialog | Qt::Window);
        fdad_togp->setAttribute(Qt::WA_DeleteOnClose);
        fdad_togp->show();
    }else if (selectedAction == v_buy)
    {
        buy_transaction_widget *buy_widget = new buy_transaction_widget(0, this->fund_code);
        buy_widget->setWindowFlags(Qt::Window);
        buy_widget->show();
        connect(buy_widget, &buy_transaction_widget::buy_val, this, [this](float val)
                {
                    emit buy_me(val, this->fund_code);
                });
    }
    else if (selectedAction == v_sell)
    {
        sell_transaction_widget *sell_widget = new sell_transaction_widget(0, this->fund_code);
        sell_widget->setWindowFlags(Qt::Window);
        connect(sell_widget, &sell_transaction_widget::sell_ratio, this, [this](float ratio)
                {
                    emit sell_me(ratio, this->fund_code);
                });
        sell_widget->show();
    }
    event->accept();
}


