#include "model/fund_model.h"
#include <vector>
#include <string>
#include <core/apptime.h>
#include <core/appconst.h>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <thread>
#include <cmath>       // 引入 std::log 和 std::sqrt 的头文件
#include <algorithm>
#include "model/opt_enum.h"

//for scalable app

namespace model {
    std::pair<int, int> fund_model::get_split_fund_model(int days) {
        std::pair<int, int> p;
        p.first = -1;
        p.second = -1;
        time_t now_t = core::getnow();
        std::string init_t_str = this->data[0].time;
        time_t init_t = core::intf_time(init_t_str);
        if (now_t < init_t) { return p; }
        int start_i = 0;
        int end_i = this->data.size() - 1;
        while (start_i <= end_i) {
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
        while (cpy_i != 0 && cpy_days != 0) {
            cpy_i--;
            cpy_days--;
        }
        p.first = cpy_i;
        return p;
    }

    bool fund_model::get_span_ann_ret(int days) {
        auto p = this->get_split_fund_model(days);
        if (p.first == -1 || p.second == -1 || (p.second - p.first) < 10) { return false; }//数据量小于十直接失败
        float ann_ret = (this->data[p.second].accum_val - this->data[p.first].accum_val) / (this->data[p.first].accum_val) * 249 / days;
        this->cal_results.span_annum_result = ann_ret;
        return true;
    }

    bool fund_model::get_span_drawdown_ret(int days) {
        auto p = this->get_split_fund_model(days);  //得到数据段索引
        if (p.first == -1 || p.second == -1 || (p.second - p.first) < 10) { return false; }//数据量小于十直接失败
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
        if (p.first == -1 || p.second == -1 || (p.second - p.first) < 10) { return false; }  // 数据量小于十直接失败
        const auto& vec = this->data;
        int n = p.second - p.first + 1;   // 数据点个数
        std::vector<float> rets;          // 1. 算对数收益率序列
        rets.reserve(n - 1);
        for (int i = p.first + 1; i <= p.second; i++) {
            float prev = vec[i - 1].accum_val;
            float cur = vec[i].accum_val;
            if (prev <= 0.0f || cur <= 0.0f) return false;  // 防非法值/除零
            rets.push_back(std::log(cur / prev));
        }
        float sum = 0.0f;                 // 2. 均值
        for (float r : rets) sum += r;
        float mean = sum / rets.size();
        float sq = 0.0f;                  // 3. 方差（样本，除以 n-1）
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

    bool fund_model::get_span_sharpe_ret(int days) {
        if (this->get_span_ann_ret(days) == true && this->get_span_vol_ret(days) == true) {
            this->cal_results.sharpe_result = (this->cal_results.span_annum_result - core::no_risk_rt_ratio) / this->cal_results.volatility_result;
            return true;
        }
        return false;
    }

    bool fund_model::get_span_calmar_ret(int days) {
        if (this->get_span_drawdown_ret(days) && this->get_span_ann_ret(days)) {
            double mdd = std::abs(this->cal_results.max_drawdown_result);
            if (mdd < 0.008) {          // 0.8% 阈值
                return false;
            }
            this->cal_results.calmar_result =
                this->cal_results.span_annum_result / mdd;
            return true;
        }
        return false;
    }

    bool fund_model::get_sortino_ret(int days) {
        auto range = get_split_fund_model(days);
        if (range.first < 0 || range.second <= range.first) {
            return false;
        }
        int n = range.second - range.first;
        if (n <= 10) return false;
        std::vector<double> rets;
        rets.reserve(n);
        for (int i = range.first + 1; i <= range.second; ++i) {
            rets.push_back(this->data[i].daily_return);
        }
        if (rets.empty()) return false;
        double sum = 0.0;
        for (double r : rets) sum += r;
        double mean = sum / rets.size();
        const double rf_daily = 0.0;
        double downside_sq = 0.0;
        int    downside_n = 0;
        for (double r : rets) {
            double diff = r - rf_daily;
            if (diff < 0.0) {
                downside_sq += diff * diff;
                ++downside_n;
            }
        }
        if (downside_n == 0) return false;
        double downside_std = std::sqrt(downside_sq / downside_n);
        double ann_ret = mean * 252.0;
        double ann_std = downside_std * std::sqrt(252.0);
        if (ann_std == 0.0) return false;
        this->cal_results.sortino_result = ann_ret / ann_std;
        return true;
    }

    bool fund_model::get_recover_factor_ret(int days) {
        auto range = get_split_fund_model(days);
        if (range.first < 10 || range.second <= range.first) {
            return false;
        }
        int start = range.first;
        int end = range.second;
        double peak_nav = this->data[start].accum_val;
        int    peak_i = start;
        double max_dd = 0.0;
        int    dd_peak_i = start;
        int    dd_trough_i = start;
        for (int i = start; i <= end; ++i) {
            double nav = this->data[i].accum_val;
            if (nav > peak_nav) {
                peak_nav = nav;
                peak_i = i;
            }
            if (peak_nav > 0.0) {
                double dd = (peak_nav - nav) / peak_nav;
                if (dd > max_dd) {
                    max_dd = dd;
                    dd_peak_i = peak_i;
                    dd_trough_i = i;
                }
            }
        }
        if (max_dd <= 0.0) {
            double s = this->data[start].accum_val;
            double e = this->data[end].accum_val;
            if (s <= 0.0) return false;
            this->cal_results.recover_factor_result = (e - s) / s;
            return true;
        }
        double peak_value = this->data[dd_peak_i].accum_val;
        int    recover_i = -1;
        for (int i = dd_trough_i + 1; i <= end; ++i) {
            if (this->data[i].accum_val >= peak_value) {
                recover_i = i;
                break;
            }
        }
        double trough_nav = this->data[dd_trough_i].accum_val;
        if (trough_nav <= 0.0) return false;
        double recover_nav = (recover_i >= 0)
            ? this->data[recover_i].accum_val
            : this->data[end].accum_val;
        this->cal_results.recover_factor_result =
            (recover_nav - trough_nav) / trough_nav;
        return true;
    }

    void fund_model::print_cal_result() {
        std::cout << "code:" << this->fund_code << " ann:" << this->cal_results.span_annum_result
            << " sharpe:" << this->cal_results.sharpe_result << " vol:" << this->cal_results.volatility_result
            << " drawn:" << this->cal_results.max_drawdown_result << " calmar:" << this->cal_results.calmar_result
            << " sortino:" << this->cal_results.sortino_result << " recover:" << this->cal_results.recover_factor_result << std::endl;
    }

    void datas_fund_manager::fast_get_all_cal_results(int days, int num_threads) {
        size_t total_funds = this->funds_list.size();
        if (total_funds == 0) { return; }
        if (num_threads <= 0) { num_threads = 4; }
        if (static_cast<size_t>(num_threads) > total_funds) { num_threads = static_cast<int>(total_funds); }
        auto worker = [&](size_t start_idx, size_t end_idx) {
            for (size_t i = start_idx; i < end_idx; ++i) {
                this->funds_list[i].get_span_ann_ret(days);
                this->funds_list[i].get_span_calmar_ret(days);
                this->funds_list[i].get_span_sharpe_ret(days);
                this->funds_list[i].get_span_vol_ret(days);
                this->funds_list[i].get_span_drawdown_ret(days);
                this->funds_list[i].get_recover_factor_ret(days);
                this->funds_list[i].get_sortino_ret(days);
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
    }

    void datas_fund_manager::print_top_code(int nums) {
        int cal_num = std::min(static_cast<size_t>(nums), this->funds_list.size());
        int i = 0;
        while (i < cal_num) {
            if (this->funds_list[i].visiable == true) {
                std::cout << "Fund Code: " << this->funds_list[i].fund_code << std::endl;
                this->funds_list[i].print_cal_result();
                i++;
            }
            else {
                i++; // 避免不可见时陷入死循环
            }
        }
    }

    void datas_fund_manager::sort_by_options(const sort_options& options) {
        const bool desc = (options.order == sort_order::DESC);
        const int  top_k = options.top_k;
        float cal_results::* ptr = nullptr;

        if (options.field == sort_field::COMPLEX_BALANCED) {
            sort_COMPLEX_BALANCED_stratagy(*this, options);
            return;
        }
        if (options.field == sort_field::COMPLEX_SRP_CLMA) {
            return;
        }

        switch (options.field) {
        case sort_field::ANN_RET:          ptr = &cal_results::span_annum_result;    break;
        case sort_field::CALMAR:           ptr = &cal_results::calmar_result;        break;
        case sort_field::SHARPE:           ptr = &cal_results::sharpe_result;        break;
        case sort_field::VOLATILITY:       ptr = &cal_results::volatility_result;    break;
        case sort_field::MAX_DRAWDOWN:     ptr = &cal_results::max_drawdown_result;  break;
        case sort_field::SORTINO:          ptr = &cal_results::sortino_result;       break;
        case sort_field::RECOVERY_FACTOR:  ptr = &cal_results::recover_factor_result; break;
        default:                           break;
        }

        if (ptr != nullptr) {
            std::sort(funds_list.begin(), funds_list.end(), [ptr, desc, &options](const fund_model& a, const fund_model& b) {
                auto is_valid = [&](const fund_model& f) {
                    if (f.fund_valid == 0) return false;
                    if (options.skip_invalid && f.cal_results.*ptr == -9999) return false;
                    return true;
                    };
                bool a_ok = is_valid(a);
                bool b_ok = is_valid(b);
                if (a_ok != b_ok) return a_ok;
                if (!a_ok && !b_ok) return false;
                float va = a.cal_results.*ptr;
                float vb = b.cal_results.*ptr;
                return desc ? va > vb : va < vb;
                });
        }

        int limit = (top_k > 0) ? top_k : (int)funds_list.size();
        int valid_cnt = 0;
        for (auto& f : funds_list) {
            bool ok = (f.fund_valid != 0) && !(options.skip_invalid && f.cal_results.*ptr == -9999 && f.cal_results.sharpe_result == -9999
                && f.cal_results.calmar_result == -9999);
            if (ok && valid_cnt < limit) {
                f.visiable = true;
                ++valid_cnt;
            }
            else {
                f.visiable = false;
            }
        }
    }

}//namespace model