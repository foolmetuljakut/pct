#ifndef pmt_project_hpp
#define pmt_project_hpp

#include "file.hpp"
#include "targetspec.hpp"

namespace Pmt {

    class Project {
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

};

#endif //pmt_project_hpp