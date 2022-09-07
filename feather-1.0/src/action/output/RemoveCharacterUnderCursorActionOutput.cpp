#include "action/output/RemoveCharacterUnderCursorActionOutput.hpp"

namespace feather::action
{
pair RemoveCharacterUnderCursorActionOutput::getResult()
{
    return newCursorPosition;
}
} // namespace feather::action
