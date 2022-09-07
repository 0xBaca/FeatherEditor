#include "action/output/RemoveWordForwardActionOutput.hpp"

namespace feather::action
{
pair RemoveWordForwardActionOutput::getResult()
{
    return newCursorPosition;
}
} // namespace feather::action
