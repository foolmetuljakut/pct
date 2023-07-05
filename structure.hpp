#ifndef cpistructure_hpp
#define cpistructure_hpp

#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <exception>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/array.hpp>
using boost::property_tree::ptree,
        boost::property_tree::write_json,
        boost::property_tree::read_json;

#include "exec.hpp"

namespace CPI {

    class CPIException : public std::exception {
    public:
        std::string w;
        CPIException(std::initializer_list<std::string> l) {
            std::stringstream ss;
            for(auto &s : l)
                ss << s;
            w = ss.str();
        }
        const char * what() const noexcept override { return w.c_str(); }
    };

    class File {
    public:
        std::string name;
        std::string hash;
        bool haschanged();
        File();
        File(std::string name);
        File(std::string name, std::string hash);
        ptree tonode();
    };

    class TargetSpec {
    public:
        std::string name;
        std::string opts;
        TargetSpec();
        TargetSpec(std::string name, std::string opts);
        /*
            target name
            other target config
        */
        bool haschanged();
        ptree tonode();
    };

    class Project {
        /*
            target spec
            List of Files
            List of dependent Projects
            List of libraries to link
            additional flags for compilation
        */
    public:
        TargetSpec spec;
        std::vector<File> files;
        std::string unittestsymbol;
        int unittestlistmax;
        Project();
        Project(ptree& node);
        Project(std::string filename);
        Project(TargetSpec& spec, std::initializer_list<File> files);
        Project(TargetSpec& spec, std::vector<File>& files);
        void addfile(std::string filename);
        bool haschanged();
        std::string compilecmd();
        void compile();
        void update();
        void save(std::string filename);
        void load(std::string filename);
        ptree tonode();
        void fromnode(ptree& node);
    };

    class Solution {
        /*
            List of Projects
        
        */
    public:
        std::string solutionfilename;
        Project mainapp;
        std::vector<Project> projects;
        Solution();
        Solution(std::string solutionfilename);
        Solution(std::string solutionfilename, Project main);
        Solution(std::string solutionfilename, std::string name, std::string opts, 
            std::initializer_list<std::string>& files);
        Solution(std::string solutionfilename, std::string name, std::string opts, 
            std::vector<std::string>& files);
        void addsubproject(Project &project);
        bool haschanged();
        void update();
        std::string compilecmd();
        ptree tonode();
        void save();
        void load();
        void compile(bool force);

    };
};

#endif // cpistructure_hpp