#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

//aliasing
template <class T>
using Input = std::vector<std::vector<T>>;
template <class T>
using Events = std::vector<T>;
using ChannelItem = std::pair<std::string, size_t>;
using Channel = std::queue<ChannelItem>;
using Calculated = std::vector<std::vector<size_t>>;
using Verified = std::vector<std::vector<std::string>>;

std::mutex mtx;
std::condition_variable cv;

Channel channel;
size_t max_sleeper = 0;
size_t sleeper = 0;
bool newIteration = false;
size_t calcMaxLC(size_t sent, size_t beforeReceive)
{
    return (sent > beforeReceive) ? sent + 1 : beforeReceive + 1;
}
template <class T>
bool onlyUnlock(T deadend)
{
    if (channel.front().first.at(1) == deadend.at(1))
    {
        --sleeper;
        return true;
    }
    else
    {
        return false;
    }
}

void breakDeadEnd()
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
void calcLC(Events<T> input, Calculated &result, size_t i)
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
                    return onlyUnlock(input[j]);
                });
                k = calcMaxLC(channel.front().second, k);
                channel.pop();
                result[i][j] = k;
            }
            else if (input[j][0] == 's' && input[j].length() == 2)
            {
                result[i][j] = ++k;
                ChannelItem x = std::make_pair(input[j], k);
                channel.push(x);
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
    Calculated result(input.size(), std::vector<size_t>(input[0].size()));
    //preprocessing
    std::vector<std::thread> p(input.size());
    for (size_t i = 0; i < input.size(); i++)
    {
        p[i] = std::thread(calcLC<std::string>, input[i], std::ref(result), i);
    }
    std::thread helper = std::thread(breakDeadEnd);

    for (size_t i = 0; i < input.size(); i++)
    {
        p[i].join();
    }
    helper.join();

    return result;
}

template <class T>
Verified verifyLC(Input<T> input)
{
    Verified result;

    return result;
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
    std::cout << "Test Case 01: " << std::endl;
    displayResult(verifyLC(sample_1)); 
    return 0;
}