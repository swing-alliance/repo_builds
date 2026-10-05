#include <string>
#include <sstream>
namespace io_medi {
    std::string buf_split_line(const std::string& buf_line, int wanted_i, char delimiter) {
        std::stringstream ss(buf_line);
        std::string item;
        int current_i = 0;
        while (std::getline(ss, item, delimiter)) {
            if (current_i == wanted_i) {
                return item;
            }
            current_i++;
        }
        return "";
    }
}
