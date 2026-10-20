#include<string>
#pragma once
//定义操作码
namespace model {

    enum class sort_field {
        // 基础单项指标
        SHARPE,              // 夏普比率（综合风险收益）
        CALMAR,              // 卡尔马比率（收益/最大回撤）
        ANN_RET,             // 年化收益率
        VOLATILITY,          // 波动率
        MAX_DRAWDOWN,        // 最大回撤
        SORTINO,             // 索提诺比率（下行风险调整收益） -- 新增推荐
        RECOVERY_FACTOR,     // 修复因子（总收益/最大回撤）   -- 新增推荐

        // 复合评分规则
        COMPLEX_SRP_CLMA,    // 夏普-卡尔马综合分
        COMPLEX_BALANCED     // 全天候稳健综合分（攻守兼备）     -- 新增推荐
    };

    enum class sort_order {
        DESC,   // 降序（大的在前）
        ASC,    // 升序（小的在前）
    };

    struct sort_options {
        sort_field  field = sort_field::SHARPE;
        sort_order  order = sort_order::DESC;
        int         top_k = -1;        // 带有过滤，只显示top_k个对其中的visiable做限制，-1 表示不限制
        bool        skip_invalid = true; // 跳过 -9999
    };


    inline bool from_string(const std::string& s, sort_field& out) {
        if (s == "SHARPE") { out = sort_field::SHARPE;            return true; }
        if (s == "CALMAR") { out = sort_field::CALMAR;            return true; }
        if (s == "ANN_RET") { out = sort_field::ANN_RET;           return true; }
        if (s == "VOLATILITY") { out = sort_field::VOLATILITY;        return true; }
        if (s == "MAX_DRAWDOWN") { out = sort_field::MAX_DRAWDOWN;      return true; }
        if (s == "SORTINO") { out = sort_field::SORTINO;           return true; }
        if (s == "RECOVERY_FACTOR") { out = sort_field::RECOVERY_FACTOR;   return true; }
        if (s == "COMPLEX_SRP_CLMA") { out = sort_field::COMPLEX_SRP_CLMA;  return true; }
        if (s == "COMPLEX_BALANCED") { out = sort_field::COMPLEX_BALANCED;  return true; }
        return false;
    }

    inline bool from_string(const std::string& s, sort_order& out) {
        if (s == "DESC") { out = sort_order::DESC; return true; }
        if (s == "ASC") { out = sort_order::ASC;  return true; }
        return false;
    }

    // ============================================================
    // 新增：枚举 -> 字符串
    // ============================================================
    inline const char* to_string(sort_field f) {
        switch (f) {
        case sort_field::SHARPE:            return "SHARPE";
        case sort_field::CALMAR:            return "CALMAR";
        case sort_field::ANN_RET:           return "ANN_RET";
        case sort_field::VOLATILITY:        return "VOLATILITY";
        case sort_field::MAX_DRAWDOWN:      return "MAX_DRAWDOWN";
        case sort_field::SORTINO:           return "SORTINO";
        case sort_field::RECOVERY_FACTOR:   return "RECOVERY_FACTOR";
        case sort_field::COMPLEX_SRP_CLMA:  return "COMPLEX_SRP_CLMA";
        case sort_field::COMPLEX_BALANCED:  return "COMPLEX_BALANCED";
        }
        return "UNKNOWN";
    }

    inline const char* to_string(sort_order o) {
        switch (o) {
        case sort_order::DESC: return "DESC";
        case sort_order::ASC:  return "ASC";
        }
        return "UNKNOWN";
    }

} // namespace model