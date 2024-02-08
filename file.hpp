#ifndef pmt_file_hpp 
#define pmt_file_hpp

#include "std.hpp"
#include "boost.hpp"
#include "exec.hpp"
#include "pmtexception.hpp"
        
namespace Pmt {
    class File {
    public:
        std::string name;
        std::string hash;
        bool haschanged(int unittestnr);
        bool isheader() const;
        bool issource() const;
        File();
        File(std::string name);
        File(std::string name, std::string hash);
        ptree tonode();
    };
};

#endif //pmt_file_hpp 