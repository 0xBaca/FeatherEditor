#include "action/output/RemoveUntilFileBeginActionOutput.hpp"

namespace feather::action
{
    pair RemoveUntilFileBeginActionOutput::getResult()
    {
        return newCursorPosition;
    }
} // namespace feather::action