#ifndef CUSTOMCHARTVIEW_H
#define CUSTOMCHARTVIEW_H

#include <QWidget>
#include <vector>
#include "chart_drawer.h"   // 引入上面的 DataPoint 和 ChartDrawer

namespace Ui {
class customchartview;
}

class customchartview : public QWidget
{
    Q_OBJECT

public:
    explicit customchartview(QWidget *parent = nullptr);
    ~customchartview();

signals:
    // 抬起笔后发出：vector + similarity + 采样数量
    void resultReady(const std::vector<DataPoint>& data,
                     double similarity,
                     int sampleCount,
                     bool to_group);

private slots:
    void on_radioButton_clicked(bool checked);

private:
    Ui::customchartview *ui;
    ChartDrawer* m_drawer = nullptr;
};

#endif // CUSTOMCHARTVIEW_H