#pragma once

#include <tuple>

namespace feather::utils::windows
{
    struct WindowInformationInterface
    {
        virtual std::tuple<std::pair<std::size_t, bool>, std::pair<std::size_t, bool>, std::pair<std::size_t, bool>> getInformation() = 0;
        virtual ~WindowInformationInterface()
        {
        }
    };
} // namespace feather::utils::windows
