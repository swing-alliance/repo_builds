cmake_minimum_required(VERSION 3.15)
project(fund_qt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 如果是 MSVC 编译器，强制指定源码字符集为 UTF-8，彻底解决中文乱码和换行报错
if(MSVC)
    add_compile_options(/utf-8)
endif()
# 💡 让 CMake 统领全局，它会自动帮你把 core、io_medi、model、tests 按正确顺序编译
add_subdirectory(core)
add_subdirectory(io_medi)
add_subdirectory(model)
add_subdirectory(tests)
add_subdirectory(sqlite3)
add_subdirectory(net)
add_subdirectory(virtual)