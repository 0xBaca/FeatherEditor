#include "helpers/Timeout.hpp"
#include "utils/exception/FeatherTimeoutException.hpp"

void Timeout::wait(std::chrono::duration<int, std::ratio<1, 100>> duration)
{
    std::unique_lock<std::mutex> lck(mutex);
    if (cv.wait_for(lck, duration) == std::cv_status::timeout)
    {
        throw feather::utils::exception::FeatherTimeoutException(duration, " ticks");
    }
}

void Timeout::wait(Timeout *obj, std::chrono::duration<int, std::ratio<1, 100>> duration)
{
    auto threadFunction = static_cast<void((Timeout::*)(std::chrono::duration<int, std::ratio<1, 100>>))>(&Timeout::wait);
    std::thread t(threadFunction, obj, duration);
    t.detach();
}

void Timeout::breakTimeout()
{
    cv.notify_one();
}
