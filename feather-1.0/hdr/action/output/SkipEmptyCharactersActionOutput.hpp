#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
struct SkipEmptyCharactersActionOutput
{
    pair result;
    pair getResult();
};
} // namespace feather::action