#pragma once

#include <string>

namespace fund {

    class DbTest {
    public:
        /**
         * @brief 传入 .db 文件路径，随机获取 fund_info 表中的一条数据
         * @param db_path SQLite 数据库文件的路径
         * @return 格式化后的记录字段字符串
         */
        static std::string get_random_fund_info(const std::string& db_path);
    };

} // namespace fund