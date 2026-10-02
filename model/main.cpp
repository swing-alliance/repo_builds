#include <iostream>
#include <string>
#include<cstdio>
#include <core/paths.h>
#include<core/apptime.h>
#include <io_medi/file_medi.h>
#include "model/fund_model.h"
#include "model/model_interpreter.h"

int main() {
    model::datas_fund_manager fd_mg;
    
    std::cout << "start_t is " << core::getnow_string() << std::endl;
    fd_mg.low_level_load_data(core::funds_row_dir);
    for (int i = 0;i < fd_mg.data_nums();i++) {
        //printf("fund_code %s ,date is %s val is %f ,return is %f\n",fd_mg.funds_list[i].fund_code.c_str(), fd_mg.funds_list[i].data[1].time, fd_mg.funds_list[i].data[1].accum_val,fd_mg.funds_list[i].data[1].daily_return);
    } 
    for (int i = 0; i < fd_mg.data_nums(); i++) {
        auto p=fd_mg.funds_list[i].get_split_fund_model(30);
        //std::cout << "about code " << fd_mg.funds_list[i].fund_code << "the start time is " << fd_mg.funds_list[i].data[p.first].time << "end time is " << fd_mg.funds_list[i].data[p.second].time << std::endl;
    }
    std::cout << "interval_t is " << core::getnow_string() << std::endl;
    for (int i = 0; i < fd_mg.data_nums(); i++) {
        fd_mg.funds_list[i].get_span_ann_ret(120);
        fd_mg.funds_list[i].get_span_calmar_ret(120);
        fd_mg.funds_list[i].get_span_drawdown_ret(120);
        fd_mg.funds_list[i].get_span_sharpe_ret(120);
        fd_mg.funds_list[i].get_span_vol_ret(120);
        fd_mg.funds_list[i].print_cal_result();
    }
    std::cout << "the length of mg is " << fd_mg.data_nums() << std::endl;
    std::cout << "end_t is " << core::getnow_string() << std::endl;
    core::app_stop(1000000);

}