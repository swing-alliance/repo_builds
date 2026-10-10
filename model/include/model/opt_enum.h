//定义操作码
namespace model {

    enum class sort_field {
        // 基础单项指标
        SHARPE,              // 夏普比率（综合风险收益）
        CALMAR,              // 卡尔马比率（收益/最大回撤）
        SORTINO,             // 索提诺比率（下行风险调整收益） -- 新增推荐
        ANN_RET,             // 年化收益率
        VOLATILITY,          // 波动率
        MAX_DRAWDOWN,        // 最大回撤
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

} // namespace model