cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(core VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# 1. 定义核心静态库 core
add_library(core STATIC
    src/paths.cpp
    src/apptime.cpp
    src/appconst.cpp
    src/log.cpp
    src/config.cpp
)

# 2. 设置头文件搜索路径
target_include_directories(core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 3. 链接第三方 json 库
target_link_libraries(core PUBLIC nlohmann_json::nlohmann_json)

# 4. （可选）如果你有内部测试的可执行程序
# add_executable(test_app main.cpp)
# target_link_libraries(test_app PRIVATE core)