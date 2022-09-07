#include "utils/windows/FramePositions.hpp"

namespace feather::utils::windows
{
    FramePositions::FramePositions() : startFramePosition{0, 0}, endFramePosition{0, 0} {}
    FramePositions::FramePositions(pair start, pair end) : startFramePosition(start), endFramePosition(end) {}
    FramePositions::FramePositions(const FramePositions &other)
    {
        startFramePosition = other.startFramePosition;
        endFramePosition = other.endFramePosition;
    }

    /* Frame start position is inclusive but end position is the first position that is outside of the frame */
    bool FramePositions::isInsideFrame(pair pos) const
    {
        if ((pos.first < startFramePosition.first) ||
            (pos.first == startFramePosition.first && pos.second < startFramePosition.second) ||
            (pos.first == endFramePosition.first && pos.second > endFramePosition.second) ||
            (pos.first > endFramePosition.first))
        {
            return false;
        }
        return true;
    }

    void FramePositions::incrementRealPosition(size_t bytes)
    {
        while (bytes--)
        {
            ++startFramePosition.first;
        }
        startFramePosition.second = 0;
    }

    void FramePositions::setStartPosition(pair newStartPos)
    {
        startFramePosition = newStartPos;
    }
} // namespace feather::utils::windows
