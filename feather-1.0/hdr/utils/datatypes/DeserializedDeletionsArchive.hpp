#pragma once

#include "utils/datatypes/Global.hpp"

#include "cereal/types/map.hpp"

namespace feather::utils::datatypes
{
    struct DeserializedDeletionsArchive
    {
        std::string watermark;
        std::map<pair, pair> deletions;

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