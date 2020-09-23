#include "Process.cpp"
#include <iostream>
typedef std::vector<std::vector<std::string>> Input;
typedef std::vector<PROC::Process> ProcList;
typedef std::vector<std::vector<int>> Calculated;
typedef std::vector<std::vector<std::string>> Verified;

Calculated calculateLC(Input);
Verified verifyLC(Input);
ProcList populateProc(Input);

Calculated calculateLC(Input elem)
{
    Calculated result;
    return result;
}

Verified verifyLC(Input elem)
{
    Verified result;
    return result;
}
ProcList populateProc(Input elemList)
{
    ProcList result;
    for (int i = 0; i < elemList.size(); i++)
    {
        PROC::Process tmp(i + 1);
        for (int j = 0; j < elemList[i].size(); j++)
        {
            tmp.appendProc(elemList[i][j]);
        }
        tmp.showProc();
        result.push_back(tmp);
    }
    return result;
}
int main()
{
    Input sample_1{
        {"a", "s1", "r3", "b"},
        {"c", "r2", "s3", ""},
        {"r1", "d", "s2", "e"}};

    ProcList s1 = populateProc(sample_1);
    return 0;
}