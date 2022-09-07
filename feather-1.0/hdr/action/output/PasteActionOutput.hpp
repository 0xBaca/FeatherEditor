#pragma once

#include "utils/datatypes/DeletionsSnapshot.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/Uuid.hpp"
#include "utils/datatypes/UuidHash.hpp"

#include <optional>
#include <unordered_map>
#include <set>

namespace feather::action
{
    struct PasteActionOutput
    {
        pair result;
        utils::datatypes::DeletionsSnapshot deletionsSnapshot;
        std::set<pair> addedBreakPointsUpdated;
        std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> positionsToSkipUpdated;
    };
} // namespace feather::action
