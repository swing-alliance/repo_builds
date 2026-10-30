#include "chart_drawer.h"
#include <QChart>
#include <QValueAxis>
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <QDebug>

ChartDrawer::ChartDrawer(QChart *chart, QWidget *parent)
    : QChartView(chart, parent)
{
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
}

void ChartDrawer::clearDraw()
{
    if (m_drawSeries) {
        chart()->removeSeries(m_drawSeries);
        delete m_drawSeries;
        m_drawSeries = nullptr;
    }
    m_exportedData.clear();
}

void ChartDrawer::setSampleCount(int count)
{
    if (count > 0)
        m_targetSampleCount = count;
}

std::vector<DataPoint> ChartDrawer::getExportData() const
{
    return m_exportedData;
}

void ChartDrawer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (chart()->plotArea().isEmpty()) {
            event->ignore();
            return;
        }

        clearDraw();
        m_isDrawing = true;

        m_drawSeries = new QLineSeries();
        m_drawSeries->setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap));
        chart()->addSeries(m_drawSeries);

        // 绑定坐标轴
        if (!chart()->axes(Qt::Horizontal).isEmpty())
            m_drawSeries->attachAxis(chart()->axes(Qt::Horizontal).first());
        if (!chart()->axes(Qt::Vertical).isEmpty())
            m_drawSeries->attachAxis(chart()->axes(Qt::Vertical).first());

        // ★ 强制落笔点永远是 (0, 1.0)
        m_drawSeries->append(0.0, Y_START);

        emit penDown(0.0, Y_START);

        event->accept();
        return;
    }
    QChartView::mousePressEvent(event);
}

void ChartDrawer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDrawing && m_drawSeries)
    {
        if (chart()->plotArea().isEmpty())
            return;

        QPointF valPt = chart()->mapToValue(event->pos());

        double minX = 0.0, maxX = 100.0;
        if (!chart()->axes(Qt::Horizontal).isEmpty()) {
            auto axisX = qobject_cast<QValueAxis*>(chart()->axes(Qt::Horizontal).first());
            if (axisX) {
                minX = axisX->min();
                maxX = axisX->max();
            }
        }

        double clampedX = qBound(minX, valPt.x(), maxX);
        double clampedY = qBound(Y_MIN, valPt.y(), Y_MAX);

        // 避免重复点
        if (m_drawSeries->count() > 0) {
            QPointF last = m_drawSeries->at(m_drawSeries->count() - 1);
            if (std::abs(clampedX - last.x()) < 1e-5 && std::abs(clampedY - last.y()) < 1e-5)
                return;
        }

        m_drawSeries->append(clampedX, clampedY);
        event->accept();
        return;
    }
    QChartView::mouseMoveEvent(event);
}

void ChartDrawer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isDrawing)
    {
        m_isDrawing = false;

        // 先采样
        resampleAndClean();

        // 发出完整 vector
        emit penUp(m_exportedData);

        event->accept();
        return;
    }
    QChartView::mouseReleaseEvent(event);
}

void ChartDrawer::resampleAndClean()
{
    if (!m_drawSeries || m_drawSeries->count() < 2)
        return;

    QList<QPointF> rawPoints = m_drawSeries->points();

    // 稳定排序，保证 (0,1) 在最前面
    std::stable_sort(rawPoints.begin(), rawPoints.end(),
                     [](const QPointF& a, const QPointF& b) {
                         return a.x() < b.x();
                     });

    // 去重
    QList<QPointF> sortedPoints;
    for (const QPointF& p : rawPoints) {
        if (sortedPoints.isEmpty() || std::abs(p.x() - sortedPoints.last().x()) > 1e-4)
            sortedPoints.append(p);
    }

    if (sortedPoints.size() < 2)
        return;

    double minX = sortedPoints.first().x();
    double maxX = sortedPoints.last().x();
    if (std::abs(maxX - minX) < 1e-4)
        return;

    m_exportedData.clear();
    m_exportedData.reserve(m_targetSampleCount);

    for (int i = 0; i < m_targetSampleCount; ++i)
    {
        double targetX = minX + static_cast<double>(i) / (m_targetSampleCount - 1) * (maxX - minX);
        double targetY = Y_START;

        for (int j = 0; j < sortedPoints.size() - 1; ++j)
        {
            double x1 = sortedPoints[j].x();
            double x2 = sortedPoints[j + 1].x();
            if (targetX >= x1 && targetX <= x2)
            {
                double y1 = sortedPoints[j].y();
                double y2 = sortedPoints[j + 1].y();
                if (std::abs(x2 - x1) < 1e-5)
                    targetY = y1;
                else
                    targetY = y1 + (targetX - x1) * (y2 - y1) / (x2 - x1);
                break;
            }
        }

        if (targetX <= minX) targetY = sortedPoints.first().y();
        if (targetX >= maxX) targetY = sortedPoints.last().y();

        targetY = qBound(Y_MIN, targetY, Y_MAX);
        m_exportedData.push_back({targetX, targetY});
    }

    // 用采样结果重新画干净曲线
    m_drawSeries->clear();
    for (const auto& pt : m_exportedData)
        m_drawSeries->append(pt.x, pt.y);
}