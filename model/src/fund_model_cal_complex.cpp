#include "model/fund_model.h"
#include <vector>
#include <string>
#include <core/apptime.h>
#include <core/appconst.h>
#include <cstdio>
#include <iostream>
#include <time.h>
#include <thread>
#include <model/opt_enum.h>
#include <algorithm>
#include <cmath>
#include <limits>

namespace model {

    // ============================================================
    //  匿名命名空间：仅本文件可见的辅助工具
    // ============================================================
    namespace {

        constexpr float INVALID_VALUE = -9999.f;

        inline bool is_valid(float v) { return v != INVALID_VALUE; }

        // 综合评分权重（业务参数，可后续挪到配置里）
        struct BalancedWeights {
            double w_ann_return = 0.30;   // 年化收益
            double w_calmar = 0.20;       // Calmar
            double w_sharpe = 0.20;       // Sharpe
            double w_sortino = 0.15;      // Sortino
            double w_recover = 0.10;      // Recovery Factor
            double w_vol_penalty = 0.05;  // 波动惩罚
            double w_dd_penalty = 0.05;   // 回撤惩罚
        };

        // 单只基金的综合分；核心指标缺失返回 -inf
        double compute_balanced_score(const cal_results& c, const BalancedWeights& w)
        {
            if (!is_valid(c.span_annum_result) ||
                !is_valid(c.sharpe_result) ||
                !is_valid(c.calmar_result) ||
                !is_valid(c.max_drawdown_result)) {
                return -std::numeric_limits<double>::infinity();
            }

            const double ann = c.span_annum_result;
            const double cal = c.calmar_result;
            const double shr = c.sharpe_result;
            const double sor = is_valid(c.sortino_result) ? c.sortino_result : 0.0;
            const double rec = is_valid(c.recover_factor_result) ? c.recover_factor_result : 0.0;
            const double vol = is_valid(c.volatility_result) ? c.volatility_result : 0.0;
            const double dd = std::fabs(c.max_drawdown_result);

            return w.w_ann_return * ann
                + w.w_calmar * cal
                + w.w_sharpe * shr
                + w.w_sortino * sor
                + w.w_recover * rec
                - w.w_vol_penalty * vol
                - w.w_dd_penalty * dd;
        }

    } // anonymous namespace


    // ============================================================
    //  复合排序：BALANCED（直接对底层 dfm.funds_list 进行原地排序）
    // ============================================================
    void sort_COMPLEX_BALANCED_stratagy(datas_fund_manager& dfm,
        const sort_options& options)
    {
        const bool desc = (options.order == sort_order::DESC);
        const int  top_k = options.top_k;
        const BalancedWeights w;

        // ---- 1. 刷新每只基金的指标 ----
        for (fund_model& f : dfm.funds_list) {
            f.get_recover_factor_ret(249);
            f.get_sortino_ret(249);
            f.get_span_ann_ret(1);
            f.get_span_calmar_ret(1);
            f.get_span_drawdown_ret(1);
            f.get_span_sharpe_ret(1);
            f.get_span_vol_ret(1);
            f.get_split_fund_model(1);
        }

        // ---- 2. 直接对底层 `dfm.funds_list` 进行原地排序 ----
        std::sort(dfm.funds_list.begin(), dfm.funds_list.end(), [&w, desc](const fund_model& a, const fund_model& b) {
            double sa = compute_balanced_score(a.cal_results, w);
            double sb = compute_balanced_score(b.cal_results, w);

            bool a_valid = (a.fund_valid != 0 && std::isfinite(sa));
            bool b_valid = (b.fund_valid != 0 && std::isfinite(sb));

            // 有效的排在无效前面
            if (a_valid != b_valid) {
                return a_valid;
            }
            // 双方都无效时按代码升序兜底
            if (!a_valid && !b_valid) {
                return a.fund_code < b.fund_code;
            }

            // 分数防抖与降序/升序
            if (std::fabs(sa - sb) > 1e-6) {
                return desc ? sa > sb : sa < sb;
            }

            // 分数相同时按代码升序兜底
            return a.fund_code < b.fund_code;
            });

        // ---- 3. 设置 visiable 显隐，只点亮前 top_k 个 ----
        int limit = (top_k > 0) ? top_k : static_cast<int>(dfm.funds_list.size());
        int valid_cnt = 0;

        for (auto& f : dfm.funds_list) {
            bool valid = (f.fund_valid != 0 && std::isfinite(compute_balanced_score(f.cal_results, w)));
            if (valid && valid_cnt < limit) {
                f.visiable = true;
                ++valid_cnt;
            }
            else {
                f.visiable = false;
            }
        }
    }

} // namespace model