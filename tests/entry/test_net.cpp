#include<net/client.h>
#include<core/paths.h>
#include<core/config.h>
#include<core/apptime.h>
#include <filesystem>
#include<iostream>
#include<vector>
namespace fs = std::filesystem;
int main() {
	core::init_default_config();
    int count = 0;
    std::vector<std::string> names;
    for (const auto& entry : fs::directory_iterator(core::funds_row_dir)) {
        
        // 判断是否是普通文件
        if (entry.is_regular_file()) {
            std::string namne = entry.path().stem().string();
            std::cout << "file: " <<namne  << std::endl;
            names.push_back(namne);
            // 获取完整路径：entry.path().string()
        }
        //if (count == 200) { break; }
        count++;
    }
    std::cout << "start_t" << core::getnow_string() << std::endl;
    net::quick_pull_csv_file(names);
    std::cout << "end_t" << core::getnow_string() << std::endl;

}