#pragma once

#include <chrono>

class Timer
{
public:
    void start()
    {
        startTime = std::chrono::high_resolution_clock::now();
    }

    double stopMilliseconds()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
        return elapsed.count();
    }

private:
    std::chrono::high_resolution_clock::time_point startTime;
};