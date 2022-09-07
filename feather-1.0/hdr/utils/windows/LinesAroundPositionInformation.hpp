#pragma once

#include "utils/windows/WindowInformationInterface.hpp"

namespace feather::utils::windows
{
    struct LinesAroundPositionInformation : public WindowInformationInterface
    {
        std::tuple<std::pair<std::size_t, bool>, std::pair<std::size_t, bool>, std::pair<std::size_t, bool>> linesLength;
        LinesAroundPositionInformation(std::tuple<std::pair<std::size_t, bool>, std::pair<std::size_t, bool>, std::pair<std::size_t, bool>> &&);
        std::tuple<std::pair<std::size_t, bool>, std::pair<std::size_t, bool>, std::pair<std::size_t, bool>> getInformation() override;
    };
} // namespace feather::utils::windows
