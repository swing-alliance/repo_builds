#include <iostream>
#include "tests/db_test.h"
#include "core/paths.h"
#include <core/log.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // 强制设置 Windows 控制台输出编码为 UTF-8
    SetConsoleOutputCP(65001);
#endif

    std::string db_path = core::app_db;

    std::string result = fund::DbTest::get_random_fund_info(db_path);
    std::cout << result << std::endl;
    core::log_it("will we be fine(free)?????");
    return 0;
}