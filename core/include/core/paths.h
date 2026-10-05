#pragma once

#include <string>

namespace core {

std::string get_exe_dir(); // 获取当前 exe 执行所在目录

std::string get_project_root(const std::string& root_name = "fund_qt"); // 获取项目根

extern std::string pj_root_dir;   // 项目根目录
extern std::string datasource;    // 项目根/datasource
extern std::string funds_row_dir; // 项目根/datasource/funds_row
extern std::string app_db_path;        // 项目根/datasource/app.db
extern std::string log_path;       // 项目根/datasource/logs
extern std::string config_path;    // 项目根/datasource/configs

} // namespace core