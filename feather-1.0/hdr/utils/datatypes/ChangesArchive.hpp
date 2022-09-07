#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::utils::datatypes
{
    struct ChangesArchive
    {
        std::string watermark;
        size_t position;
        std::vector<char> const &changes;

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