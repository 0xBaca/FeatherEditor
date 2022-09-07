#pragma once

#include "utils/datatypes/Error.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "windows/WindowsManager.hpp"

#include <memory>
#include <optional>

namespace feather::utils::algorithm
{
   class SearchEngine
   {
   public:
      virtual std::pair<std::optional<std::pair<pair, size_t>>, utils::datatypes::ERROR> getNextMatch(std::shared_ptr<feather::windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, utils::FEATHER_MODE &, utils::Direction) = 0;
      virtual ~SearchEngine() = 0;
   };
} // namespace feather::utils::algorithm