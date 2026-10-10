#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <charconv> // 引入超高速数值解析库
#include <cstring>  // 引入 std::memcpy
#include "model/fund_model.h"
#include "model/model_interpreter.h"

namespace model_int {
	// 能吊打 pandas 的针对基金的 CSV 解析器，专门针对基金净值数据格式优化，支持 UTF-8 BOM 自动跳过，支持表头自动跳过，支持缺失值处理，支持高性能数值解析。
    bool mem_to_obj_fund(model::fund_model* target, const std::string* buf) {
        if (!buf || buf->empty()) {
            return false;
        }
        const char* p = buf->data();
        const char* end = p + buf->size();
        if (buf->size() >= 3 &&
            (unsigned char)p[0] == 0xEF &&
            (unsigned char)p[1] == 0xBB &&
            (unsigned char)p[2] == 0xBF) {
            p += 3;
        }
        std::vector<model::fund_row_tm_val> alotdata;
        alotdata.reserve(buf->size() / 30); // 稍微调大每行预估字节数（因为多了一列数据）
        bool is_first_line = true; // 用于标记并跳过表头
        while (p < end) {
            const char* line_start = p;
            while (p < end && *p != '\n' && *p != '\r') {
                p++;
            }
            if (p > line_start) {
                std::string_view line(line_start, p - line_start);
                if (is_first_line) {
                    is_first_line = false;
                    while (p < end && (*p == '\n' || *p == '\r')) { p++; }
                    continue;
                }
                size_t first_comma = line.find(',');
                if (first_comma != std::string_view::npos) {
                    size_t second_comma = line.find(',', first_comma + 1);
                    model::fund_row_tm_val row;
                    std::string_view time_view = line.substr(0, first_comma);
                    size_t copy_len = std::min(time_view.size(), sizeof(row.time) - 1);
                    std::memcpy(row.time, time_view.data(), copy_len);
                    row.time[copy_len] = '\0';
                    std::string_view val_str;
                    std::string_view ret_str;
                    if (second_comma != std::string_view::npos) {
                        val_str = line.substr(first_comma + 1, second_comma - first_comma - 1);
                        ret_str = line.substr(second_comma + 1);
                    }
                    else {
                        val_str = line.substr(first_comma + 1);
                    }
                    if (val_str.empty() || val_str == "0") {
                        row.accum_val = 0.0f;
                    }
                    else {
                        float parsed_val = 0.0f;
                        auto [ptr, ec] = std::from_chars(val_str.data(), val_str.data() + val_str.size(), parsed_val);
                        row.accum_val = (ec == std::errc()) ? parsed_val : 0.0f;
                    }
                    if (ret_str.empty() || ret_str == "0") {
                        row.daily_return = 0.0f;
                    }
                    else {
                        float parsed_ret = 0.0f;
                        auto [ptr, ec] = std::from_chars(ret_str.data(), ret_str.data() + ret_str.size(), parsed_ret);
                        row.daily_return = (ec == std::errc()) ? parsed_ret : 0.0f;
                    }
                    alotdata.push_back(row);
                }
            }
            while (p < end && (*p == '\n' || *p == '\r')) {
                p++;
            }
        }
        alotdata.shrink_to_fit();
        target->data = std::move(alotdata);
        return true;
    }
}