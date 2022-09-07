#include "action/output/RemoveUntilLineEndActionOutput.hpp"

namespace feather::action
{
    pair RemoveUntilLineEndActionOutput::getResult()
    {
        return newCursorPosition;
    }
} // namespace feather::action
