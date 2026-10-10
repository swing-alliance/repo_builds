#include"model/fund_model.h"
#include<vector>
#include<string>
#include<core/apptime.h>
#include<core/appconst.h>
#include<cstdio>
#include<iostream>
#include<time.h>
#include<thread>
#include"model/opt_enum.h"
#include<algorithm>
//for scalable app


namespace model{
    std::pair<int,int> fund_model::get_split_fund_model(int days) {
        std::pair<int, int> p;
        p.first = -1;
        p.second = -1;
        time_t now_t = core::getnow();
        std::string init_t_str = this->data[0].time;
        time_t init_t = core::intf_time(init_t_str);
        if (now_t < init_t) { return p; }
        int start_i = 0;
        int end_i = this->data.size() - 1;
        while (start_i<=end_i) {
            int mid_i = (start_i + end_i) / 2;
            std::string mid_t = this->data[mid_i].time;
            if (core::intf_time(mid_t) <= now_t) {
                p.second = mid_i;
                start_i = mid_i + 1;
            }
            else {
                end_i = mid_i - 1;
            }
        }
        int cpy_i = p.second;
        int cpy_days = days;
        while (cpy_i != 0 && cpy_days!=0) {
            cpy_i--;
            cpy_days--;
        }
        p.first = cpy_i;
        return p;
    }  
    bool fund_model::get_span_ann_ret(int days) {
        auto p = this->get_split_fund_model(days);
        if (p.first == -1 || p.second == -1 || (p.second - p.first)< 10 ) { return false;}//数据量小于十直接失败
        float ann_ret = (this->data[p.second].accum_val-this->data[p.first].accum_val)/(this->data[p.first].accum_val)*249/days;
        this->cal_results.span_annum_result = ann_ret;
        return true;
    }

    bool fund_model::get_span_drawdown_ret(int days) {
        auto p = this->get_split_fund_model(days);  //得到数据段索引
        if (p.first == -1 || p.second == -1 || (p.second - p.first) < 10) {return false;}//数据量小于十直接失败
        const auto& vec = this->data;
        float peak = vec[p.first].accum_val;
        float max_drawdown = 0.0f;
        for (int i = p.first; i <= p.second; i++) {
            float val = vec[i].accum_val;
            if (val > peak) {
                peak = val; // 更新新高
            }
            else {
                float dd = (val - peak) / peak;
                if (dd < max_drawdown) {
                    max_drawdown = dd;
                }
            }
        }
        this->cal_results.max_drawdown_result = max_drawdown;
        return true;
    }

    bool fund_model::get_span_vol_ret(int days) {
        auto p = this->get_split_fund_model(days);  // 得到数据段索引
        if (p.first == -1 || p.second == -1 || (p.second - p.first) < 10) { return false; }  // 数据量小于十直接失败}
        const auto& vec = this->data;
        int n = p.second - p.first + 1;   // 数据点个数
        std::vector<float> rets;          // 1. 算对数收益率序列，存多个返回值log(前/后)
        rets.reserve(n - 1);
        for (int i = p.first + 1; i <= p.second; i++) {
            float prev = vec[i - 1].accum_val;
            float cur = vec[i].accum_val;
            if (prev <= 0.0f || cur <= 0.0f) return false;  // 防非法值/除零
            rets.push_back(std::log(cur / prev));
        }
        float sum = 0.0f;                // 2. 均值
        for (float r : rets) sum += r;
        float mean = sum / rets.size();
        float sq = 0.0f;                 // 3. 方差（样本，除以 n-1）
        for (float r : rets) {
            float d = r - mean;
            sq += d * d;
        }
        float var = sq / (rets.size() - 1);
        float daily_vol = std::sqrt(var); // 4. 标准差 → 年化
        float annual_vol = daily_vol * std::sqrt(249.0f);  // 你定的 249
        this->cal_results.volatility_result = annual_vol;  // 5. 存结果
        return true;
    }

    bool fund_model::get_span_sharpe_ret(int days){
        if (this->get_span_ann_ret(days) == true && this->get_span_vol_ret(days) == true) {
            this->cal_results.sharpe_result = (this->cal_results.span_annum_result - core::no_risk_rt_ratio) / this->cal_results.volatility_result;
            return true;
        }
        return false;
    }

    bool fund_model::get_span_calmar_ret(int days) {
        if (this->get_span_drawdown_ret(days) && this->get_span_ann_ret(days)) {
            double mdd = std::abs(this->cal_results.max_drawdown_result);
            if (mdd < 0.008) {          // 0.8% 阈值，可按业务调整// 回撤太小 → calmar 会被放大到失真，判定不可信
                return false;
            }
            this->cal_results.calmar_result =
                this->cal_results.span_annum_result / mdd;
            return true;
        }
        return false;
    }

    void fund_model::print_cal_result() {
        std::cout << "code:" << this->fund_code << " ann:" << this->cal_results.span_annum_result
            << " sharpe:" << this->cal_results.sharpe_result << " vol:" << this->cal_results.volatility_result
            << " drawn:" << this->cal_results.max_drawdown_result << " calmar:" << this->cal_results.calmar_result<<std::endl;
    
    }
    
    void datas_fund_manager::get_all_cal_results(int days, int num_threads) {
        size_t total_funds = this->funds_list.size();
        if (total_funds == 0) {
            return;
        }
        if (num_threads <= 0) {
            num_threads = 4;
        }
        if (static_cast<size_t>(num_threads) > total_funds) {
            num_threads = static_cast<int>(total_funds);
        }
        auto worker = [&](size_t start_idx, size_t end_idx) {
            for (size_t i = start_idx; i < end_idx; ++i) {
                this->funds_list[i].get_span_ann_ret(days);
                this->funds_list[i].get_span_calmar_ret(days);
                this->funds_list[i].get_span_sharpe_ret(days);
                this->funds_list[i].get_span_vol_ret(days);
                this->funds_list[i].get_span_drawdown_ret(days);
            }
            };
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        size_t chunk_size = total_funds / num_threads;
        size_t remainder = total_funds % num_threads;
        size_t current_start = 0;
        for (int i = 0; i < num_threads; ++i) {
            size_t current_chunk_size = chunk_size + (i < remainder ? 1 : 0);
            size_t current_end = current_start + current_chunk_size;

            if (current_start < current_end) {
                threads.emplace_back(worker, current_start, current_end);
            }
            current_start = current_end;
        }
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        return;
    }


    void datas_fund_manager::print_top_code(int nums) {
        for (size_t i = 0; i < std::min(static_cast<size_t>(nums), this->funds_list.size()); ++i) {
            std::cout << "Fund Code: " << this->funds_list[i].fund_code << std::endl;
        }
    }


    void sort_by_options(const sort_options& options) {
		
    
    
    }





    }//namespace model