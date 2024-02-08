#include "solution.hpp"
Pmt::Solution::Solution(std::string solutionfilename) :
    solutionfilename(solutionfilename) {}

Pmt::Solution::Solution() {}

Pmt::Solution::Solution(std::string solutionfilename, Project main)  :
    solutionfilename(solutionfilename), mainapp(main) {
    save();
}

Pmt::Solution::Solution(std::string solutionfilename, 
    std::string name, std::string compilerflags, std::string linkerflags, 
    std::initializer_list<std::string>& files) :
    solutionfilename(solutionfilename) {
    TargetSpec target(name, compilerflags, linkerflags);
    std::vector<File> fs;
    std::transform(files.begin(), files.end(), fs.begin(),
        [](std::string file) {
            return File(file, "00000000000000000000000000000000");
        });
    mainapp = Project(target, fs);
}

Pmt::Solution::Solution(std::string solutionfilename, 
    std::string name, std::string compilerflags, std::string linkerflags, 
    std::vector<std::string>& files) :
    solutionfilename(solutionfilename)  {
    TargetSpec target(name, compilerflags, linkerflags);
    std::vector<File> fs;
    std::transform(files.begin(), files.end(), fs.begin(),
        [](std::string file) {
            return File(file, "00000000000000000000000000000000");
        });
    mainapp = Project(target, fs);
}

void Pmt::Solution::addsubproject(Project &project) {
    projects.push_back(project);
}

bool Pmt::Solution::haschanged() {
    bool did = mainapp.haschanged();
    for(auto& p : projects)
        did |= p.haschanged();
    return did;
}

ptree Pmt::Solution::tonode() {
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

void Pmt::Solution::save() {
    std::ofstream fout(solutionfilename);
    write_json(fout, tonode());
}

void Pmt::Solution::load() {
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

bool Pmt::Solution::build(bool force) {
    for(auto& p : projects)
        if(p.haschanged() || force) {
            bool result = p.build(force);
            std::ofstream fout(solutionfilename);
            write_json(fout, tonode());
            if(!result)
                return false;
        }
    if(mainapp.haschanged() || force) {
        bool result = mainapp.build(force);
        std::ofstream fout(solutionfilename);
        write_json(fout, tonode());
            if(!result)
                return false;
    }
    return true;
}
