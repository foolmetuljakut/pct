#ifndef pmtexception_hpp
#define pmtexception_hpp

#include <string>
#include <sstream>

namespace Pmt {

class PmtException : public std::exception {
public:
    std::string w;
    PmtException(std::initializer_list<std::string> l);
    const char * what() const noexcept override { return w.c_str(); }
};

};

#endif //pmtexception_hpp
