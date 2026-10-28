#include "cal_param_widget.h"
#include "ui_cal_param_widget.h"
#include <model/opt_enum.h>
#include <QString>
#include<QIcon>
#include<QShortcut>
cal_param_Widget::cal_param_Widget(model::datas_fund_manager& fmm,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::cal_param_Widget)
{
    ui->setupUi(this);
    QShortcut* shortcut_enter = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(shortcut_enter, &QShortcut::activated, ui->yes_btn, &QPushButton::click);
    setWindowTitle("计算参数");
    setWindowIcon(QIcon("src/infinite.png"));
    ui->dayslineEdit->setText("60");
    m_radioGroup->addButton(ui->calma_btn);
    m_radioGroup->addButton(ui->maxdrawn_btn);
    m_radioGroup->addButton(ui->sharpe_btn);
    m_radioGroup->addButton(ui->sotino_btn);
    m_radioGroup->addButton(ui->volatility_btn);
    m_radioGroup->addButton(ui->yearly_btn);
    m_radioGroup->addButton(ui->recover_factor_btn);
    m_radioGroup->addButton(ui->complex_balance_btn);
    ui->sharpe_btn->setChecked(true);
    ui->sort_rule_comb->addItem("DESC");
    ui->sort_rule_comb->addItem("ASC");
    ui->lineEdit->setText("-1");
    ui->filter_btn->click();
    ui->sort_rule_comb->setCurrentIndex(0);
    connect(ui->yes_btn,&QPushButton::clicked,this,[&](){
        auto option=this->get_sort_option();
        qDebug()<<static_cast<int>(option.field);
        qDebug()<<static_cast<int>(option.order);
        qDebug()<<static_cast<int>(option.skip_invalid);
        qDebug()<<static_cast<int>(option.top_k);
        bool ok;
        int cal_days = ui->dayslineEdit->text().toInt(&ok);
        if (!ok){cal_days = 60;}
        fmm.fast_get_all_cal_results(cal_days,8);   //先算完数据到内存

        fmm.sort_by_options(option);   //原地排序
        qDebug()<<"排序结束了，第一是"<<fmm.funds_list[0].fund_code;
        emit param_accepted();
        this->close();
        this->deleteLater();
    });
    connect(ui->no_btn,&QPushButton::clicked,this,[this](){this->close();this->deleteLater();});

}

cal_param_Widget::~cal_param_Widget()
{
    delete ui;
}

model::sort_options cal_param_Widget::get_sort_option()
{
    QAbstractButton* btn = m_radioGroup->checkedButton();
    QString sorttext = btn ? btn->text() : "";
    QString ordertext=ui->sort_rule_comb->currentText();
    model::sort_options output;
    output.skip_invalid=ui->filter_btn->isChecked();
    bool ok;
    int topK = ui->lineEdit->text().toInt(&ok);
    if (!ok){topK = -1;qDebug() << "top_k parse failed, use default -1, raw:" << ui->lineEdit->text();}
    output.top_k=topK;
    if (!model::from_string(ordertext.toStdString(), output.order)){qDebug() << "error serious get sort option";}
    if (!model::from_string(sorttext.toStdString(), output.field)){qDebug() << "error serious get sort option";}
    return output;
}
