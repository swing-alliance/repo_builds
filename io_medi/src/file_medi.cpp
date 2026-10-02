#include <string>
#include <cstdio>
#include <cstring>
#include <io_medi/file_medi.h>

namespace io_medi {


    //更快
    std::string readFileToBuffer(std::string path) {
        FILE* fp = fopen(path.c_str(), "rb");
        if (!fp) return {};
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if (size <= 0) {
            fclose(fp);
            return {};
        }
        std::string buf;
        buf.resize(size);
        size_t read_bytes = fread(&buf[0], 1, size, fp);
        fclose(fp);
        if (read_bytes != size) {
            buf.resize(read_bytes); // 适配某些系统（如 Windows 换行符转换）的实际读取量
        }
        return buf;
    }

	void writeBufferToFile(const std::string& path, const std::string& buffer) {
		FILE* fp = fopen(path.c_str(), "wb");
		if (!fp) return;
		fwrite(buffer.data(), 1, buffer.size(), fp);
		fclose(fp);
	}

} // namespace io_medi