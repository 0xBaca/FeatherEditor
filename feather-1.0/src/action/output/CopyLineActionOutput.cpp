#include "action/output/CopyLineActionOutput.hpp"

namespace feather::action
{
    std::optional<std::pair<pair, pair>> CopyLineActionOutput::getResult()
    {
        return newCursorPosition;
    }
} // namespace feather::actio