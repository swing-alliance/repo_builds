cmake_minimum_required(VERSION 3.15)

# 💡 强制开启 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(model VERSION 1.0.0 LANGUAGES CXX)

# ❌【必须删掉这两行】：不要在子模块里反向去 add_subdirectory 根目录或其他模块！
# add_subdirectory( ../io_medi io_medi_build)
# add_subdirectory( ../core core_build)

# 2. 定义静态库 model
add_library(model STATIC
    src/fund_model.cpp
    src/model_interpreter.cpp
    src/fund_model_cal.cpp
)

# 3. 头文件路径
target_include_directories(model PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 4. 直接链接依赖（只要最外层加载了 core 和 io_medi，这里直接写名字即可）
target_link_libraries(model PUBLIC 
    core 
    io_medi
)

# 5. 可执行程序
add_executable(model_inst
    main.cpp
)
target_link_libraries(model_inst PRIVATE model)