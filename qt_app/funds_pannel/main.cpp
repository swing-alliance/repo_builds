#include "fund_pannel_widget.h"
#include <core/paths.h>
#include <QApplication>
#include <model/fund_model.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    fund_pannel_widget w;
    if(w.g_fund_manager == nullptr){
        w.g_fund_manager = new model::datas_fund_manager();
    }
    w.g_fund_manager->fast_low_level_load_data(core::funds_row_dir);
    w.connect_funds_ui();
    w.show();

    return QApplication::exec();
}