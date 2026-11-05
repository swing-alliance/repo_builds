#include "core/paths.h"
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#endif

namespace core {

    namespace {

        // 内部函数：获取 exe 的绝对路径字符串（统一转换为 UTF-8）
        std::string get_exe_path_str() {
#ifdef _WIN32
            wchar_t buf[MAX_PATH] = { 0 };
            DWORD len = GetModuleFileNameW(NULL, buf, MAX_PATH);
            if (len == 0) return "";
            // std::filesystem 自动处理宽字符到 UTF-8 的安全转换
            return std::filesystem::path(buf).string();

#elif defined(__linux__)
            char buf[PATH_MAX] = { 0 };
            ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
            if (len == -1) return "";
            buf[len] = '\0';
            return std::string(buf);

#elif defined(__APPLE__)
            char buf[PATH_MAX] = { 0 };
            uint32_t size = sizeof(buf);
            if (_NSGetExecutablePath(buf, &size) != 0) return "";
            return std::string(buf);

#else
            return "";
#endif
        }

    } // namespace

    // ---------------- 获取 exe 所在目录 ----------------
    std::string get_exe_dir() {
        std::string exe_path = get_exe_path_str();
        if (exe_path.empty()) return "";
        return std::filesystem::path(exe_path).parent_path().string();
    }

    // ---------------- 向上查找项目根 ----------------
    std::string get_project_root(const std::string& root_name) {
        std::filesystem::path dir(get_exe_dir());
        if (dir.empty()) return "";

        std::filesystem::path current = dir;
        while (!current.empty()) {
            if (current.filename().string() == root_name) {
                return current.string();
            }
            std::filesystem::path parent = current.parent_path();
            if (parent == current) break;   // 已到根目录
            current = parent;
        }
        return "";
    }

    // ---------------- 全局路径定义 ----------------
    // 注意：如果你的项目根文件夹名字不是 "fund_qt"，请修改此处传入的名称
    std::string pj_root_dir = get_project_root("fund_qt");
    std::string datasource = std::filesystem::path(pj_root_dir).empty() ? "" : (std::filesystem::path(pj_root_dir) / "datasource").string();
    std::string funds_row_dir = datasource.empty() ? "" : (std::filesystem::path(datasource) / "funds_row").string();
    std::string app_db_path = datasource.empty() ? "" : (std::filesystem::path(datasource) / "app.db").string();
    std::string log_path = datasource.empty() ? "" : (std::filesystem::path(datasource) / "app.log").string();
    std::string config_path = datasource.empty() ? "" : (std::filesystem::path(datasource) / "config.json").string();

} // namespace core