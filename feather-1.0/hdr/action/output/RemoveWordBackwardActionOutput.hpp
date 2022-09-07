#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
struct RemoveWordBackwardActionOutput
{
    pair newCursorPosition;
    pair getResult();
};
} // namespace feather::action
