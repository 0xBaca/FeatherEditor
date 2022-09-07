#pragma once

#include "utils/datatypes/Global.hpp"

#include "cereal/types/vector.hpp"

namespace feather::utils::datatypes
{
    struct DeserializedChangesArchive
    {
        std::string watermark;
        size_t position;
        std::vector<char> changes;

        template <class Archive>
        void save(Archive &ar) const
        {
            ar(watermark, position, changes);
        }

        template <class Archive>
        void load(Archive &ar)
        {
            ar(watermark, position, changes);
        }
    };
}