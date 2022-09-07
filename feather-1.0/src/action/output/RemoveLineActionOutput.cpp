#include "action/output/RemoveLineActionOutput.hpp"

namespace feather::action
{
    pair RemoveLineActionOutput::getResult()
    {
        return newCursorPosition;
    }
} // namespace feather::action