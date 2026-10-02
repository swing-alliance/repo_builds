#include <string>
#include <mutex>
#include<cstdio>
#include"core/paths.h"
#include"core/apptime.h"
namespace core {
	static std::mutex log_mutex;
	void log_it(std::string log_msg)
	{
		std::string buf = "系统时间:" + core::getnow_string() + "|" + log_msg + "\n";
		std::lock_guard<std::mutex> lock(log_mutex);
		FILE * fd = fopen(core::log_path.c_str(), "a");
		if (fd == NULL) { perror("Failed to open log file");return;}
		int ret = fwrite(buf.c_str(), 1, buf.size(), fd);
		if (ret != buf.size()) {
			perror("Failed to write to log file");
		}
		fclose(fd);
		return;
	}



}
