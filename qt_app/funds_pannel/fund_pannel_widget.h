#ifndef FUND_PANNEL_WIDGET_H
#define FUND_PANNEL_WIDGET_H
#include <QWidget>
#include <model/fund_model.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class fund_pannel_widget;
}
class fund_card;

QT_END_NAMESPACE

class fund_pannel_widget : public QWidget
{
    Q_OBJECT

public:
    model::datas_fund_manager * g_fund_manager=nullptr;
    void connect_funds_ui();
    explicit fund_pannel_widget(QWidget *parent = nullptr);
    ~fund_pannel_widget() override;

private:
    Ui::fund_pannel_widget *ui;
};
#endif // FUND_PANNEL_WIDGET_H
