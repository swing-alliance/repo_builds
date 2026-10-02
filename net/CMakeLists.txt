cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(net VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)



# 2. 声明并拉取 nlohmann_json（同理，如果它也偶尔下载失败，也可以加加速前缀）
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# 3. 定义 net 静态库目标
add_library(net STATIC
    src/client.cpp
)

# 4. 设置头文件路径，把下载好的 cpp-httplib 路径暴露给编译器
target_include_directories(net PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${cpp-httplib_SOURCE_DIR}>
)

# 5. 链接依赖
target_link_libraries(net PUBLIC 
    core 
    io_medi
    nlohmann_json::nlohmann_json
)