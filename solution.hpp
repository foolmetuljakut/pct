#ifndef pmt_solution_hpp 
#define pmt_solution_hpp

#include "project.hpp"

namespace Pmt {
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

#endif //pmt_solution_hpp 