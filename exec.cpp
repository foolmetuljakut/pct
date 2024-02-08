#include "exec.hpp"

std::tuple<std::string, int> Pmt::exec(std::string cmd) {
    int exitcode = 0;
    std::array<char, 128> buffer;
    std::string result;

    FILE *pipe = popen(cmd.c_str(), "r");
    if (pipe == nullptr) {
        throw std::runtime_error("popen() failed!");
    }
    try {
        std::size_t bytesread;
        while ((bytesread = std::fread(buffer.data(), sizeof(buffer.at(0)), sizeof(buffer), pipe)) != 0) {
            result += std::string(buffer.data(), bytesread);
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    
    exitcode = WEXITSTATUS(pclose(pipe));
    return std::make_tuple(result, exitcode);
}
