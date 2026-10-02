#pragma once

#include <ctime>
#include <string>

namespace core {

// ---------------- 取当前时间 ----------------
std::time_t getnow(); // 获取当前时间（秒级 Unix 时间戳，已应用全局偏移）
std::string get_real_now_string(); // 获取当前时间的格式化字符串（不受偏移影响）
std::string getnow_string(const char* fmt = "%Y-%m-%d %H:%M:%S"); // 获取当前时间的格式化字符串

// ---------------- 偏移控制 ----------------
void usetime_offset(long long seconds); // 设置全局时间偏移（秒），正数往后，负数往前
long long get_time_offset(); // 获取当前偏移秒数
void clear_time_offset(); // 清除偏移，恢复系统时间
void app_stop(int t);  //1000ms等于1s

// ---------------- 时间戳 <-> 字符串 计算 ----------------
std::string strf_time(std::time_t t, const char* fmt = "%Y-%m-%d %H:%M:%S"); // 时间戳转字符串
std::time_t intf_time(const std::string& s); // 字符串转时间戳，支持 "YYYY-MM-DD" 或 "YYYY-MM-DD HH:MM:SS"，只有日期补 00:00:00，失败返回 -1



} // namespace core