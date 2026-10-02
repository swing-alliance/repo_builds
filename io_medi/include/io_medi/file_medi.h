#include<string>
namespace io_medi {
	extern std::string readFileToBuffer(std::string path); //读取绝对路径下的文件到缓冲区
	extern void writeBufferToFile(const std::string& path, const std::string& buffer); //将缓冲区写入绝对路径下的文件,覆盖原有文件

}

