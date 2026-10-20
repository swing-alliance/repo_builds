cmake_minimum_required(VERSION 3.15)
project(fund_qt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 针对不同编译器处理字符集，彻底解决源码中的中文乱码问题
if (MSVC)
    add_compile_options(/utf-8)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # MinGW (GCC/Clang) 对应的 UTF-8 编码参数
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 💡 让 CMake 统领全局，自动按正确顺序编译各个子模块
add_subdirectory(core)
add_subdirectory(io_medi)
add_subdirectory(model)
add_subdirectory(tests)
add_subdirectory(sqlite3)
add_subdirectory(net)
add_subdirectory(virtual)


#cmake -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
#cmake --build build