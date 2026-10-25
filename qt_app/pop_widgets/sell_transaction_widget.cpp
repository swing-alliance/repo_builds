#include "sell_transaction_widget.h"
#include "ui_sell_transaction_widget.h"

sell_transaction_widget::sell_transaction_widget(int sell_type_index,QString sell_code,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::sell_transaction_widget)
{
    ui->setupUi(this);
    ui->comboBox->addItem("基金");
    ui->comboBox->addItem("股票");
    ui->comboBox->setCurrentIndex(sell_type_index);
    ui->comboBox->setEnabled(false);
    ui->code_symbol_lbl->setText(sell_code);
    connect(ui->no_btn,&QPushButton::clicked,this,[this](){this->deleteLater();this->close();});
    connect(ui->yes_btn,&QPushButton::clicked,this,[this](){
        bool ok = false;
        float ratio = this->ui->lineEdit->text().toFloat(&ok);
        if(ratio<0 ||ratio>1){ok=false;}
        if (ok) {emit this->sell_ratio(ratio);}
        this->deleteLater();this->close();});
}

sell_transaction_widget::~sell_transaction_widget()
{
    delete ui;
}
