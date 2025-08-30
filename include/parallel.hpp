#pragma once

#include <functional>
#include <thread>
#include <atomic>

namespace parallel
{
    void runParallel(
        std::function<void()> taskA,
        std::function<void()> taskB,
        int coreA = -1,
        int coreB = -1);
}
