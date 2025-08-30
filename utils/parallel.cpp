#include "parallel.hpp"

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

namespace parallel
{

    static void setAffinity(std::thread &t, int core)
    {
#ifdef __linux__
        if (core < 0)
            return;
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core, &cpuset);
        int rc = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);
        (void)rc;
#endif
    }

    static void setAffinitySelf(int core)
    {
#ifdef __linux__
        if (core < 0)
            return;
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
#endif
    }

    void runParallel(
        std::function<void()> taskA,
        std::function<void()> taskB,
        int coreA,
        int coreB)
    {
        std::thread threadA([coreA, taskA]()
                            {
        setAffinitySelf(coreA);
        if (taskA) taskA(); });

        std::thread threadB([coreB, taskB]()
                            {
        setAffinitySelf(coreB);
        if (taskB) taskB(); });

        setAffinity(threadA, coreA);
        setAffinity(threadB, coreB);

        if (threadA.joinable())
            threadA.join();
        if (threadB.joinable())
            threadB.join();
    }

}
