#include "io_medi/db_medi.h"
#include <iostream>
#include <sqlite3.h>
#include <core/paths.h>
#include <filesystem>
#include <core/log.h>


namespace io_medi {

    void db_exec_stream(const std::string& sql, std::function<void(const std::string&, int)> on_row) {
        sqlite3* db = nullptr;
        auto db_path_obj = std::filesystem::path(core::app_db_path);
        auto parent_dir = db_path_obj.parent_path();
        if (!parent_dir.empty() && !std::filesystem::exists(parent_dir)) {
            std::filesystem::create_directories(parent_dir);
        }
        int rc = sqlite3_open(core::app_db_path.c_str(), &db);
        if (rc != SQLITE_OK) {
            std::string err_msg = db ? sqlite3_errmsg(db) : "未知错误";
            std::cerr << "无法打开数据库: " << err_msg << std::endl;
            core::log_it(err_msg);
            if (db) {
                sqlite3_close(db);
            }
            return;
        }
        auto sqlite_callback = [](void* user_data, int argc, char** argv, char** azColName) -> int {
            auto* func = static_cast<std::function<void(const std::string&, int)>*>(user_data);
            std::string row_str;
            for (int i = 0; i < argc; i++) {
                row_str += (argv[i] ? argv[i] : "NULL");
                if (i < argc - 1) {
                    row_str += "|";
                }
            }
            (*func)(row_str, argc);
            return 0; // 返回 0 表示继续查询下一行
            };

        char* err_msg = nullptr;
        rc = sqlite3_exec(db, sql.c_str(), sqlite_callback, &on_row, &err_msg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL 执行错误: " << (err_msg ? err_msg : "未知错误") << std::endl;
            if (err_msg) {
                sqlite3_free(err_msg);
            }
        }
        sqlite3_close(db);
    }

} // namespace io_medi