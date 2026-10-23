#ifndef CAL_PARAM_WIDGET_H
#define CAL_PARAM_WIDGET_H
#include <QButtonGroup>
#include <QWidget>
#include <model/opt_enum.h>
#include <model/fund_model.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class cal_param_Widget;
}
QT_END_NAMESPACE

class cal_param_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit cal_param_Widget(model::datas_fund_manager& fmm,QWidget *parent = nullptr);
    ~cal_param_Widget() override;
    model::sort_options get_sort_option();
private:
    Ui::cal_param_Widget *ui;
    QButtonGroup* m_radioGroup=new QButtonGroup;
};
#endif // CAL_PARAM_WIDGET_H
