#pragma once

#include "utils/datatypes/Global.hpp"

#include <cstddef>
#include <utility>

namespace feather::action
{
struct AlignToMiddleOfScreenActionOutput
{
    pair newCursorPosition;
    pair getResult();
};
} // namespace feather::action
