#pragma once

#include "WindowsAbstractFactory.hpp"

namespace feather::windows
{
struct WindowsManagerDummy : public WindowsManager
{
    explicit WindowsManagerDummy(WindowsAbstractFactory *window, std::shared_ptr<utils::logger::policy::FileLogPolicy> log)
        : WindowsManager(window, log) {}
    explicit WindowsManagerDummy(WindowsAbstractFactory *window, std::shared_ptr<utils::logger::policy::MemoryLogPolicy> log)
        : WindowsManager(window, log) {}
    explicit WindowsManagerDummy(WindowsAbstractFactory *window, std::shared_ptr<utils::logger::policy::OutputDeviceLogPolicy> log)
        : WindowsManager(window, log) {}
};
} // namespace feather::windows
