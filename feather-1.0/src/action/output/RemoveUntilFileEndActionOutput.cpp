#include "action/output/RemoveUntilFileEndActionOutput.hpp"

namespace feather::action
{
    pair RemoveUntilFileEndActionOutput::getResult()
    {
        return newCursorPosition;
    }
} // namespace feather::actio