#include "file.hpp"

Pmt::File::File() {}

Pmt::File::File(std::string name) : name(name) {
    std::stringstream cmd;
    cmd << "md5sum " << this->name;
    hash = std::get<0>(exec(cmd.str().c_str())).substr(0, 32);
}

Pmt::File::File(std::string name, std::string hash) : name(name), hash(hash) { }

bool Pmt::File::haschanged(int unittestnr) {
    if(name.empty())
        throw PmtException({"file has invalid name (empty)"});

    // add check: existence of corresponding object file

    std::stringstream cmd;
    cmd << "md5sum " << name;
    std::string newhash = std::get<0>(exec(cmd.str().c_str())).substr(0, 32);

    std::stringstream ofile;
    if(issource()) {
        ofile << "build/" << name << "." << unittestnr << ".o";
    }
    else {
        ofile << name << ".gch";
    }
    return hash.compare(newhash) || !std::filesystem::exists(ofile.str());
}

bool Pmt::File::isheader() const
{
    return name.find(".h") != std::string::npos || 
            name.find(".hpp") != std::string::npos || 
            name.find(".hxx") != std::string::npos;
}

bool Pmt::File::issource() const
{
    return !isheader();
}

ptree Pmt::File::tonode() {
    ptree node;
    node.put("name", this->name);
    node.put("hash", this->hash);
    return node;
}