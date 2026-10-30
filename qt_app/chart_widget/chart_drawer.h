#ifndef CHART_DRAWER_H
#define CHART_DRAWER_H

#include <QChartView>
#include <QLineSeries>
#include <QMouseEvent>
#include <vector>

struct DataPoint {
    double x;
    double y;
};

class ChartDrawer : public QChartView
{
    Q_OBJECT
public:
    explicit ChartDrawer(QChart *chart, QWidget *parent = nullptr);
    void clearDraw();
    void setSampleCount(int count);
    std::vector<DataPoint> getExportData() const;

signals:
    void penDown(double x, double y);
    void penUp(const std::vector<DataPoint>& data);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void resampleAndClean();

    bool m_isDrawing = false;
    QLineSeries* m_drawSeries = nullptr;

    const double Y_MIN = 0.0;
    const double Y_MAX = 2.0;
    const double Y_START = 1.0;          // 强制落笔 Y = 1.0

    int m_targetSampleCount = 500;
    std::vector<DataPoint> m_exportedData;
};

#endif // CHART_DRAWER_H