#pragma once
/**
 * @file parallel.hpp
 * @brief Simple helper to run two independent tasks in parallel.
 *
 * This tiny utility provides a single, well-documented function:
 *   - runParallel(taskA, taskB, coreA = -1, coreB = -1)
 *
 * It spawns two threads, optionally pins them to CPU cores (Linux-only),
 * waits for both to finish and propagates any exception thrown inside a
 * task back to the caller.
 *
 * Design goals:
 *  - Minimal public surface (single function).
 *  - Portable: cpu affinity code is compiled only on Linux.
 *  - Exception-safe: captures exceptions from tasks and rethrows after join.
 *
 * Author: a7mddra
 */

#include <functional>
#include <thread>

namespace parallel
{

/**
 * @brief Run two tasks in parallel, optionally pin each thread to a CPU core.
 *
 * Both tasks are executed concurrently on separate std::thread objects.
 * If a task throws an exception it is captured and rethrown in the calling
 * thread after both worker threads have been joined.
 *
 * @param taskA  First task to run. May be empty (nullptr) — then it's skipped.
 * @param taskB  Second task to run. May be empty (nullptr) — then it's skipped.
 * @param coreA  CPU core index to pin thread A to (Linux only). -1 = no affinity.
 * @param coreB  CPU core index to pin thread B to (Linux only). -1 = no affinity.
 *
 * @throws Any exception thrown by taskA or taskB will be rethrown from the
 *         calling thread after both worker threads have finished.
 *
 * @note Setting affinity is best-effort: it is a no-op on non-Linux platforms.
 */
void runParallel(
    std::function<void()> taskA,
    std::function<void()> taskB,
    int coreA = -1,
    int coreB = -1);

}
