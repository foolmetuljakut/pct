#include "project.hpp"

Pmt::Project::Project() : vMajor{0}, vMinor{0}, vPatch{0}, vBuild{0} {}

Pmt::Project::Project(ptree& node) : vMajor{0}, vMinor{0}, vPatch{0}, vBuild{0} {
    fromnode(node);
}

Pmt::Project::Project(std::string filename) {
    load(filename);
}

Pmt::Project::Project(TargetSpec& spec, std::initializer_list<File> files) : spec(spec) {
    for( auto& file : files)
        this->files.push_back(file);
    vMajor = 0;
    vMinor = 0;
    vPatch = 0;
    vBuild = 0;
}

Pmt::Project::Project(TargetSpec& spec, std::vector<File>& files) {
    for( auto& file : files)
        this->files.push_back(file);
    vMajor = 0;
    vMinor = 0;
    vPatch = 0;
    vBuild = 0;
}

void Pmt::Project::addfile(std::string filename) {
    for(auto& file : files)
        if(! file.name.compare(filename))
            return;
    files.push_back(File(filename, "00000000000000000000000000000000"));
}

bool Pmt::Project::haschanged() {
    bool did = spec.haschanged() || !std::filesystem::exists(spec.name);
    for(auto& file : files) {
        if (unittestlistmax > 0) {
            for(int unittestnr = 0; unittestnr < unittestlistmax; unittestnr++)
                did |= file.haschanged(unittestnr);
        } else {
            did |= file.haschanged(0);
        }
        if(did)
            return true;
    }
    return false;
}

std::string Pmt::Project::compilecmd(const File& file, int unittestnr) {

    std::stringstream s;

    std::stringstream ofile;
    ofile << "build/" << file.name << "." << unittestnr << ".o";

    std::filesystem::path path(ofile.str());
    auto parentdir = path.parent_path();
    if(!std::filesystem::exists(parentdir))
        std::filesystem::create_directories(parentdir);

    if(file.issource()) {
        s << "g++ -c " << file.name << " -o " << ofile.str();
    } else {
        s << "g++ " << file.name;
    }

    s << " " << spec.opts;

    if(unittestnr > 0) {
        s << " -D" << unittestsymbol << unittestnr;
    }
    else {
        s << " -D" << "MAIN";
    }

    return s.str();
}

bool Pmt::Project::compile(bool force, int unittestnr)
{
    if(unittestnr > 0)
        std::cout << "[compiling \033[92m" << spec.name << "-ut" << unittestnr << "\033[37m]:\n";
    else
        std::cout << "[compiling \033[92m" << spec.name << "\033[37m]:\n";

    auto sortFileList = [this](){
        // this weird construction places headers first in the list,
        // so they get compiled earlier due to the use of PCHs
        std::sort(files.begin(), files.end(), 
            [](const File& lhs, const File& rhs) {
                int lhsPt = lhs.name.size() - lhs.name.rfind('.');
                int rhsPt = rhs.name.size() - rhs.name.rfind('.');
                std::string lhsEnding = lhs.name.substr(lhs.name.size() - lhsPt, lhsPt);
                std::string rhsEnding = rhs.name.substr(rhs.name.size() - rhsPt, rhsPt);
                return -lhsEnding.compare(rhsEnding); 
                // compare returns 1 for cpp / hpp 
                // -1 * compare => -1, sorting hpp before cpp
            });
    };

    sortFileList();

    for(auto& file : files) {
        if(!force)
            if(!file.haschanged(unittestnr))
                continue;

        auto s = compilecmd(file, unittestnr);
        std::cout << s << std::endl;
        
        // <1> = exit code, if(exitcode) => caught error
        if(std::get<1>(exec(s))) {
            sortFileList();
            return false;
        }

        std::stringstream hashing;
        hashing << "md5sum " << file.name;
        file.hash = std::get<0>(exec(hashing.str().c_str())).substr(0, 32);
    }

    sortFileList();

    return true;
}

std::string Pmt::Project::linkcmd(int unittestnr)
{
    std::stringstream s;
    s << "g++ -o ";
    if(unittestnr == 0) { // regular compilation
        s << spec.name << " ";
    }
    else {
        s << spec.name << "-ut" << unittestnr << " ";
    }
    
    for(auto& file : files) {
        if(file.isheader()) {
            s << file.name << " ";
        } else {
            s << "build/" << file.name << "." << unittestnr << ".o ";
        }
    }
    s << spec.opts << " " << spec.lflags;

    if(unittestnr > 0) {
        s << " -D" << unittestsymbol << unittestnr;
    }
    else {
        s << " -D" << "MAIN";
    }

    return s.str();
}

bool Pmt::Project::link(int unittestnr)
{
    std::string s = linkcmd(unittestnr);
    if(unittestnr > 0)
        std::cout << "[linking \033[92m" << spec.name << "-ut" << unittestnr << "\033[37m]:\n" << s << std::endl;
    else
        std::cout << "[linking \033[92m" << spec.name << "\033[37m]:\n" << s << std::endl;
    
    return !std::get<1>(exec(s));
}

bool Pmt::Project::build(bool force) {
    vBuild++;

    int bnd = unittestsymbol.size() > 0 ? unittestlistmax : 0;
    for(size_t i = 0; i <= bnd; i++) {
        if(!compile(force, i))
            return false;
        if(!link(i))
            return false;
    }
    return true;
}

ptree Pmt::Project::tonode() {
    ptree node;
    node.put_child("target", spec.tonode());
    std::stringstream version;
    version << vMajor << "." << vMinor << "." << vPatch << "." << vBuild;
    node.put("version", version.str());
    if(unittestsymbol.size() > 0) {
        node.put("unittestsymbol", unittestsymbol);
        node.put("unittestlistmax", unittestlistmax);
    }
    /*no i've tried, adding a list DOESN'T get simpler*/
    ptree filenode;
    for(auto& file : files)
        filenode.push_back(ptree::value_type("", file.tonode()));
    node.add_child("files", filenode);
    return node;
}

void Pmt::Project::fromnode(ptree& node) {

    if(node.count("unittestsymbol")) {
        unittestsymbol = node.get<std::string>("unittestsymbol");
        unittestlistmax = node.get<int>("unittestlistmax");
    }
    else {
        unittestsymbol = "";
        unittestlistmax = 0;
    }

    if(node.count("version")) {
        std::string version = node.get<std::string>("version");
        std::string token;
        size_t pos = 0, ctr = 0;
        while((pos = version.find(".")) != std::string::npos) {
            token = version.substr(0, pos);
            size_t n = std::stol(token);
            switch(ctr) {
                case 0:
                    vMajor = n;
                    ctr++;
                    break;
                case 1:
                    vMinor = n;
                    ctr++;
                    break;
                case 2:
                    vPatch = n;
                    ctr++;
                    break;
                default:
                    throw PmtException({"what the fuck is happening with your version?! ", node.get<std::string>("version")});
            }
            version.erase(0, pos +1);
        }
        vBuild = std::stol(version); // last segment is not processed by while loop
    } else {
        vMajor = 0;
        vMinor = 0;
        vPatch = 0;
        vBuild = 0;
    }

    std::string name = node.get<std::string>("target.name"),
                compilerflags = node.get<std::string>("target.compilerflags"),
                linkerflags = node.get<std::string>("target.linkerflags");
    spec = TargetSpec(name, compilerflags, linkerflags);
    for( auto& file : node.get_child("files")) {
        std::string name = file.second.get<std::string>("name"),
                    hash = file.second.get<std::string>("hash");
        files.push_back(File(name, hash));
    }
}

void Pmt::Project::save(std::string filename) {
    std::ofstream fout(filename);
    write_json(fout, tonode());
}

void Pmt::Project::load(std::string filename) {
    std::ifstream fin(filename);
    ptree node;
    read_json(fin, node);
    fromnode(node);
}