#pragma once

#include <future>
#include <string>
#include <tuple>
#include <utility>

#include <cstddef>
#include <iostream>

#define ctrl(x) ((x)&0x1f)

namespace feather
{

  typedef std::pair<size_t, size_t> pair;

#ifdef _DEBUG_MODE_
  /*std::ostream &operator<<(std::ostream &out, pair const &in)
  {
    out << in.first << " " << in.second;
    return out;
  }
  */
#endif

  struct pair_hash
  {
    template <typename T1, typename T2>
    std::size_t operator()(std::pair<T1, T2> const &p) const
    {
      size_t result = std::hash<T1>()(std::get<0>(p));
      return (result << 16) + std::hash<T1>()(std::get<1>(p));
    }
  };

  struct rgb_hash
  {
    template <typename T1, typename T2, typename T3>
    std::size_t operator()(std::tuple<T1, T2, T3> const &t) const
    {
      size_t result = std::hash<T1>()(std::get<0>(t));
      result = (result << 16) + std::hash<T1>()(std::get<1>(t));
      return (result << 16) + std::hash<T1>()(std::get<2>(t));
    }
  };

  template <typename T>
  bool isFutureReady(std::future<T> const &future)
  {
    return (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
  }

  // UTF-8
  static const size_t HEX_MODE_CHRACTER_WIDTH = 3UL;
  static const size_t MAX_UTF8_CHAR_BYTES = 4UL;
  static const size_t MIN_MAIN_WINDOW_WIDTH = 30UL;
  static const size_t MIN_MAIN_WINDOW_HEIGHT = 6UL;

  // SPECIAL CHARACTERS
  static const std::string FEATHER_WATERMARK("0xFEAE\n");

  // TIME
  using namespace std::chrono_literals;
  static const std::chrono::milliseconds HALF_SECOND = 500ms;
  static const std::chrono::milliseconds PASTE_GRACE_PERIOD = 200ms;
  static const std::chrono::milliseconds SAVE_GRACE_PERIOD = 100ms;
  //Two minutes
  static const std::chrono::milliseconds UW_GRACE_PERIOD = 120000ms;
  //10 minutes
  static const std::chrono::milliseconds TRIAL_GRACE_PERIOD = 400000ms;
  static const std::chrono::seconds ONE_SECOND = 1s;
  static const std::chrono::seconds TWO_SECOND = 2s;
  static const std::chrono::seconds THREE_SECOND = 3s;

  // OTHER
  static constexpr size_t ONE_MEGABYTE = (1UL << 20);
  static const uint32_t ONE_YEAR_SECONDS = 31536000;
} // namespace feather