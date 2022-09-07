#pragma once

#include "utils/datatypes/Colors.hpp"
#include "utils/datatypes/Global.hpp"

#include <map>
#include <set>

namespace feather::utils::datatypes
{
    struct DeserializedColorsArchive
    {
        std::string watermark;
        size_t colorPriority;
        std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> textChunksColors;
        std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> textColors;

        template <class Archive>
        void save(Archive &ar) const
        {
            ar(watermark, colorPriority, textChunksColors, textColors);
        }

        template <class Archive>
        void load(Archive &ar)
        {
            ar(watermark, colorPriority, textChunksColors, textColors);
        }
    };
}