#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
struct MoveToLineEndWithEditActionOutput
{
    pair cursorRealPosition;
    pair getResult();
};
} // namespace feather::action
