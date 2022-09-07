#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
struct RemoveWordForwardActionOutput
{
    pair newCursorPosition;
    pair getResult();
};
} // namespace feather::action
