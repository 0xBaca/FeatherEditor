#include "action/output/RemoveWordBackwardActionOutput.hpp"

namespace feather::action
{
pair RemoveWordBackwardActionOutput::getResult()
{
    return newCursorPosition;
}
} // namespace feather::action
