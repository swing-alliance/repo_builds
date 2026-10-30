#include "customchartview.h"
#include "ui_customchartview.h"

#include <QVBoxLayout>
#include <QChart>
#include <QValueAxis>

customchartview::customchartview(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::customchartview)
{
    ui->setupUi(this);
    ui->doubleSpinBox_similarity->setRange(0.0, 1.0);   // 顺便锁死范围
    ui->doubleSpinBox_similarity->setDecimals(2);       // 两位小数，才能显示 0.50
    ui->doubleSpinBox_similarity->setSingleStep(0.01);
    ui->doubleSpinBox_similarity->setValue(0.50);

    // 采样数默认 30
    ui->spinBox_samplenum->setRange(0, 1000);           // 按需调整范围
    ui->spinBox_samplenum->setSingleStep(1);
    ui->spinBox_samplenum->setValue(30);

    // ---------- 创建图表 ----------
    QChart* chart = new QChart;
    QValueAxis* axisX = new QValueAxis;
    QValueAxis* axisY = new QValueAxis;

    axisX->setRange(0, 100);
    axisY->setRange(0, 2);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    // ---------- 创建画布（强制 0,1 起始）----------
    m_drawer = new ChartDrawer(chart, this);

    // ---------- 放入 ui->container ----------
    QVBoxLayout* layout = new QVBoxLayout(ui->container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_drawer);
    ui->container->setLayout(layout);

    // ---------- 采样数量同步 ----------
    m_drawer->setSampleCount(ui->spinBox_samplenum->value());

    connect(ui->spinBox_samplenum, QOverload<int>::of(&QSpinBox::valueChanged),
            m_drawer, &ChartDrawer::setSampleCount);

    // ---------- 抬起笔后发出最终结果 ----------
    connect(m_drawer, &ChartDrawer::penUp, this,
            [this](const std::vector<DataPoint>& data)
            {
                double similarity = ui->doubleSpinBox_similarity->value();
                int sampleCount   = ui->spinBox_samplenum->value();
                bool to_group=ui->radioButton->isChecked();
                emit resultReady(data, similarity, sampleCount,to_group);
            });
}

customchartview::~customchartview()
{
    delete ui;
}


