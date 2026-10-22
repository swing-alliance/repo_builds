#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H
#include <QWidget>
#include <model/fund_model.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChartWidget;
}
QT_END_NAMESPACE

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    model::fund_model * fm = nullptr;
    void refresh_ui();
    explicit ChartWidget(model::fund_model *fm,QString name,QString code, QWidget *parent = nullptr);
    ~ChartWidget() override;
signals:
    void pop_here();
private:
    Ui::ChartWidget *ui;
    bool m_inConstruct = true; // 防止构造阶段重复刷新绘图
};

#endif // CHARTWIDGET_H
