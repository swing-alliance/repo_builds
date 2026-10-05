#include<net/client.h>

#include<core/config.h>
#include<core/apptime.h>
#include <filesystem>
#include<iostream>
#include<vector>
namespace fs = std::filesystem;

std::string find_souce_path() {
    const fs::path target_tail = fs::path("my_types") / "Equity";
    fs::path current = fs::current_path();
    while (true) {
        // 1) 先看当前层直接拼接能不能命中（快路径）
        fs::path direct = current / target_tail;
        if (fs::exists(direct) && fs::is_directory(direct)) {
            return direct.string();
        }

        // 2) 再在当前层向下递归搜索（能覆盖兄弟包）
        std::error_code ec;
        for (auto it = fs::recursive_directory_iterator(
            current, fs::directory_options::skip_permission_denied, ec);
            it != fs::recursive_directory_iterator(); it.increment(ec)) {
            if (ec) break;

            const fs::path& p = it->path();
            if (it->is_directory() &&
                p.filename() == "Equity" &&
                p.parent_path().filename() == "my_types") {
                return p.string();
            }

            // 可选：避免向下钻进已经巨大的无关目录（如 .git、build）
            if (it->is_directory()) {
                const std::string name = p.filename().string();
                if (name == ".git" || name == "build" || name == "node_modules") {
                    it.disable_recursion_pending();
                }
            }
        }
        if (!current.has_parent_path() || current.parent_path() == current) {
            break;  // 到根了
        }
        current = current.parent_path();
    }
    throw std::runtime_error("找不到 my_types/Equity 目录");
}

int main() {
std::cout<<find_souce_path()<<std::endl;
    core::init_default_config();
    int count = 0;
    std::vector<std::string> names;
    for (const auto& entry : fs::directory_iterator(find_souce_path())) {

        // 判断是否是普通文件
        if (entry.is_regular_file()) {
            std::string namne = entry.path().stem().string();
            //std::cout << "file: " << namne << std::endl;
            names.push_back(namne);
            // 获取完整路径：entry.path().string()
        }
        //if (count == 200) { break; }
        count++;
    }
    std::cout << "start_t" << core::getnow_string() << std::endl;
    net::quick_pull_csv_file_general(names, 400,find_souce_path());
    std::cout << "end_t" << core::getnow_string() << std::endl;

}