#include "action/output/AlignToMiddleOfScreenActionOutput.hpp"

namespace feather::action
{
pair AlignToMiddleOfScreenActionOutput::getResult()
{
    return newCursorPosition;
}
} // namespace feather::action
