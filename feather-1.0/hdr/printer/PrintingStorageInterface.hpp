#pragma once

#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/Colors.hpp"
#include "utils/datatypes/Uuid.hpp"

#include <map>
#include <set>
#include <unordered_map>

namespace feather::printer
{
  class PrintingStorageInterface
  {
  public:
    virtual void addToNewChanges(utils::datatypes::Uuid const &, pair, std::vector<char> const &, size_t) = 0;
    virtual std::vector<char> getNewChanges(utils::datatypes::Uuid const &, size_t, size_t, size_t) = 0;
    virtual void removeChanges(utils::datatypes::Uuid const &, size_t) = 0;
    virtual void dumpChanges(utils::datatypes::Uuid const &, pair, std::vector<char> const &) = 0;
    virtual void dumpColors(utils::datatypes::Uuid const &, std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, size_t) = 0;
    virtual void dumpDeletions(utils::datatypes::Uuid const &, std::map<pair, pair> const &) = 0;
    virtual ~PrintingStorageInterface() {}
  };
} // namespace feather::printer