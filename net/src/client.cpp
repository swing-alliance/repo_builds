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
        if (this->is_configured) return;
        this->server_host = core::get_conf<std::string>("rest_server_host", "err");
        this->server_port = core::get_conf<int>("rest_server_port", 0);
        this->is_configured = true;
    }

    // 内部使用：复用已有 Client
    int push_csv_file(httplib::Client& cli, const std::string& csv_name) {
        nlohmann::json req_body;
        req_body["username"] = "zyh";
        req_body["password"] = "123456";
        req_body["csv_name"] = csv_name;
        req_body["csv_data"] = io_medi::readFileToBuffer(core::funds_row_dir + "/" + csv_name + ".csv");

        auto res = cli.Post("/get_csv", req_body.dump(), "application/json");
        if (!res) {
            core::log_it("Failed to push CSV file: " + csv_name + ", Connection failed (res is nullptr).");
            return -1;
        }
        if (res->status != 200) {
            core::log_it("Failed to push CSV file: " + csv_name + ", HTTP status: " + std::to_string(res->status) + ", body: " + res->body);
            return -1;
        }
        return 0;
    }

    // 内部使用：复用已有 Client
    int pull_csv_file(httplib::Client& cli, const std::string& csv_name) {
        nlohmann::json req_body;
        req_body["username"] = "zyh";
        req_body["password"] = "123456";
        req_body["fund_code"] = csv_name;

        auto res = cli.Post("/fund_csv", req_body.dump(), "application/json");
        if (!res) {
            core::log_it("Failed to pull CSV file: " + csv_name + ", Connection failed (res is nullptr).");
            return -1;
        }
        if (res->status != 200) {
            core::log_it("Failed to pull CSV file: " + csv_name + ", HTTP status: " + std::to_string(res->status) + ", body: " + res->body);
            return -1;
        }

        std::string target_path = core::funds_row_dir + "/" + csv_name + ".csv";
        io_medi::writeBufferToFile(target_path, res->body);
        return 0;
    }

    // 兼容旧接口（单文件，不复用连接，仅在非批量场景使用）
    int push_csv_file(std::string csv_name) {
        g_server_config.init_config();
        httplib::Client cli(g_server_config.server_host, g_server_config.server_port);
        cli.set_keep_alive(true);
        cli.set_connection_timeout(5, 0);
        cli.set_read_timeout(15, 0);
        cli.set_write_timeout(15, 0);
        return push_csv_file(cli, csv_name);
    }

    int pull_csv_file(std::string csv_name) {
        g_server_config.init_config();
        httplib::Client cli(g_server_config.server_host, g_server_config.server_port);
        cli.set_keep_alive(true);
        cli.set_connection_timeout(5, 0);
        cli.set_read_timeout(15, 0);
        cli.set_write_timeout(15, 0);
        return pull_csv_file(cli, csv_name);
    }

    void quick_pull_csv_file(const std::vector<std::string>& csv_names, int threads) {
        if (csv_names.empty()) return;

        g_server_config.init_config();
        const size_t total_files = csv_names.size();
        const int actual_threads = static_cast<int>(std::min(total_files, static_cast<size_t>(threads)));

        core::log_it("Starting multi-threaded pull for " + std::to_string(total_files) +
            " files using " + std::to_string(actual_threads) + " threads.");

        std::atomic<size_t> current_index{ 0 };
        std::atomic<int> fail_count{ 0 };
        std::vector<std::thread> worker_threads;
        worker_threads.reserve(actual_threads);

        for (int i = 0; i < actual_threads; ++i) {
            worker_threads.emplace_back([&]() {
                // 每个线程只创建一次 Client，并开启 keep-alive
                httplib::Client cli(g_server_config.server_host, g_server_config.server_port);
                cli.set_keep_alive(true);
                cli.set_connection_timeout(5, 0);
                cli.set_read_timeout(15, 0);
                cli.set_write_timeout(15, 0);

                while (true) {
                    size_t index = current_index.fetch_add(1, std::memory_order_relaxed);
                    if (index >= total_files) break;

                    if (pull_csv_file(cli, csv_names[index]) != 0) {
                        fail_count.fetch_add(1, std::memory_order_relaxed);
                    }
                }
                });
        }

        for (auto& t : worker_threads) {
            if (t.joinable()) t.join();
        }

        core::log_it("Multi-threaded pull finished. Total: " + std::to_string(total_files) +
            ", Failed: " + std::to_string(fail_count.load()));
    }

    void quick_push_csv_file(const std::vector<std::string>& csv_names, int threads) {
        if (csv_names.empty()) return;
        g_server_config.init_config();
        const size_t total_files = csv_names.size();
        const int actual_threads = static_cast<int>(std::min(total_files, static_cast<size_t>(threads)));

        core::log_it("Starting multi-threaded push for " + std::to_string(total_files) +
            " files using " + std::to_string(actual_threads) + " threads.");

        std::atomic<size_t> current_index{ 0 };
        std::atomic<int> fail_count{ 0 };
        std::vector<std::thread> worker_threads;
        worker_threads.reserve(actual_threads);

        for (int i = 0; i < actual_threads; ++i) {
            worker_threads.emplace_back([&]() {
                httplib::Client cli(g_server_config.server_host, g_server_config.server_port);
                cli.set_keep_alive(true);
                cli.set_connection_timeout(5, 0);
                cli.set_read_timeout(15, 0);
                cli.set_write_timeout(15, 0);

                while (true) {
                    size_t index = current_index.fetch_add(1, std::memory_order_relaxed);
                    if (index >= total_files) break;

                    if (push_csv_file(cli, csv_names[index]) != 0) {
                        fail_count.fetch_add(1, std::memory_order_relaxed);
                    }
                }
                });
        }
        for (auto& t : worker_threads) {
            if (t.joinable()) t.join();
        }
        core::log_it("Multi-threaded push finished. Total: " + std::to_string(total_files) +
            ", Failed: " + std::to_string(fail_count.load()));
    }

    // 兼容旧版 findoutfund
    int pull_csv_file_general(httplib::Client& cli, const std::string& csv_name, const std::string& target_path) {
        nlohmann::json req_body;
        req_body["username"] = "zyh";
        req_body["password"] = "123456";
        req_body["fund_code"] = csv_name;

        auto res = cli.Post("/fund_csv", req_body.dump(), "application/json");
        if (!res) {
            core::log_it("Failed to pull CSV file: " + csv_name + ", Connection failed (res is nullptr).");
            return -1;
        }
        if (res->status != 200) {
            core::log_it("Failed to pull CSV file: " + csv_name + ", HTTP status: " + std::to_string(res->status) + ", body: " + res->body);
            return -1;
        }

        std::string full_path = target_path + "/" + csv_name + ".csv";
        io_medi::writeBufferToFile(full_path, res->body);
        return 0;
    }

    // 兼容旧接口（单文件）
    int pull_csv_file_general(std::string csv_name, std::string target_path) {
        g_server_config.init_config();
        httplib::Client cli(g_server_config.server_host, g_server_config.server_port);
        cli.set_keep_alive(true);
        cli.set_connection_timeout(5, 0);
        cli.set_read_timeout(15, 0);
        cli.set_write_timeout(15, 0);
        return pull_csv_file_general(cli, csv_name, target_path);
    }

    void quick_pull_csv_file_general(const std::vector<std::string>& csv_names, int threads, std::string target_path) {
        if (csv_names.empty()) return;

        g_server_config.init_config();
        const size_t total_files = csv_names.size();
        const int actual_threads = static_cast<int>(std::min(total_files, static_cast<size_t>(threads)));

        core::log_it("Starting multi-threaded pull for " + std::to_string(total_files) +
            " files using " + std::to_string(actual_threads) + " threads.");

        std::atomic<size_t> current_index{ 0 };
        std::atomic<int> fail_count{ 0 };
        std::vector<std::thread> worker_threads;
        worker_threads.reserve(actual_threads);

        for (int i = 0; i < actual_threads; ++i) {
            worker_threads.emplace_back([&, target_path]() {
                httplib::Client cli(g_server_config.server_host, g_server_config.server_port);
                cli.set_keep_alive(true);
                cli.set_connection_timeout(5, 0);
                cli.set_read_timeout(15, 0);
                cli.set_write_timeout(15, 0);

                while (true) {
                    size_t index = current_index.fetch_add(1, std::memory_order_relaxed);
                    if (index >= total_files) break;

                    if (pull_csv_file_general(cli, csv_names[index], target_path) != 0) {
                        fail_count.fetch_add(1, std::memory_order_relaxed);
                    }
                }
                });
        }

        for (auto& t : worker_threads) {
            if (t.joinable()) t.join();
        }

        core::log_it("Multi-threaded pull finished. Total: " + std::to_string(total_files) +
            ", Failed: " + std::to_string(fail_count.load()));
    }

} // namespace net