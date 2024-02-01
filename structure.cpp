#include "structure.hpp"

CPI::File::File() {}

CPI::File::File(std::string name) : name(name) {
    std::stringstream cmd;
    cmd << "md5sum " << this->name;
    hash = std::get<0>(exec(cmd.str().c_str())).substr(0, 32);
}

CPI::File::File(std::string name, std::string hash) : name(name), hash(hash) { }

bool CPI::File::haschanged() {
    if(name.empty())
        throw CPIException({"file has invalid name (empty)"});

    // add check: existence of corresponding object file

    std::stringstream cmd;
    cmd << "md5sum " << name;
    std::string newhash = std::get<0>(exec(cmd.str().c_str())).substr(0, 32);

    std::stringstream ofile;
    ofile << "build/" << name << ".o";
    return hash.compare(newhash) || !std::filesystem::exists(ofile.str());
}

ptree CPI::File::tonode() {
    ptree node;
    node.put("name", this->name);
    node.put("hash", this->hash);
    return node;
}

CPI::TargetSpec::TargetSpec() {}

CPI::TargetSpec::TargetSpec(std::string name, std::string opts) : name(name), opts(opts) {}

bool CPI::TargetSpec::haschanged() {
    return false;
}

ptree CPI::TargetSpec::tonode() {
    ptree node;
    node.put("name", this->name);
    node.put("opts", this->opts);
    return node;
}

CPI::Project::Project() : vMajor{0}, vMinor{0}, vPatch{0}, vBuild{0} {}

CPI::Project::Project(ptree& node) : vMajor{0}, vMinor{0}, vPatch{0}, vBuild{0} {
    fromnode(node);
}

CPI::Project::Project(std::string filename) {
    load(filename);
}

CPI::Project::Project(TargetSpec& spec, std::initializer_list<File> files) : spec(spec) {
    for( auto& file : files)
        this->files.push_back(file);
    vMajor = 0;
    vMinor = 0;
    vPatch = 0;
    vBuild = 0;
}

CPI::Project::Project(TargetSpec& spec, std::vector<File>& files) {
    for( auto& file : files)
        this->files.push_back(file);
    vMajor = 0;
    vMinor = 0;
    vPatch = 0;
    vBuild = 0;
}

void CPI::Project::addfile(std::string filename) {
    for(auto& file : files)
        if(! file.name.compare(filename))
            return;
    files.push_back(File(filename, "00000000000000000000000000000000"));
}

bool CPI::Project::haschanged() {
    bool did = spec.haschanged() || !std::filesystem::exists(spec.name);
    for(auto& file : files)
        did |= file.haschanged();
        if(did)
            return true;
    return false;
}

std::string CPI::Project::compilecmd(std::string filename, int unittestnr) {

    std::stringstream ofile;
    ofile << "build/" << filename << ".o";

    std::filesystem::path path(ofile.str());
    auto parentdir = path.parent_path();
    if(!std::filesystem::exists(parentdir))
        std::filesystem::create_directories(parentdir);

    std::stringstream s;
    s << "g++ -c " << filename << " -o " << ofile.str() << " ";
    s << spec.opts;

    if(unittestnr > 0) {
        s << " -D" << unittestsymbol << unittestnr;
    }
    else {
        s << " -D" << "MAIN";
    }
    return s.str();
}

bool CPI::Project::compile(bool force, int unittestnr)
{
    for(auto& file : files) {
        // if header => skip
        if(file.name.find(".h") != std::string::npos || file.name.find(".hpp") != std::string::npos || file.name.find(".hxx") != std::string::npos)
            continue;

        if(!force)
            if(!file.haschanged())
                continue;

        auto s = compilecmd(file.name, unittestnr);
        if(unittestsymbol.size() > 0)
            std::cout << "[compiling \033[92m" << spec.name << "-ut" << unittestnr << "\033[37m]:\n" << s << std::endl;
        else
            std::cout << "[compiling \033[92m" << spec.name << "\033[37m]:\n" << s << std::endl;
        
        // <1> = exit code, if(exitcode) => caught error
        if(std::get<1>(exec(s))) {
            return false;
        }

        std::stringstream hashing;
        hashing << "md5sum " << file.name;
        file.hash = std::get<0>(exec(hashing.str().c_str())).substr(0, 32);
    }
    return true;
}

std::string CPI::Project::linkcmd(int unittestnr)
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
        // if header
        if(file.name.find(".h") != std::string::npos || file.name.find(".hpp") != std::string::npos || file.name.find(".hxx") != std::string::npos)
            s << file.name << " ";
        else // or source
            s << "build/" << file.name << ".o ";
    }
    s << spec.opts;

    if(unittestnr > 0) {
        s << " -D" << unittestsymbol << unittestnr;
    }
    else {
        s << " -D" << "MAIN";
    }

    return s.str();
}

bool CPI::Project::link(int unittestnr)
{
    std::string s = linkcmd(unittestnr);
    if(unittestsymbol.size() > 0)
        std::cout << "[linking \033[92m" << spec.name << "-ut" << unittestnr << "\033[37m]:\n" << s << std::endl;
    else
        std::cout << "[linking \033[92m" << spec.name << "\033[37m]:\n" << s << std::endl;
    
    return !std::get<1>(exec(s));
}

bool CPI::Project::build(bool force) {
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

ptree CPI::Project::tonode() {
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

void CPI::Project::fromnode(ptree& node) {

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
                    break;
                case 1:
                    vMinor = n;
                    break;
                case 2:
                    vPatch = n;
                    break;
                default:
                    throw CPIException({"what the fuck is happening with your version?! ", node.get<std::string>("version")});
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
                opts = node.get<std::string>("target.opts");
    spec = TargetSpec(name, opts);
    for( auto& file : node.get_child("files")) {
        std::string name = file.second.get<std::string>("name"),
                    hash = file.second.get<std::string>("hash");
        files.push_back(File(name, hash));
    }
}

void CPI::Project::save(std::string filename) {
    std::ofstream fout(filename);
    write_json(fout, tonode());
}

void CPI::Project::load(std::string filename) {
    std::ifstream fin(filename);
    ptree node;
    read_json(fin, node);
    fromnode(node);
}

CPI::Solution::Solution(std::string solutionfilename) :
    solutionfilename(solutionfilename) {}

CPI::Solution::Solution() {}

CPI::Solution::Solution(std::string solutionfilename, Project main)  :
    solutionfilename(solutionfilename), mainapp(main) {
    save();
}

CPI::Solution::Solution(std::string solutionfilename, 
    std::string name, std::string opts, 
    std::initializer_list<std::string>& files) :
    solutionfilename(solutionfilename) {
    TargetSpec target(name, opts);
    std::vector<File> fs;
    std::transform(files.begin(), files.end(), fs.begin(),
        [](std::string file) {
            return File(file, "00000000000000000000000000000000");
        });
    mainapp = Project(target, fs);
}

CPI::Solution::Solution(std::string solutionfilename, 
    std::string name, std::string opts, 
    std::vector<std::string>& files) :
    solutionfilename(solutionfilename)  {
    TargetSpec target(name, opts);
    std::vector<File> fs;
    std::transform(files.begin(), files.end(), fs.begin(),
        [](std::string file) {
            return File(file, "00000000000000000000000000000000");
        });
    mainapp = Project(target, fs);
}

void CPI::Solution::addsubproject(Project &project) {
    projects.push_back(project);
}

bool CPI::Solution::haschanged() {
    bool did = mainapp.haschanged();
    for(auto& p : projects)
        did |= p.haschanged();
    return did;
}

ptree CPI::Solution::tonode() {
    ptree node;
    node.put_child("mainapp", mainapp.tonode());
    /*no i've tried, adding a list DOESN'T get simpler*/
    ptree projectnode;
    if(projects.size() > 0) {
        for(auto& project : projects)
            projectnode.push_back(ptree::value_type("", project.tonode()));
        node.add_child("projects", projectnode);        
    }
    return node;
}

void CPI::Solution::save() {
    std::ofstream fout(solutionfilename);
    write_json(fout, tonode());
}

void CPI::Solution::load() {
    std::ifstream fin(solutionfilename);
    ptree node;
    read_json(fin, node);
    
    ptree mainappnode = node.get_child("mainapp");
    mainapp = Project(mainappnode);
    
    if(node.count("projects")) {
        for( auto& projectnode : node.get_child("projects")) {
            projects.push_back(Project(projectnode.second));
        }
    }
}

void CPI::Solution::compile(bool force) {
    for(auto& p : projects)
        if(p.haschanged() || force) {
            bool result = p.build(force);
            std::ofstream fout(solutionfilename);
            write_json(fout, tonode());
            if(!result)
                return;
        }
    if(mainapp.haschanged() || force) {
        bool result = mainapp.build(force);
        std::ofstream fout(solutionfilename);
        write_json(fout, tonode());
            if(!result)
                return;
    }
}







