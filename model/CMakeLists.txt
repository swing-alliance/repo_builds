cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(model VERSION 1.0.0 LANGUAGES CXX)


add_library(model STATIC
    src/fund_model.cpp
    src/model_interpreter.cpp
    src/fund_model_cal.cpp
 src/fund_model_cal_complex.cpp)

target_include_directories(model PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(model PUBLIC 
    core 
    io_medi
)

add_executable(model_inst
    main.cpp
 "src/fund_model_cal_complex.cpp")
target_link_libraries(model_inst PRIVATE model)