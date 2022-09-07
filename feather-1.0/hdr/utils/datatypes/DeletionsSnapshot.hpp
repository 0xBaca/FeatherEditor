#pragma once

#include "utils/datatypes/Global.hpp"

#include <map>

namespace feather::utils::datatypes
{
    struct DeletionsSnapshot
    {
        size_t totalBytesOfDeletions;
        std::map<pair, pair> deletions;
        std::map<pair, pair> reverseDeletions;
        void shift(pair, size_t);
    };
}