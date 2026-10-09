cmake_minimum_required(VERSION 3.15)
project(virtual VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(virtual STATIC
    src/v_account.cpp
    src/v_brokerage.cpp
    src/v_tracker.cpp
    src/v_app.cpp
)

target_include_directories(virtual PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(virtual PUBLIC
    core          # ← 确认 core 存在，否则删掉或先建
    model
    io_medi
)

# 可执行文件
add_executable(gov go_virtual.cpp)
target_link_libraries(gov PRIVATE virtual)