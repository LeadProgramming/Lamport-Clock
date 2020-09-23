#include "Process.h"
using namespace PROC;
Process::Process()
{
    this->proc_id = 0;
    this->configuration = {};
}
Process::Process(const int id)
{
    this->proc_id = id;
}
void Process::appendProc(const std::string &elem)
{
    this->configuration.push_back(elem);
}
void Process::showProc()
{
    for (auto &i : this->configuration)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}