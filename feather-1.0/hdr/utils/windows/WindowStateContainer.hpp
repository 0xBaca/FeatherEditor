#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"

namespace feather::utils::windows
{
    struct WindowStateContainer
    {
        std::optional<utils::datatypes::Uuid> windowUUID;
        std::optional<utils::datatypes::Uuid> siblingUUID;
        bool isSecondaryHexModeWindow;
        std::pair<pair, pair> secondaryWindowCursor;
        pair cursorRealPosition;
    };
}