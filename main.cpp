#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>
template <class T>
using Input = std::vector<std::vector<T>>;
template <class T>
using Events = std::vector<T>;
using CalcItem = std::pair<std::string, int>;
using VerifyItem = std::pair<int, int>;
using CalcChannel = std::priority_queue<CalcItem, std::vector<CalcItem>, std::greater<CalcItem>>;
using VerifyChannel = std::priority_queue<VerifyItem, std::vector<VerifyItem>, std::greater<VerifyItem>>;
using Calculated = std::vector<std::vector<int>>;
using Verified = std::vector<std::vector<std::string>>;

std::mutex mtx;
std::condition_variable cv;
//collects send events
CalcChannel cc;
//collects receive events
Verified v_result;
VerifyChannel vc;
int max_sleeper = 0;
int sleeper = 0;
bool newIteration = false;
int eventCount = 1;

int compareMax(int sent, int beforeReceive)
{
    return (sent > beforeReceive) ? sent + 1 : beforeReceive + 1;
}
template <class T>
bool calcUnlock(T deadend)
{
    if (cc.top().first[1] == deadend[1])
    {
        --sleeper;
        return true;
    }
    else
    {
        return false;
    }
}

void CalcCheckpoint()
{
    while (max_sleeper != 0)
    {
        if (!newIteration)
        {
            if (sleeper == max_sleeper)
            {
                newIteration = true;
                cv.notify_all();
            }
        }
    }
}

template <class T>
void calcLC(Events<T> input, Calculated &result, int i)
{
    if (!input.empty() || input[i].front())
    {
        size_t k = 0;
        for (size_t j = 0; j < input.size(); j++)
        {
            if (input[j][0] == 'r' && input[j].length() == 2)
            {
                std::unique_lock<std::mutex> lck(mtx);
                newIteration = false;
                //only unlock the cv if the earliest send matches the receive.
                ++sleeper;
                cv.wait(lck, [=]() {
                    return calcUnlock(input[j]);
                });
                k = compareMax(cc.top().second, k);
                cc.pop();
                result[i][j] = k;
            }
            else if (input[j][0] == 's' && input[j].length() == 2)
            {
                result[i][j] = ++k;
                CalcItem x = std::make_pair(input[j], k);
                cc.push(x);
            }
            else if (input[j].length() == 1)
            {
                result[i][j] = ++k;
            }
            else if (input[j] == "")
            {
                result[i][j] = 0;
                break;
            }
        }
        --max_sleeper;
        newIteration = false;
    }
}

template <class T>
Calculated calculateLC(Input<T> input)
{
    max_sleeper = input.size();
    Calculated result(input.size(), std::vector<int>(input[0].size()));
    //preprocessing
    std::vector<std::thread> p(input.size());
    for (size_t i = 0; i < input.size(); i++)
    {
        p[i] = std::thread(calcLC<std::string>, input[i], std::ref(result), i);
    }
    std::thread helper = std::thread(CalcCheckpoint);

    for (size_t i = 0; i < input.size(); i++)
    {
        p[i].join();
    }
    helper.join();

    return result;
}
bool found = false;
bool isCorrect = false;
// evaluate the receive event LC value and compare it to the last LC value.
bool findRelatedEvents()
{
    for (size_t i = 0; i < v_result.size(); i++)
    {
        //a process cannot receive the send event from the same process.
        if (i != (size_t)(vc.top().second))
        {
            for (size_t j = 0; j < v_result[i].size(); j++)
            {
                //the current receive event minus the found LC value
                if (v_result[i][j][0] == 't')
                {
                    int tmp = vc.top().first - std::atoi(&v_result[i][j][1]);
                    if (tmp == 1)
                    {
                        v_result[i][j] = "s" + std::to_string(eventCount);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
void VerCheckpoint()
{
    while (max_sleeper != 0)
    {
        if (!newIteration)
        {
            if (sleeper == max_sleeper)
            {
                isCorrect = findRelatedEvents();
                if (!isCorrect)
                {
                    std::cout << "INCORRECT" << std::endl;
                    std::exit(1);
                }
                else
                {
                    found = true;
                    newIteration = true;
                    cv.notify_all();
                }
            }
        }
    }
}
bool verUnlock(int deadend)
{
    if (found && vc.top().first == deadend)
    {
        --sleeper;
        return true;
    }
    else
    {
        return false;
    }
}

template <class T>
void verLC(Events<T> input, int i)
{
    if (!input.empty() || input.front() != 0)
    {
        for (size_t j = 0; j < input.size(); j++)
        {
            //labeling each LC value with a terminal event.
            if ((j == 0 && input[j] > 1) || (j > 0 && input[j] - input[j - 1] > 1))
            {
                std::unique_lock<std::mutex> lck(mtx);
                VerifyItem x = std::make_pair(input[j], i);
                vc.push(x);
                newIteration = false;
                ++sleeper;
                cv.wait(lck, [=]() {
                    return verUnlock(input[j]);
                });
                v_result[i][j] = "r" + std::to_string(eventCount++);
                vc.pop();
                found = false;
            }
            else if ((j == 0 && input[j] == 1) || (j > 0 && input[j] - input[j - 1] == 1))
            {
                v_result[i][j] = "t" + std::to_string(input[j]);
            }
            else if (input[j] == 0)
            {
                v_result[i][j] = "NULL";
            }
        }
        --max_sleeper;
        newIteration = false;
    }
}
void displayVerifyResult()
{
    char alpha = 'a';
    for (size_t i = 0; i < v_result.size(); i++)
    {
        for (size_t j = 0; j < v_result[i].size(); j++)
        {
            if (v_result[i][j][0] == 't')
            {
                v_result[i][j] = alpha++;
            }
        }
    }
    for (size_t i = 0; i < v_result.size(); i++)
    {
        for (size_t j = 0; j < v_result[i].size(); j++)
        {
            std::cout << v_result[i][j] << " ";
        }
        std::cout << std::endl;
    }
    eventCount = 1;
}
template <class T>
void verifyLC(Input<T> input)
{
    max_sleeper = input.size();
    v_result.resize(input.size());
    for (size_t i = 0; i < input.size(); i++)
    {
        v_result[i].resize(input[i].size());
    }
    //preprocessing
    std::vector<std::thread> p(input.size());
    for (size_t i = 0; i < input.size(); i++)
    {
        p[i] = std::thread(verLC<int>, input[i], i);
    }
    std::thread helper = std::thread(VerCheckpoint);
    for (size_t i = 0; i < input.size(); i++)
    {
        p[i].join();
    }
    helper.join();
    displayVerifyResult();
}

template <class T>
void displayResult(T result)
{
    for (size_t i = 0; i < result.size(); i++)
    {
        for (size_t j = 0; j < result[i].size(); j++)
        {
            std::cout << result[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    Input<std::string> sample_1{
        {"a", "s1", "r3", "b"},
        {"c", "r2", "s3", ""},
        {"r1", "d", "s2", "e"}};
    Input<std::string> sample_2{
        {"s1", "b", "r3", "e"},
        {"a", "r2", "s3", ""},
        {"r1", "c", "d", "s2"}};
    Input<int> sample_3{
        {1, 2, 8, 9},
        {1, 6, 7, 0},
        {2, 3, 4, 5}};
    Input<int> sample_4{
        {1, 2, 8, 9},
        {1, 6, 7, 0},
        {2, 4, 5, 6}};

    std::cout << std::endl;
    std::cout << "Calculation" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "Test Case 01: " << std::endl;
    displayResult(calculateLC(sample_1));
    std::cout << "----------------" << std::endl;
    std::cout << "Test Case 02: " << std::endl;
    displayResult(calculateLC(sample_2));
    std::cout << "----------------" << std::endl;

    std::cout << "Verification" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "Test Case 01: " << std::endl;
    std::cout << "----------------" << std::endl;
    verifyLC(sample_3);
    std::cout << "----------------" << std::endl;
    std::cout << "Test Case 02: " << std::endl;
    std::cout << "----------------" << std::endl;
    verifyLC(sample_4);
    std::cout << "----------------" << std::endl;

    return 0;
}