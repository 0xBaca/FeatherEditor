#pragma once

#include "utils/Utf8Util.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

using namespace feather::utils;

class FileStorageHelper
{
public:
  template <typename ItFrom, typename ItTo,
            typename = std::enable_if<std::is_same<typename std::iterator_traits<ItFrom>::iterator_category, std::forward_iterator_tag>::value>>
  static void compareLine(ItFrom itFromCurr, ItFrom itFromEnd, std::shared_ptr<Utf8Util> util, ItTo itTo)
  {
    size_t pos = 0;
    while (itFromCurr != itFromEnd)
    {
      ASSERT_EQ(*itFromCurr, *itTo) << "It is " << *(itFromCurr) << " when expected " << *itTo << ", differs in pos " << pos << std::endl;
      ++itFromCurr;
      ++itTo;
      ++pos;
    }
  }
};
