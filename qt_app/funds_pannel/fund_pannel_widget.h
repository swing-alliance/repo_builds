#ifndef FUND_PANNEL_WIDGET_H
#define FUND_PANNEL_WIDGET_H
#include <QWidget>
#include <model/fund_model.h>
#include "chartwidget.h"
#include<virtual/v_account.h>
#include<virtual/v_tracker.h>
#include<virtual/v_app.h>
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
    void need_refresh_cards_entity();
    void on_card_dir_chart(model::fund_model* fund_ptr);
private slots:
    void init_v_sim_pack();
    void refresh_cards_entity();
    void do_v_buy(float val,QString fund_code);
    void do_v_sell(float ratio,QString fund_code);
private:
    Ui::fund_pannel_widget *ui;
    ChartWidget* chart_widget = nullptr;
    v_sim::v_sim_pack * v_sim_pack=nullptr;
};

#endif // FUND_PANNEL_WIDGET_H
