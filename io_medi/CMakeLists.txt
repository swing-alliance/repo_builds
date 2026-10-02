cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(io_medi VERSION 1.0.0 LANGUAGES CXX)


add_library(io_medi STATIC
    src/file_medi.cpp
)


# 3. 【核心关键】告诉编译器去哪里找 <io_medi/file_medi.h>
target_include_directories(io_medi PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


