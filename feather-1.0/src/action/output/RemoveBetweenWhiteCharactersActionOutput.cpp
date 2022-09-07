#include "action/output/RemoveBetweenWhiteCharactersActionOutput.hpp"

namespace feather::action
{
    pair RemoveBetweenWhiteCharactersActionOutput::getResult()
    {
        return newCursorPosition;
    }
} // namespace feather::action