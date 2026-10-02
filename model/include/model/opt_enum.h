//定义操作码
namespace model {

    enum class sort_field {
        SHARPE,      // 夏普
        CALMAR,      // 卡尔马
        ANN_RET,     // 年化收益
        VOLATILITY,  // 波动率
        MAX_DRAWDOWN,// 最大回撤
        SCORE,       // 综合分
    };

    enum class sort_order {
        DESC,   // 降序（大的在前）
        ASC,    // 升序（小的在前）
    };

    struct sort_options {
        sort_field  field = sort_field::SHARPE;
        sort_order  order = sort_order::DESC;
        int         top_k = -1;        // -1 表示不限制
        bool        skip_invalid = true; // 跳过 -9999
    };

} // namespace model