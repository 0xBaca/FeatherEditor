#pragma once

#include "printer/PrintingOrchestrator.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::reader
{
    class KeyStroke;
};

namespace feather::action
{
    struct GetStringsActionInput
    {
        windows::MainWindowInterface* window;
        GetStringsActionInput(windows::MainWindowInterface *);
    };
} // namespace feather::action