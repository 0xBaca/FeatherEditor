#ifndef HELPER_TIMEOUT
#define HELPER_TIMEOUT

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <thread>

using namespace std::chrono_literals;

class Timeout
{
private:
  void wait(std::chrono::duration<int, std::ratio<1, 100>>);
  std::condition_variable cv;
  std::mutex mutex;

public:
  static void wait(Timeout*, std::chrono::duration<int, std::ratio<1, 100>>);
  void breakTimeout();
};
#endif
