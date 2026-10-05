#pragma once

#include <string>
#include <functional>

namespace io_medi {
    void db_exec_stream(const std::string& sql, std::function<void(const std::string&, int)> on_row);
    
}