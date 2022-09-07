#pragma once

#include "utils/datatypes/Error.hpp"

#include <optional>
#include <string>

namespace feather::action
{
    struct SaveChangesActionOutput
    {
        utils::datatypes::ERROR result;
        std::optional<std::string> junkFile;
        SaveChangesActionOutput(utils::datatypes::ERROR, std::optional<std::string>);
        utils::datatypes::ERROR getResult() const;
    };
} // namespace feather::action