#include "pmtexception.hpp"

namespace Pmt {

PmtException::PmtException(std::initializer_list<std::string> l) {
    std::stringstream ss;
    for(auto &s : l)
        ss << s;
    w = ss.str();
}

};