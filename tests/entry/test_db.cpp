#include <iostream>
#include "io_medi/db_medi.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    // 1. 解决 Windows 控制台中文乱码：设置控制台输出代码页为 UTF-8 (65001)
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    // 2. 定义你要执行的 SQL 语句
    std::string sql = "SELECT * FROM fund_info LIMIT 2;";

    std::cout << "=== 正在查询 fund_info 表 ===" << std::endl;

    // 3. 调用 io_medi::db_exec_stream
    io_medi::db_exec_stream(sql, [](const std::string& row_data, int col_count) {
        // 每当底层查到符合条件的一行就会进入这里
        std::cout << "[成功获取行] 字段数量: " << col_count << std::endl;
        std::cout << "行内容详情: " << row_data << std::endl;
        });

    std::cout << "=== 查询执行完毕 ===" << std::endl;
    return 0;
}