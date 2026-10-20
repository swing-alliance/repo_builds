#include<net/client.h>
#include<core/config.h>
#include<core/apptime.h>
#include <filesystem>
#include<iostream>
#include<fstream>
#include<vector>
#include <sstream>
#include <thread>

namespace fs = std::filesystem;

std::string find_source_path() {
    const fs::path target_tail = fs::path("my_types") / "Equity";
    fs::path current = fs::current_path();
    while (true) {
        fs::path direct = current / target_tail;
        if (fs::exists(direct) && fs::is_directory(direct)) {
            return direct.string();
        }
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
            if (it->is_directory()) {
                const std::string name = p.filename().string();
                if (name == ".git" || name == "build" || name == "node_modules") {
                    it.disable_recursion_pending();
                }
            }
        }
        if (!current.has_parent_path() || current.parent_path() == current) {
            break;
        }
        current = current.parent_path();
    }
    throw std::runtime_error("找不到 my_types/Equity 目录");
}

std::string find_mapping_latestdate_csv()
{
    const fs::path target_file_name = "mapping_latestdate.csv";
    const fs::path target_parent_dir = "mapping";
    fs::path current = fs::current_path();
    while (true)
    {
        fs::path direct = current / target_parent_dir / target_file_name;
        if (fs::exists(direct) && fs::is_regular_file(direct))
        {
            return direct.string();
        }
        std::error_code ec;
        for (auto it = fs::recursive_directory_iterator(
            current, fs::directory_options::skip_permission_denied, ec);
            it != fs::recursive_directory_iterator(); )
        {
            if (ec)
            {
                it.increment(ec);
                continue;
            }
            const fs::path& p = it->path();
            if (fs::is_regular_file(p)
                && p.filename() == target_file_name
                && p.parent_path().filename() == target_parent_dir)
            {
                return p.string();
            }
            if (fs::is_directory(p))
            {
                const std::string name = p.filename().string();
                if (name == ".git" || name == "build" || name == "node_modules")
                {
                    it.disable_recursion_pending();
                }
            }
            it.increment(ec);
        }
        if (!current.has_parent_path() || current.parent_path() == current)
        {
            break;
        }
        current = current.parent_path();
    }
    throw std::runtime_error("找不到 mapping/mapping_latestdate.csv 文件");
}

static inline void trim_cr(std::string& s)
{
    if (!s.empty() && s.back() == '\r')
        s.pop_back();
}

// 处理一批文件：每个线程调用，无共享变量、无锁
void process_slice(const std::vector<fs::path>& files, std::ostringstream& out)
{
    for (const fs::path& p : files)
    {
        std::ifstream fin(p.string());
        if (!fin.is_open())
            continue;

        std::string line;
        std::string last_valid_line;

        // 跳过表头
        std::getline(fin, line);

        // 只保存最后一行有效行；中间行读完直接丢弃，不做任何处理
        while (std::getline(fin, line))
        {
            if (!line.empty())
            {
                last_valid_line.swap(line); // swap：O(1)，内部指针交换，不拷贝字符串内存
            }
        }
        fin.close();

        if (last_valid_line.empty())
            continue;

        // 仅对【最后一行】做一次处理
        trim_cr(last_valid_line);
        auto comma_pos = last_valid_line.find(',');
        if (comma_pos == std::string::npos)
            continue;

        std::string latest_date(last_valid_line.data(), comma_pos);
        out << p.string() << "," << latest_date << "\n";
    }
}

int main() {
    std::cout << find_source_path() << std::endl;
    core::init_default_config();

    // 收集全部csv路径
    std::vector<fs::path> all_csv;
    for (const auto& entry : fs::directory_iterator(find_source_path()))
    {
        if (!entry.is_regular_file()) continue;
        auto p = entry.path();
        if (p.extension() == ".csv")
        {
            all_csv.push_back(std::move(p));
        }
    }

    // 切分4份，4线程
    const int thread_cnt = 4;
    std::vector<std::vector<fs::path>> slices(thread_cnt);
    for (size_t i = 0; i < all_csv.size(); i++)
    {
        int idx = static_cast<int>(i % thread_cnt);
        slices[idx].push_back(std::move(all_csv[i]));
    }

    // 每个线程独立输出缓冲区，完全无锁
    std::ostringstream buf0, buf1, buf2, buf3;
    std::thread t0(process_slice, std::cref(slices[0]), std::ref(buf0));
    std::thread t1(process_slice, std::cref(slices[1]), std::ref(buf1));
    std::thread t2(process_slice, std::cref(slices[2]), std::ref(buf2));
    std::thread t3(process_slice, std::cref(slices[3]), std::ref(buf3));

    t0.join();
    t1.join();
    t2.join();
    t3.join();

    // 合并输出结果
    std::ostringstream final_buf;
    final_buf << "path,latest_date\n";
    final_buf << buf0.str();
    final_buf << buf1.str();
    final_buf << buf2.str();
    final_buf << buf3.str();
    std::string buf = final_buf.str();

    int count = 0;
    std::vector<std::string> names;
    for (const auto& entry : fs::directory_iterator(find_source_path())) {
        if (entry.is_regular_file()) {
            std::string namne = entry.path().stem().string();
            names.push_back(namne);
        }
        count++;
    }

    std::cout << "start_t" << core::getnow_string() << std::endl;
    net::quick_pull_csv_file_general(names, 400, find_source_path());

    std::cout << "Found path:" << find_mapping_latestdate_csv() << std::endl;
    std::ofstream out(find_mapping_latestdate_csv());
    out << buf;
    out.close();

    std::cout << "end_t" << core::getnow_string() << std::endl;
    core::app_stop(10000000);
    return 0;
}
