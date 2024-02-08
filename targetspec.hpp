#ifndef pmt_targetspec_hpp
#define pmt_targetspec_hpp

#include <string>
#include "boost.hpp"

namespace Pmt {
    class TargetSpec {
    public:
        std::string name;
        std::string opts;
        std::string lflags;
        TargetSpec();
        TargetSpec(std::string name, std::string opts, std::string lflags);
        /*
            target name
            other target config
        */
        bool haschanged();
        ptree tonode();
    };
};

#endif //pmt_targetspec_hpp