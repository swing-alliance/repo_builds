#pragma once

#include <string>
#include <type_traits>
#include <nlohmann/json.hpp>

namespace core {

    // --- 底层非模板接口声明（实现在 config.cpp 中） ---
    int internal_json_get(const std::string& key, nlohmann::json& out_val);
    int internal_json_set(const std::string& key, const nlohmann::json& val, bool create_if_not_exists);
	void init_default_config(); // 初始化默认配置
	void clr_config(); // 清空配置文件
    // --- 内部辅助：校验 JSON 中的值是否与模板类型 T 匹配 ---
    template<typename T>
    inline bool is_json_type_match(const nlohmann::json& j) {
        if constexpr (std::is_same_v<T, std::string>) {
            return j.is_string();
        }
        else if constexpr (std::is_arithmetic_v<T>) {
            if constexpr (std::is_same_v<T, bool>) {
                return j.is_boolean();
            }
            else if constexpr (std::is_floating_point_v<T>) {
                return j.is_number_float() || j.is_number_integer();
            }
            else {
                return j.is_number_integer();
            }
        }
        return false;
    }

    /**
     * @brief 创建配置字段
     * @return 0 成功创建，1 字段已存在且类型正确，-1 失败或类型冲突
     */
    template<typename T>
    int mk_conf_field(const std::string& field_name, T value) {
        nlohmann::json j_val;
        // 调用底层接口获取原始 json 节点
        if (internal_json_get(field_name, j_val) == 0) {
            // 字段已存在，检查类型是否匹配
            if (is_json_type_match<T>(j_val)) {
                return 1; // 存在且类型正确
            }
            return -1; // 存在但类型不匹配冲突
        }
        // 不存在，创建并写入
        int ret = internal_json_set(field_name, value, true);
        return (ret == 0) ? 0 : -1;
    }

    /**
     * @brief 获取配置字段（直接返回类型 T）
     * @param default_value 如果字段不存在或类型错误时返回的默认值
     */
    template<typename T>
    T get_conf(const std::string& field_name, const T& default_value = T()) {
        nlohmann::json j_val;
        if (internal_json_get(field_name, j_val) != 0) {
            return default_value;
        }
        if (!is_json_type_match<T>(j_val)) {
            return default_value;
        }
        try {
            return j_val.get<T>();
        }
        catch (...) {
            return default_value;
        }
    }

    /**
     * @brief 修改配置字段
     * @return 0 成功，-1 失败（字段不存在或类型不正确）
     */
    template<typename T>
    int mod_conf(const std::string& field_name, T value) {
        nlohmann::json j_val;
        if (internal_json_get(field_name, j_val) != 0) {
            return -1; // 字段不存在
        }
        if (!is_json_type_match<T>(j_val)) {
            return -1; // 类型不正确，拒绝修改
        }
        int ret = internal_json_set(field_name, value, false);
        return (ret == 0) ? 0 : -1;
    }

   

} // namespace core