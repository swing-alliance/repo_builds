#include "model/fund_model.h"
#include "model/model_interpreter.h"
#include <vector>
#include<io_medi/file_medi.h>
#include<core/paths.h>
#include <string>
#include <iostream>
#include<filesystem>
#include<io_medi/db_medi.h>
#include<io_medi/buf_medi.h>
#include <algorithm>
#include<thread>
namespace fs = std::filesystem;

namespace model {
	// 1. 低级从文件系统加载（用于最小化验证、计算测试,服务器低负担部署）
    void datas_fund_manager::low_level_load_data(const std::string& file_path) {
        if (!fs::exists(file_path) || !fs::is_directory(file_path)) {
            std::cerr << "错误：无效的目录路径！" << std::endl;
            return;
        }
        this->funds_list.clear();
        this->funds_list.reserve(10000);
        for (const auto& entry : fs::directory_iterator(file_path)) {
            if (entry.is_regular_file()) {
                auto path = entry.path();
                std::string extension = path.extension().string();
                if (extension == ".csv") {
                    std::string fund_code = path.stem().string();
                    model::fund_model fdmd;
                    fdmd.fund_code = fund_code;
                    std::string buf = io_medi::readFileToBuffer(path.string());
                    model_int::mem_to_obj_fund(&fdmd, &buf);
                    // 优化 2：必须用 std::move！把 fdmd “移”进去，拒绝复制大内存！
                    this->funds_list.push_back(std::move(fdmd));
                }
            }
        }
        return;
    }

    void datas_fund_manager::fast_low_level_load_data(const std::string& file_path, int num_threads) {
        if (!fs::exists(file_path) || !fs::is_directory(file_path)) {
            std::cerr << "错误：无效的目录路径！" << std::endl;
            return;
        }
        std::vector<std::filesystem::path> csv_paths;
        for (const auto& entry : fs::directory_iterator(file_path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".csv") {
                csv_paths.push_back(entry.path());
            }
        }
        if (csv_paths.empty()) {
            this->funds_list.clear();
            return;
        }
        this->funds_list.clear();
        this->funds_list.reserve(csv_paths.size());
        if (num_threads <= 0) num_threads = 4;
        size_t total_files = csv_paths.size();
        num_threads = static_cast<int>(std::min(static_cast<size_t>(num_threads), total_files));
        auto worker = [&](size_t start_idx, size_t end_idx, std::vector<model::fund_model>& local_results) {
            local_results.reserve(end_idx - start_idx);
            for (size_t i = start_idx; i < end_idx; ++i) {
                const auto& path = csv_paths[i];
                std::string fund_code = path.stem().string();
                model::fund_model fdmd;
                fdmd.fund_code = fund_code;
                std::string buf = io_medi::readFileToBuffer(path.string());
                model_int::mem_to_obj_fund(&fdmd, &buf);
                local_results.push_back(std::move(fdmd));
            }
            };
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<std::vector<model::fund_model>> thread_results(num_threads);
        size_t chunk_size = total_files / num_threads;
        size_t remainder = total_files % num_threads;
        size_t current_start = 0;
        for (int i = 0; i < num_threads; ++i) {
            size_t current_chunk_size = chunk_size + (i < remainder ? 1 : 0);
            size_t current_end = current_start + current_chunk_size;

            if (current_start < current_end) {
                threads.emplace_back(worker, current_start, current_end, std::ref(thread_results[i]));
            }
            current_start = current_end;
        }
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        for (auto& local_vec : thread_results) {
            for (auto& fdmd : local_vec) {
                this->funds_list.push_back(std::move(fdmd));
            }
        }
        return;
    }



	// 2. 标准全量加载（适配后台计算和后期 Qt UI 展示）
    void datas_fund_manager::std_load_data(const std::string& file_path) {
        this->low_level_load_data(file_path);
		std::string sql = "SELECT fund_code, fund_name, fund_type, fund_info, update_time FROM fund_info;";
        io_medi::db_exec_stream(sql, [this](const std::string& row_data, int col_num) {
            for (int i = 0;i < this->data_nums();i++) {
                if(this->funds_list[i].fund_code == io_medi::buf_split_line(row_data,0,'|')) {
                    // 填充其他字段
                    this->funds_list[i].fund_name = io_medi::buf_split_line(row_data,1,'|');
                    this->funds_list[i].fund_type = io_medi::buf_split_line(row_data,2,'|');
                    this->funds_list[i].fund_info = io_medi::buf_split_line(row_data,3,'|');
                    this->funds_list[i].update_time = io_medi::buf_split_line(row_data,4,'|');
                }
            }
        });
    }



}

