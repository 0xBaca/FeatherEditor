#include "action/output/MoveToLineEndWithEditActionOutput.hpp"

namespace feather::action
{
pair MoveToLineEndWithEditActionOutput::getResult()
{
    return cursorRealPosition;
}
} // namespace feather::action
