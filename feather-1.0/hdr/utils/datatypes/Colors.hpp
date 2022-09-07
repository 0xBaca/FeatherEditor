#pragma once

#include <functional>
#include <numeric>
#include <set>

namespace feather::utils::datatypes
{
    enum class COLOR
    {
        FEATHER_COLOR_BLACK,
        FEATHER_COLOR_BLACK_BCKG,
        FEATHER_COLOR_RED,
        FEATHER_COLOR_RED_BCKG,
        FEATHER_COLOR_GREEN,
        FEATHER_COLOR_GREEN_BCKG,
        FEATHER_COLOR_YELLOW,
        FEATHER_COLOR_YELLOW_BCKG,
        FEATHER_COLOR_BLUE,
        FEATHER_COLOR_BLUE_BCKG,
        FEATHER_COLOR_MAGENTA,
        FEATHER_COLOR_MAGENTA_BCKG,
        FEATHER_COLOR_ORANGE,
        FEATHER_COLOR_ORANGE_BCKG,
        FEATHER_COLOR_CYAN,
        FEATHER_COLOR_CYAN_BCKG,
        FEATHER_COLOR_WHITE,
        FEATHER_COLOR_WHITE_BCKG,
        FEATHER_COLOR_INFO_WINDOW,
        FEATHER_COLOR_INFO_WINDOW_GREEN_BACKGROUND,
        FEATHER_COLOR_INFO_WINDOW_RED_BACKGROUND,
        FEATHER_COLOR_DEFAULT_BACKGROUND,
        FEATHER_COLOR_GREEN_WITH_YELLOW_BACKGROUND,
        FEATHER_COLOR_GREY,
        FEATHER_COLOR_GREY_BCKG,
        FEATHER_COLOR_PROGRESS_WINDOW,
        FEATHER_HEX_WINDOW_CURSOR_COLOR,
        FEATHER_HEX_WINDOW_CURSOR_COLOR_BCKG,
        FEATHER_STRINGS_HIGHLIGHT_COLOR
    };

    struct ColorHash
    {
        std::size_t operator()(const std::set<COLOR> &t) const
        {
            return std::accumulate(t.begin(), t.end(), 123, [](std::size_t h, COLOR t){
                return h ^ std::hash<COLOR>()(t);
            });
        }
    };
} // namespace feather::utils::datatypes
