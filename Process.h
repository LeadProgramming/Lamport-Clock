#ifndef PROCESS_H
#define PROCESS_H
#include <iostream>
#include <vector>
namespace PROC
{
    typedef std::vector<std::string> CfgList;
    class Process
    {
    private:
        int proc_id = 0;
        CfgList configuration;

    public:
        Process();
        Process(const int);
        void appendProc(const std::string &);
        void showProc();
    };

} // namespace PROC
#endif