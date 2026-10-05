#include <string>

namespace io_medi {
	std::string buf_split_line(const std::string& buf_line, int wanted_i, char delimiter); //遇到\n或者\r\n退出,按照delimiter分割buf_line,返回分割后的第一段,buf_line会被修改为剩余部分,wabted_i表示想要的第几段,从0开始计数,如果没有找到返回空字符串

}
