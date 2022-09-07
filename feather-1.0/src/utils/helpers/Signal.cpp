#include "utils/helpers/Signal.hpp"

sig_atomic_t sigIntReceived = 0;
sig_atomic_t sigBusReceived = 0;

namespace feather::utils::helpers
{
    extern "C" void lambdaSigBusHandler(int)
    {
        sigBusReceived = true;
    }

    extern "C" void lambdaSigIntHandler(int)
    {
        sigIntReceived = true;
    }

    void blockSignals(bool resetState)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGBUS);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_BLOCK, &mask, 0);
        if (resetState)
        {
            sigBusReceived = sigIntReceived = false;
        }
    }

    void unblockSignals()
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGBUS);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_UNBLOCK, &mask, 0);
        std::signal(SIGBUS, lambdaSigBusHandler);
        std::signal(SIGINT, lambdaSigIntHandler);
    }
}