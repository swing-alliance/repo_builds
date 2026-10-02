#include "core/paths.h"

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

// 判断是否是路径分隔符
bool is_sep(char c) {
#ifdef _WIN32
    return c == '\\' || c == '/';
#else
    return c == '/';
#endif
}

// 取父目录
std::string parent_dir(const std::string& p) {
    if (p.empty()) return "";

    // 去掉末尾的分隔符
    std::string s = p;
    while (s.size() > 1 && is_sep(s.back())) {
        s.pop_back();
    }

    // 找最后一个分隔符
    size_t pos = s.find_last_of("/\\");
    if (pos == std::string::npos) return "";
    if (pos == 0) return s.substr(0, 1);   // 根目录，比如 "/"

#ifdef _WIN32
    // Windows 下 "C:" 这种盘符要保留
    if (pos == 2 && s[1] == ':') {
        return s.substr(0, 3);
    }
#endif

    return s.substr(0, pos);
}

// 取文件名（最后一段）
std::string file_name(const std::string& p) {
    if (p.empty()) return "";
    std::string s = p;
    while (s.size() > 1 && is_sep(s.back())) {
        s.pop_back();
    }
    size_t pos = s.find_last_of("/\\");
    if (pos == std::string::npos) return s;
    return s.substr(pos + 1);
}

// 拼接路径
std::string join(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    if (is_sep(a.back())) return a + b;
#ifdef _WIN32
    return a + "\\" + b;
#else
    return a + "/" + b;
#endif
}

// 判断是否是绝对路径
bool is_absolute(const std::string& p) {
    if (p.empty()) return false;
#ifdef _WIN32
    // "C:\..." 或 "\\server\share"
    if (p.size() >= 2 && p[1] == ':') return true;
    if (p.size() >= 2 && is_sep(p[0]) && is_sep(p[1])) return true;
    return false;
#else
    return p[0] == '/';
#endif
}

} // namespace

// ---------------- 获取 exe 所在目录 ----------------
std::string get_exe_dir() {
#ifdef _WIN32
    char buf[MAX_PATH] = {0};
    DWORD len = GetModuleFileNameA(NULL, buf, MAX_PATH);
    if (len == 0) return "";
    return parent_dir(std::string(buf, len));

#elif defined(__linux__)
    char buf[PATH_MAX] = {0};
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len == -1) return "";
    buf[len] = '\0';
    return parent_dir(std::string(buf));

#elif defined(__APPLE__)
    char buf[PATH_MAX] = {0};
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) != 0) return "";
    return parent_dir(std::string(buf));

#else
    return "";
#endif
}

// ---------------- 向上查找项目根 ----------------
std::string get_project_root(const std::string& root_name) {
    std::string dir = get_exe_dir();
    if (dir.empty()) return "";

    while (!dir.empty()) {
        if (file_name(dir) == root_name) {
            return dir;
        }
        std::string parent = parent_dir(dir);
        if (parent == dir) break;   // 已到根目录
        dir = parent;
    }
    return "";
}

// ---------------- 全局路径定义 ----------------
std::string pj_root_dir   = get_project_root();
std::string datasource    = join(pj_root_dir, "datasource");
std::string funds_row_dir = join(datasource, "funds_row");
std::string app_db        = join(datasource, "app.db");
std::string log_path =      join(datasource, "app.log");
std::string config_path =    join(datasource, "config.json");

} // namespace core