#pragma once

#include "utils/datatypes/Global.hpp"

#include <utility>

namespace feather::utils::windows
{
    struct FramePositions
    {
        pair startFramePosition;
        pair endFramePosition;
        FramePositions();
        FramePositions(pair, pair);
        FramePositions(const FramePositions &other);
        bool isInsideFrame(pair) const;
        void incrementRealPosition(size_t noBytes = 1);
        void setStartPosition(pair);
    };
} // namespace feather::utils::windows