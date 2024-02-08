#include "targetspec.hpp"

Pmt::TargetSpec::TargetSpec() {}

Pmt::TargetSpec::TargetSpec(std::string name, std::string opts, std::string lflags) 
    : name(name), opts(opts), lflags(lflags) {}

bool Pmt::TargetSpec::haschanged() {
    return false;
}

ptree Pmt::TargetSpec::tonode() {
    ptree node;
    node.put("name", this->name);
    node.put("compilerflags", this->opts);
    node.put("linkerflags", this->lflags);
    return node;
}