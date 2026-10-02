#include "tests/db_test.h"
#include "sqlite3.h"
#include <sstream>
#include <iostream>

namespace fund {

    std::string DbTest::get_random_fund_info(const std::string& db_path) {
        sqlite3* db = nullptr;
        // 1. 打开 SQLite 数据库
        int rc = sqlite3_open(db_path.c_str(), &db);
        if (rc != SQLITE_OK) {
            std::string err_msg = "无法打开数据库: ";
            if (db) {
                err_msg += sqlite3_errmsg(db);
                sqlite3_close(db);
            }
            else {
                err_msg += "内存分配失败或路径无效";
            }
            return err_msg;
        }

        // 2. 编写 SQL：利用 SQLite 的 RANDOM() 随机抽取一行
        const char* sql = "SELECT * FROM fund_info ORDER BY RANDOM() LIMIT 1;";
        sqlite3_stmt* stmt = nullptr;

        // 3. 预编译 SQL 语句
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::string err_msg = "SQL 预编译失败: " + std::string(sqlite3_errmsg(db));
            sqlite3_close(db);
            return err_msg;
        }

        std::stringstream ss;
        // 4. 执行查询并解析结果
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int col_count = sqlite3_column_count(stmt);
            ss << "=== 成功随机获取一条基金信息 (" << db_path << ") ===\n";

            for (int i = 0; i < col_count; ++i) {
                const char* col_name = sqlite3_column_name(stmt, i);
                const unsigned char* col_val = sqlite3_column_text(stmt, i);

                ss << "  [" << (col_name ? col_name : "unknown") << "] = "
                    << (col_val ? reinterpret_cast<const char*>(col_val) : "NULL") << "\n";
            }
        }
        else if (rc == SQLITE_DONE) {
            ss << "提示: fund_info 表中没有找到任何数据。\n";
        }
        else {
            ss << "查询执行出错: " << sqlite3_errmsg(db) << "\n";
        }

        // 5. 清理释放资源
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return ss.str();
    }

} // namespace fund