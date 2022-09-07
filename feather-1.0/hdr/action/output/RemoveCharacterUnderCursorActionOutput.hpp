#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
struct RemoveCharacterUnderCursorActionOutput
{
    pair newCursorPosition;
    pair getResult();
};
} // namespace feather::action
