#include "windows/WindowsAbstractFactory.hpp"

namespace feather::windows
{
    WindowsAbstractFactory::~WindowsAbstractFactory()
    {
    }

    size_t WindowsAbstractFactory::MIN_ACCEPTABLE_WINDOW_HEIGHT = 11UL;
    size_t WindowsAbstractFactory::MIN_ACCEPTABLE_WINDOW_WIDTH = 40UL;

    size_t WindowsAbstractFactory::AGREEMENT_WINDOW_HEIGHT = 10UL;
    size_t WindowsAbstractFactory::AGREEMENT_WINDOW_WIDTH = 36UL;
    size_t WindowsAbstractFactory::EXIT_WINDOW_HEIGHT = 6UL;
    size_t WindowsAbstractFactory::EXIT_WINDOW_WIDTH = 30UL;
    size_t WindowsAbstractFactory::PROGRESS_WINDOW_HEIGHT = 1UL;
    size_t WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH = 20UL;
    size_t WindowsAbstractFactory::SHORTCUTS_WINDOW_WIDTH = 35UL;
} // namespace feather::windows