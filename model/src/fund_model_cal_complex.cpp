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
            double w_calmar = 0.20;   // Calmar
            double w_sharpe = 0.20;   // Sharpe
            double w_sortino = 0.15;   // Sortino
            double w_recover = 0.10;   // Recovery Factor
            double w_vol_penalty = 0.05;   // 波动惩罚
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
    //  复合排序：BALANCED
    // ============================================================
    void sort_COMPLEX_BALANCED_stratagy(datas_fund_manager& dfm,
        const sort_options& options)
    {
        const bool desc = (options.order == sort_order::DESC);
        const int  top_k = options.top_k;

        // ---- 1. 刷新每只基金的指标（必须用引用！）----
        for (fund_model& f : dfm.funds_list) {
            f.get_recover_factor_ret(1);
            f.get_sortino_ret(1);
            f.get_span_ann_ret(1);
            f.get_span_calmar_ret(1);
            f.get_span_drawdown_ret(1);
            f.get_span_sharpe_ret(1);
            f.get_span_vol_ret(1);
            f.get_split_fund_model(1);   // 若内部有副作用则保留；无则删掉
        }

        // ---- 2. 计算综合评分，先全部隐藏 ----
        const BalancedWeights w;

        struct Scored { fund_model* f; double score; };
        std::vector<Scored> scored;
        scored.reserve(dfm.funds_list.size());

        for (fund_model& f : dfm.funds_list) {
            f.visiable = false;
            if (f.fund_valid == 0) continue;

            const double s = compute_balanced_score(f.cal_results, w);
            if (!std::isfinite(s)) continue;

            scored.push_back({ &f, s });
        }

        // ---- 3. 排序 ----
        std::sort(scored.begin(), scored.end(),
            [desc](const Scored& a, const Scored& b) {
                return desc ? a.score > b.score : a.score < b.score;
            });

        // ---- 4. 设置 visiable，只亮前 top_k ----
        int limit = (top_k > 0) ? top_k : static_cast<int>(scored.size());
        if (limit > static_cast<int>(scored.size()))
            limit = static_cast<int>(scored.size());

        for (int i = 0; i < limit; ++i)
            scored[i].f->visiable = true;
    }




} // namespace model