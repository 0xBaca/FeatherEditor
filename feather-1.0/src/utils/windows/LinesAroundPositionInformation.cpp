#include "utils/windows/LinesAroundPositionInformation.hpp"

namespace feather::utils::windows
{
    LinesAroundPositionInformation::LinesAroundPositionInformation(std::tuple<std::pair<std::size_t, bool>, std::pair<std::size_t, bool>, std::pair<std::size_t, bool>> &&input)
        : linesLength(input)
    {
    }

    std::tuple<std::pair<std::size_t, bool>, std::pair<std::size_t, bool>, std::pair<std::size_t, bool>> LinesAroundPositionInformation::getInformation()
    {
        return linesLength;
    }
} // namespace feather::utils::windows
