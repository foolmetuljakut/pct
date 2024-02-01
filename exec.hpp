#ifndef exec_cpp
#define exec_cpp

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <utility>

namespace CPI {
    std::tuple<std::string, int> exec(std::string cmd);
};

#endif