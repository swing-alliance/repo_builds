#include "chartwidget.h"
#include "ui_chartwidget.h"
#include "simpleplotwidget.h"
#include <model/fund_model.h>
#include <QString>
#include <utility>
#include <vector>
#include <QVBoxLayout>

ChartWidget::ChartWidget(model::fund_model *fm, QString name,QString code,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChartWidget)
    , fm(fm)
{
    ui->setupUi(this);
    ui->code_lbl->setText(code);
    ui->name_lbl->setText(name);
    setAttribute(Qt::WA_DeleteOnClose);

    // 先填充下拉框，再连接信号，避免构造阶段触发
    ui->comboBox->addItems({"15", "30", "60", "90", "120", "256", "380","500","700","900","all"});
    ui->comboBox->setCurrentIndex(1);   // 默认选 30

    connect(ui->comboBox, &QComboBox::currentTextChanged,
            this, &ChartWidget::refresh_ui);
    connect(ui->return_btn,&QPushButton::clicked,this,[this](){
        emit this->pop_here();
    });

    m_inConstruct = false;   // 允许刷新
    refresh_ui();            // 构造完成后画一次图
}

void ChartWidget::refresh_ui()
{
    if (!fm || m_inConstruct) return;
    QString select_text = ui->comboBox->currentText();
    std::vector<std::pair<QString, float>> plot_data;
    if (select_text == "all") {
        for (const auto& row : this->fm->data) {
            QString date = QString::fromLatin1(row.time);
            plot_data.emplace_back(date, row.accum_val);
        }
    } else {
        int limit = select_text.toInt();
        auto p = this->fm->get_split_fund_model(limit);
        if (p.first == -1 && p.second == -1) return;

        for (int i = p.first; i < p.second; ++i) {
            QString date = QString::fromLatin1(this->fm->data[i].time);
            plot_data.emplace_back(date, this->fm->data[i].accum_val);
        }
    }
    // 安全清理旧图
    QLayout* lay = ui->chartcontainer->layout();
    if (!lay) {
        lay = new QVBoxLayout(ui->chartcontainer);
        ui->chartcontainer->setLayout(lay);
    }
    while (QLayoutItem* item = lay->takeAt(0)) {
        if (QWidget* old = item->widget()) {
            old->setParent(nullptr);
            delete old;
        }
        delete item;
    }
    auto* plotWidget = new SimplePlotWidget<QString, float>(plot_data, "日期", "值");
    lay->addWidget(plotWidget);
}

ChartWidget::~ChartWidget()
{
    delete ui;
}

