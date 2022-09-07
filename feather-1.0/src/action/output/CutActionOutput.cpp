#include "action/output/CutActionOutput.hpp"

namespace feather::action
{
    std::optional<std::pair<pair, pair>> CutActionOutput::getResult()
    {
        return newCursorPosition;
    }
} // namespace feather::actio