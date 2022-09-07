#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
struct SwitchModeActionOutput
{
    pair result;
    pair getResult();
};
} // namespace feather::action