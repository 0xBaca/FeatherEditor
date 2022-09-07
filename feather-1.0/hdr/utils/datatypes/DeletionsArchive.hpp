#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::utils::datatypes
{
    struct DeletionsArchive
    {
        std::string watermark;
        std::map<pair, pair> const &deletions;

        template <class Archive>
        void save(Archive &ar) const
        {
            ar(watermark, deletions);
        }

        template <class Archive>
        void load(Archive &ar)
        {
            ar(watermark, deletions);
        }
    };
}