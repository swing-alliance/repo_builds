#include "core/config.h"
#include <fstream>
#include <mutex>
#include <core/paths.h>

namespace core {
    static std::mutex config_file_mutex;
    static nlohmann::json load_json_locked() {
        std::ifstream file(core::config_path);
        if (!file.is_open()) {
            return nlohmann::json::object();
        }
        try {
            nlohmann::json j;
            file >> j;
            if (j.is_object()) {
                return j;
            }
        }
        catch (...) {}
        return nlohmann::json::object();
    }

    // 安全保存整个 JSON 文件（带缩进）
    static int save_json_locked(const nlohmann::json& j) {
        std::ofstream file(core::config_path);
        if (!file.is_open()) {
            return -1;
        }
        try {
            file << j.dump(4);
            return 0;
        }
        catch (...) {
            return -1;
        }
    }

    int internal_json_get(const std::string& key, nlohmann::json& out_val) {
        std::lock_guard<std::mutex> lock(config_file_mutex);
        nlohmann::json j = load_json_locked();
        if (j.contains(key)) {
            out_val = j[key];
            return 0;
        }
        return -1;
    }

    // 底层实现：加锁并安全写入 JSON 字段
    int internal_json_set(const std::string& key, const nlohmann::json& val, bool create_if_not_exists) {
        std::lock_guard<std::mutex> lock(config_file_mutex);
        nlohmann::json j = load_json_locked();
        if (!create_if_not_exists && !j.contains(key)) {
            return -1; // 不允许新建且键不存在
        }
        j[key] = val;
        return save_json_locked(j);
    }


    void init_default_config() {
        mk_conf_field<std::string>("rest_server_host", "35.212.253.0");
		mk_conf_field<int>("rest_server_port", 8880);
    }

	void clr_config() {
		std::lock_guard<std::mutex> lock(config_file_mutex);
		nlohmann::json j = nlohmann::json::object();
		save_json_locked(j);
	}

} // namespace core