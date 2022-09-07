#pragma once

#include <csignal>

namespace feather::utils::helpers
{
    extern "C" void lambdaSigBusHandler(int);
    extern "C" void lambdaSigIntHandler(int);

    void blockSignals(bool = true);
    void unblockSignals();
}
