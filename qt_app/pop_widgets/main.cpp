#include "cal_param_widget.h"

#include <QApplication>
#include <model/fund_model.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    model::datas_fund_manager fmm=*new model::datas_fund_manager;
    cal_param_Widget w(fmm);
    w.show();
    return QApplication::exec();
}
