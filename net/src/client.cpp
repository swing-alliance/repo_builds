#include <iostream>
#include <net/httplib.h>
#include "net/client.h"
#include <core/paths.h>
#include <core/config.h>
#include <nlohmann/json.hpp>
#include <io_medi/file_medi.h>
#include <string>
#include <core/log.h>
#include <thread>
#include <atomic>
#include <vector>
namespace net {
    serverconfig g_server_config;
    void serverconfig::init_config() {
        if (this->is_configured == true) { return; }
        this->server_host = core::get_conf<std::string>("rest_server_host", "err");
        this->server_port = core::get_conf<int>("rest_server_port", 0);
        this->is_configured = true;
        return;
    }
    int pull_csv_file(std::string csv_name) {
        g_server_config.init_config();
        httplib::Client cli(g_server_config.server_host, g_server_config.server_port);
        cli.set_connection_timeout(5, 0);
        cli.set_read_timeout(10, 0);
        nlohmann::json req_body;
        req_body["username"] = "zyh";
        req_body["password"] = "123456";
        req_body["fund_code"] = csv_name;
        std::string json_str = req_body.dump();
        auto res = cli.Post("/fund_csv", json_str, "application/json");
        if (res) {
            if (res->status == 200) {
                std::string buf = res->body;
                std::string target_path = core::funds_row_dir + "/" + csv_name + ".csv";
                io_medi::writeBufferToFile(target_path, buf);
            }
            else {
                core::log_it("Failed to pull CSV file: " + csv_name + ", HTTP status: " + std::to_string(res->status) + ", body: " + res->body);
                return -1;
            }
        }
        else {
            core::log_it("Failed to pull CSV file: " + csv_name + ", Connection failed (res is nullptr).");
            return -1;
        }
        return 0;
    }
    void quick_pull_csv_file(const std::vector<std::string>& csv_names) {
        if (csv_names.empty()) { return; }
        g_server_config.init_config();
        const size_t total_files = csv_names.size();
        const int max_threads = 400;
        int actual_threads = static_cast<int>(total_files < max_threads ? total_files : max_threads);
        core::log_it("Starting multi-threaded pull for " + std::to_string(total_files) + " files using " + std::to_string(actual_threads) + " threads.");
        std::atomic<size_t> current_index(0);
        std::atomic<int> fail_count(0);
        std::vector<std::thread> worker_threads;
        worker_threads.reserve(actual_threads);
        for (int i = 0; i < actual_threads; ++i) {
            worker_threads.emplace_back([&csv_names, &current_index, &fail_count, total_files]() {
                while (true) {
                    size_t index = current_index.fetch_add(1, std::memory_order_relaxed);
                    if (index >= total_files) { break; }
                    if (pull_csv_file(csv_names[index]) != 0) { fail_count.fetch_add(1, std::memory_order_relaxed); }
                }
                });
        }
        for (auto& t : worker_threads) { if (t.joinable()) { t.join(); } }
        core::log_it("Multi-threaded pull finished. Total: " + std::to_string(total_files) + ", Failed: " + std::to_string(fail_count.load()));
    }
}