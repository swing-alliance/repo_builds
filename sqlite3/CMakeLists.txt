cmake_minimum_required(VERSION 3.15)
project(sqlite3_target)

# 把 sqlite3.c 直接编译成一个静态库
add_library(sqlite3 STATIC
    sqlite3.c
    sqlite3.h
)

# 允许其他模块 #include 到这里的头文件
target_include_directories(sqlite3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)