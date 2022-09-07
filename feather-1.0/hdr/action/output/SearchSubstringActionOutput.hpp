#pragma once

#include "printer/PrintingOrchestrator.hpp"
#include "utils/datatypes/Error.hpp"

namespace feather::action
{
    struct SearchSubstringActionOutput
    {
        std::pair<std::optional<std::pair<pair, size_t>>, utils::datatypes::ERROR> result;
    };
} // namespace feather::action