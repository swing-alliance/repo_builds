#include "core/apptime.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <sstream>

namespace core {

    namespace {

        std::atomic<long long> g_offset_seconds{ 0 };

        // 按本机时区解释 tm → epoch
        std::time_t tm_to_time_t(std::tm* tm_buf) {
            return std::mktime(tm_buf);   // 本地（中国）时区
        }

    } // namespace

    std::time_t getnow() {
        std::time_t base = std::time(nullptr);
        long long offset = g_offset_seconds.load(std::memory_order_relaxed);
        return base + static_cast<std::time_t>(offset);
    }

    std::string getnow_string(const char* fmt) {
        return strf_time(getnow(), fmt);
    }

    void app_stop(int t) {
        if (t > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(t));
        }
    }

    void usetime_offset(long long seconds) {
        g_offset_seconds.fetch_add(seconds, std::memory_order_relaxed);
    }

    long long get_time_offset() {
        return g_offset_seconds.load(std::memory_order_relaxed);
    }

    void clear_time_offset() {
        g_offset_seconds.store(0, std::memory_order_relaxed);
    }

    std::string strf_time(std::time_t t, const char* fmt) {
        std::tm tm_buf{};
#ifdef _WIN32
        localtime_s(&tm_buf, &t);
#else
        localtime_r(&t, &tm_buf);
#endif
        char buf[128] = { 0 };
        std::strftime(buf, sizeof(buf), fmt, &tm_buf);
        return std::string(buf);
    }

    std::time_t intf_time(const std::string& s) {
        const char* fmt_full = "%Y-%m-%d %H:%M:%S";
        const char* fmt_date = "%Y-%m-%d";

        std::tm tm_buf{};
        std::istringstream ss(s);

        ss >> std::get_time(&tm_buf, fmt_full);
        if (ss.fail()) {
            ss.clear();
            ss.str(s);
            ss >> std::get_time(&tm_buf, fmt_date);
            if (ss.fail()) {
                return static_cast<std::time_t>(-1);
            }
            tm_buf.tm_hour = 0;
            tm_buf.tm_min = 0;
            tm_buf.tm_sec = 0;
        }

        tm_buf.tm_isdst = -1;
        return tm_to_time_t(&tm_buf);   // 现在是 mktime，本地时区
    }

} // namespace core