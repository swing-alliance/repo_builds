#ifndef SIMPLEPLOTWIDGET_H
#define SIMPLEPLOTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <vector>
#include <utility>
#include <string>
#include <type_traits>
#include <algorithm>

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QPainter>
#include <QString>
#include <QMouseEvent>
#include <QWheelEvent>

// ==================== 交互视图（支持平移、缩放、双击复位） ====================
class InteractiveChartView : public QChartView {

public:
    explicit InteractiveChartView(QChart *chart, QWidget *parent = nullptr)
        : QChartView(chart, parent), m_isDragging(false)
    {
        setRenderHint(QPainter::Antialiasing);
        setRubberBand(QChartView::NoRubberBand);
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_isDragging = true;
            m_lastMousePos = event->pos();
            setCursor(Qt::ClosedHandCursor);
        }
        QChartView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (m_isDragging) {
            QPointF delta = event->pos() - m_lastMousePos;
            m_lastMousePos = event->pos();
            chart()->scroll(-delta.x(), delta.y());
        }
        QChartView::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_isDragging = false;
            unsetCursor();
        }
        QChartView::mouseReleaseEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override {
        const double factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
        chart()->zoom(factor);
        event->accept();
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            chart()->zoomReset();
        }
        QChartView::mouseDoubleClickEvent(event);
    }

private:
    bool m_isDragging;
    QPoint m_lastMousePos;
};


// ==================== 泛型图表控件主类 ====================
template<typename T1, typename T2>
class SimplePlotWidget : public QWidget {
public:
    explicit SimplePlotWidget(const std::vector<std::pair<T1, T2>> &data,
                              const std::string &xDesc,
                              const std::string &yDesc,
                              QWidget *parent = nullptr)
        : QWidget(parent)
    {
        resize(750, 500);

        QLineSeries *series = new QLineSeries();
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->legend()->hide();

        double minY = 0.0;
        double maxY = 0.0;
        bool hasData = !data.empty();

        if (hasData) {
            minY = static_cast<double>(data[0].second);
            maxY = static_cast<double>(data[0].second);
        }

        // 1. 根据 X 轴类型进行分支处理
        if constexpr (std::is_same_v<T1, QString>) {
            QStringList categories;
            int index = 0;
            for (const auto &item : data) {
                categories.append(item.first);
                double yVal = static_cast<double>(item.second);
                series->append(index, yVal);

                if (yVal < minY) minY = yVal;
                if (yVal > maxY) maxY = yVal;
                index++;
            }

            QBarCategoryAxis *axisX = new QBarCategoryAxis();
            axisX->append(categories);
            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);
            axisX->setTitleText(QString::fromStdString(xDesc));
        }
        else {
            for (const auto &item : data) {
                double xVal = static_cast<double>(item.first);
                double yVal = static_cast<double>(item.second);
                series->append(xVal, yVal);

                if (yVal < minY) minY = yVal;
                if (yVal > maxY) maxY = yVal;
            }
            chart->createDefaultAxes();
            if (auto axisX = chart->axisX()) {
                axisX->setTitleText(QString::fromStdString(xDesc));
            }
        }

        // 2. 核心：严格锁定并自适应 Y 轴的比较范围，保证 Y 轴始终具有可比较的高效可视区间
        QValueAxis *axisY = new QValueAxis();
        if (hasData) {
            double padding = (maxY - minY) * 0.1; // 上下预留 10% 边距
            if (padding == 0.0) {
                padding = 1.0;
            }
            axisY->setRange(minY - padding, maxY + padding);
        }
        axisY->setTitleText(QString::fromStdString(yDesc));
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        // 3. 视图加载与布局
        InteractiveChartView *chartView = new InteractiveChartView(chart, this);
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(chartView);
        setLayout(layout);
    }
};

#endif // SIMPLEPLOTWIDGET_H