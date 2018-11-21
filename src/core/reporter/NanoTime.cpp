#include <chrono>
#include <mutex>
#include "NanoTime.h"


namespace future {
    std::mutex NANO_TIME_GUARD_LOCK;
    static std::chrono::time_point<std::chrono::system_clock> NANO_START_TIME(
            std::chrono::system_clock::now());

    std::int64_t GetNanoTime(void) {
        std::lock_guard<std::mutex> lock(NANO_TIME_GUARD_LOCK);
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> lapseTime = std::chrono::time_point_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now());
        std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> startTime = std::chrono::time_point_cast<std::chrono::nanoseconds>(
                NANO_START_TIME);
        std::int64_t nanoTime =
                lapseTime.time_since_epoch().count() + startTime.time_since_epoch().count();
        return nanoTime;
    }
}
