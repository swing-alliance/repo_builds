#include <core/config.h>
#include <string>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    // 💡 核心修复：设置 Windows 控制台输出编码为 UTF-8，防止中文乱码
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    int rc = core::mk_conf_field<std::string>("test_key", "sadwff");
    std::cout << "rc: " << rc << std::endl;
    std::string rc2 = core::get_conf<std::string>("test_key", "err");
    std::cout << "rc2: " << rc2 << std::endl;
    std::cout << "故意出错看" << std::endl;
    int rc3 = core::mod_conf("test_key", 1);
    std::cout << "rc3: " << rc3 << std::endl;
    int rc4 = core::mod_conf<std::string>("test_key", "new_value");
    std::cout << "rc4: " << rc4 << std::endl;
    int sad = core::mk_conf_field<float>("server_time", 123124.12);
    int rc5 = core::mk_conf_field<std::string>("test_key", "sadwff");
    std::cout << "rc5: " << rc5 << std::endl;
	float rc6 = core::get_conf<float>("server_time", 0.0f);
    std::cout << "rc6: " << rc6 << std::endl;

    return 0;
}