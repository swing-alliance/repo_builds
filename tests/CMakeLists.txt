cmake_minimum_required(VERSION 3.15)
project(tests VERSION 1.0.0 LANGUAGES CXX)

# 1. 因为没有了 .cpp 文件，将 tests 改为 INTERFACE（纯接口/头文件库）
add_library(tests STATIC
    src/db_test.cpp
)



# 2. 暴露 tests 的头文件路径（使用 INTERFACE 关键字）
target_include_directories(tests PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 3. 传递依赖：让 tests 自动把 core 传递给使用者
target_link_libraries(tests PUBLIC 
    core 
    sqlite3
    net
    model
)

# 4. 主测试可执行程序
add_executable(test_inst
    entry/test_md.cpp
)

# 5. 可执行程序链接 tests 接口库
target_link_libraries(test_inst PRIVATE tests)

# 6. 针对 MSVC 开启 /utf-8 编码支持
if(MSVC)
  target_compile_options(test_inst PRIVATE /utf-8)
endif()