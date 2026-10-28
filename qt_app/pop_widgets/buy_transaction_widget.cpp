#include "buy_transaction_widget.h"
#include "ui_buy_transaction_widget.h"
#include <QPushButton>
#include<QIcon>
#include<QShortcut>
buy_transaction_widget::buy_transaction_widget(int buy_type_index,QString buy_code,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::buy_transaction_widget)
{
    ui->setupUi(this);
    setWindowTitle("添加买入");
    QShortcut* shortcut_enter = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(shortcut_enter, &QShortcut::activated, ui->yes_btn, &QPushButton::click);
    setWindowIcon(QIcon("src/infinite.png"));
    ui->comboBox->addItem("基金");
    ui->comboBox->addItem("股票");
    ui->comboBox->setCurrentIndex(buy_type_index);
    ui->comboBox->setEnabled(false);
    ui->code_symbol_lbl->setText(buy_code);
    connect(ui->no_btn,&QPushButton::clicked,this,[this](){this->deleteLater();this->close();});
    connect(ui->yes_btn,&QPushButton::clicked,this,[this](){
        bool ok = false;
        float value = this->ui->lineEdit->text().toFloat(&ok);
        if (ok) {emit this->buy_val(value);}
        this->deleteLater();this->close();});
}

buy_transaction_widget::~buy_transaction_widget()
{
    delete ui;
}
