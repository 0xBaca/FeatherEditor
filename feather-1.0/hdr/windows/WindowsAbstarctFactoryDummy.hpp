#pragma once

#include "windows/WindowsAbstractFactory.hpp"

struct WinsowsAbstractFactoryDummy : public WinsowsAbstractFactory {
    WindowsAbstractFactoryDummy(): WindowsAbstractFactory() {
    }
}
