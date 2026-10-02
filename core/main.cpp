#include "core/apptime.h"
#include <iostream>

// 编译：
// cd A:\fund_qt\core
// g++ -std=c++17 -Iinclude test.cpp src/path.cpp src/systime.cpp -o output/test.exe
// .\output\test.exe

int main() {
    std::cout << "===== systime offset test =====" << std::endl;

    // 1. 系统时间
    core::clear_time_offset();
    std::cout << "system     : " << core::getnow_string() << std::endl;

    // 2. 偏移 -7600 秒（往前 2h6m40s）
    core::usetime_offset(-7600);
    std::cout << "-7600 str  : " << core::getnow_string() << std::endl;
    std::cout << "-7600 val  : " << core::get_time_offset() << std::endl;

    // 3. 恢复系统时间
    core::clear_time_offset();
    std::cout << "clear      : " << core::getnow_string() << std::endl;
    std::cout << "offset     : " << core::get_time_offset() << std::endl;

    return 0;
}