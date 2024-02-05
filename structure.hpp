#ifndef cpistructure_hpp
#define cpistructure_hpp

#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <exception>
#include <filesystem>
#include <algorithm>
#include <array>
#include <functional>

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
        bool haschanged(int unittestnr);
        bool isheader() const;
        bool issource() const;
        File();
        File(std::string name);
        File(std::string name, std::string hash);
        ptree tonode();
    };

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
        size_t vMajor, vMinor, vPatch, vBuild;
        Project();
        Project(ptree& node);
        Project(std::string filename);
        Project(TargetSpec& spec, std::initializer_list<File> files);
        Project(TargetSpec& spec, std::vector<File>& files);
        void addfile(std::string filename);
        bool haschanged();
        std::string compilecmd(const File& file, int unittestnr = 0);
        bool compile(bool force, int unittestnr = 0);
        std::string linkcmd(int unittestnr = 0);
        bool link(int unittestnr = 0);
        bool build(bool force);
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
        Solution(std::string solutionfilename, std::string name, 
            std::string compilerflags, std::string linkerflags, 
            std::initializer_list<std::string>& files);
        Solution(std::string solutionfilename, std::string name, 
            std::string compilerflags, std::string linkerflags, 
            std::vector<std::string>& files);
        void addsubproject(Project &project);
        bool haschanged();
        ptree tonode();
        void save();
        void load();
        bool build(bool force);

    };
};

#endif // cpistructure_hpp