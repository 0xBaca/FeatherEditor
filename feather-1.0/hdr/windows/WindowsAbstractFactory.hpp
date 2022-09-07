#pragma once

#include "utils/BufferFiller.hpp"
#include "windows/MainWindowInterface.hpp"
#include "windows/SubWindowInterface.hpp"

namespace feather::windows
{
  class WindowsAbstractFactory
  {
  public:
    static size_t MIN_ACCEPTABLE_WINDOW_HEIGHT;
    static size_t MIN_ACCEPTABLE_WINDOW_WIDTH;

    static size_t AGREEMENT_WINDOW_HEIGHT;
    static size_t AGREEMENT_WINDOW_WIDTH;
    static size_t EXIT_WINDOW_WIDTH;
    static size_t EXIT_WINDOW_HEIGHT;
    static size_t PROGRESS_WINDOW_WIDTH;
    static size_t PROGRESS_WINDOW_HEIGHT;
    static size_t SHORTCUTS_WINDOW_WIDTH;

    virtual std::unique_ptr<SubWindowInterface> createAgreementWindow(std::shared_ptr<MainWindowInterface>) = 0;
    virtual std::pair<pair, pair> getAgreementWindowDimensions() = 0;
    virtual std::shared_ptr<MainWindowInterface> createMainWindow(std::unique_ptr<utils::BufferFillerInterface>, std::optional<utils::datatypes::Uuid> const &) = 0;
    virtual std::pair<pair, pair> getMainWindowDimensions() = 0;
    virtual std::unique_ptr<MainWindowInterface> createSecondaryHexModeMainWindow(std::unique_ptr<utils::BufferFillerInterface>, std::optional<utils::datatypes::Uuid> const &) = 0;
    virtual std::pair<pair, pair> getSecondaryHexModeMainWindowDimensions() = 0;
    virtual std::unique_ptr<SubWindowInterface> createBottomBarWindow(std::shared_ptr<MainWindowInterface>) = 0;
    virtual std::pair<pair, pair> getBottomBarWidowDimensions() = 0;
    virtual std::unique_ptr<SubWindowInterface> createExitWindow(std::shared_ptr<MainWindowInterface>) = 0;
    virtual std::pair<pair, pair> getExitWidowDimensions() = 0;
    virtual std::unique_ptr<SubWindowInterface> createInfoWindow(std::shared_ptr<MainWindowInterface>) = 0;
    virtual std::pair<pair, pair> getInfoWidowDimensions() = 0;
    virtual std::unique_ptr<SubWindowInterface> createProgressWindow(std::shared_ptr<MainWindowInterface>) = 0;
    virtual std::pair<pair, pair> getProgressWidowDimensions() = 0;
    virtual std::unique_ptr<SubWindowInterface> createShortcutsWindow(std::shared_ptr<MainWindowInterface>) = 0;
    virtual std::pair<pair, pair> getShortcutsWidowDimensions() = 0;
    virtual std::unique_ptr<SubWindowInterface> createTopWindow(std::shared_ptr<MainWindowInterface>) = 0;
    virtual std::pair<pair, pair> getTopWidowDimensions() = 0;
    virtual std::shared_ptr<utils::logger::policy::FileLogPolicy> getLog() = 0;
    virtual ~WindowsAbstractFactory();
  };
} // namespace feather::windows