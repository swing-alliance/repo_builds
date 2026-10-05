#pragma once
#include <string>
#include <vector>

namespace model {
    class fund_row_tm_val {   // 单条净值记录
    public:
        char time [11];    // 净值日期
        float accum_val;    // 累计净值
        float daily_return;  //日回报率
    };
    class cal_results {    //计算结果
    public:
        float max_drawdown_result = -9999;
        float span_annum_result = -9999;
        float volatility_result = -9999;
        float calmar_result = -9999;
        float sharpe_result = -9999;
    };
    
    class fund_model {   // 基金模型，完整映射 fund_info（冷表）与 fund_info_flex（热表）结构
    public:
        // --- 1. 冷表字段 (fund_info：长久信息，不常变动) ---
        std::string fund_code;                // 主键 基金代码
        std::string fund_name;                // 基金名称
        std::string fund_type;                // 基金类型，混合|股票等
        std::string fund_info;                // 基金详情
        std::string update_time;              // 更新时间
        int fund_valid = 1;                   // 0无效，1有效，默认1
        // --- 2. 热表字段 (fund_info_flex：经常变动的信息) ---
        std::string stocks_info;              // 股票信息 JSON 数组字符串，只保留最新一期
        int mark_flag = 0;                    // 0未标记，1标记，默认0
        std::string group_name;               // 多分组 | 分隔，示例"消费|指数|波段"；无分组存空

        // --- 3. 业务扩展字段 ---
        std::vector<fund_row_tm_val> data;    // 净值序列（特殊，用于计算与图表展示）
        bool visiable=true;    //上层ui用，程序加载出来默认可见，在排序或者筛选后出现脏值时或者主动不想见
        //----4.计算结果字段------
        cal_results cal_results;
        //----5.相关类方法-------
        std::pair<int, int> get_split_fund_model(int days); //根据apptime得到时间向前推移得到单个fund相应净值,根据index定位fund_model,返回内部于此的起始指针
        bool get_span_ann_ret(int days);    //计算其区间年华率并且填充到结果字段
        bool get_span_drawdown_ret(int days);
        bool get_span_vol_ret(int days);
        bool get_span_sharpe_ret(int days);
        bool get_span_calmar_ret(int days);
        void print_cal_result();

    };

    

    // 数据管理者类（放在命名空间外或内都可以，按你的原样保留）
    class datas_fund_manager {
    public:
        std::vector<model::fund_model> funds_list;
        // 内联函数：实时获取数据总量，避免手动的 data_nums 产生不同步 Bug
        inline size_t data_nums() const {
            return funds_list.size();
        }
        void low_level_load_data(const std::string& file_path);    // 适配计算
        void std_load_data(const std::string & file_path);    // 适配计算和ui

        
        void get_max_drawndown_fund(int days);
        void get_per_annum_rated_fund(int days);
        void get_volatility_rated_fund(int days);
        void get_calmar_rated_fund(int days);      //卡尔马，年化/最大回撤
        void get_sharpe_rated_fund(int days);      //夏普
    };





} // namespace model

