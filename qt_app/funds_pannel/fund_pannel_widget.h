#ifndef FUND_PANNEL_WIDGET_H
#define FUND_PANNEL_WIDGET_H
#include <QWidget>
#include <model/fund_model.h>
#include "chartwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class fund_pannel_widget;
}
class fund_card;
class ChartWidget;
QT_END_NAMESPACE

class fund_pannel_widget : public QWidget
{
    Q_OBJECT
public:
    model::datas_fund_manager * g_fund_manager=nullptr;
    void connect_funds_ui();
    void refresh_funds_ui();
    explicit fund_pannel_widget(QWidget *parent = nullptr);
    ~fund_pannel_widget() override;

signals:
    void on_card_dir_chart(model::fund_model* fund_ptr);
private slots:

private:
    Ui::fund_pannel_widget *ui;
    ChartWidget* chart_widget = nullptr;
};

#endif // FUND_PANNEL_WIDGET_H
