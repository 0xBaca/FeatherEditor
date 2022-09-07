#pragma once

#include "utils/logger/policy/FileLogPolicy.hpp"
#include "windows/WindowsAbstractFactory.hpp"

namespace feather::windows
{
  class NCursesWindowsFactory : public WindowsAbstractFactory
  {
  public:
    static WindowsAbstractFactory *getInstance(std::shared_ptr<utils::logger::policy::FileLogPolicy>);
    std::unique_ptr<SubWindowInterface> createAgreementWindow(std::shared_ptr<MainWindowInterface>) override;
    std::pair<pair, pair> getAgreementWindowDimensions() override;
    std::shared_ptr<MainWindowInterface> createMainWindow(std::unique_ptr<utils::BufferFillerInterface>, std::optional<utils::datatypes::Uuid> const &) override;
    std::pair<pair, pair> getMainWindowDimensions() override;
    std::unique_ptr<MainWindowInterface> createSecondaryHexModeMainWindow(std::unique_ptr<utils::BufferFillerInterface>, std::optional<utils::datatypes::Uuid> const &) override;
    std::pair<pair, pair> getSecondaryHexModeMainWindowDimensions() override;
    std::unique_ptr<SubWindowInterface> createBottomBarWindow(std::shared_ptr<MainWindowInterface>) override;
    std::pair<pair, pair> getBottomBarWidowDimensions() override;
    std::unique_ptr<SubWindowInterface> createExitWindow(std::shared_ptr<MainWindowInterface>) override;
    std::pair<pair, pair> getExitWidowDimensions() override;
    std::unique_ptr<SubWindowInterface> createInfoWindow(std::shared_ptr<MainWindowInterface>) override;
    std::pair<pair, pair> getInfoWidowDimensions() override;
    std::unique_ptr<SubWindowInterface> createProgressWindow(std::shared_ptr<MainWindowInterface>) override;
    std::pair<pair, pair> getProgressWidowDimensions() override;
    std::unique_ptr<SubWindowInterface> createShortcutsWindow(std::shared_ptr<MainWindowInterface>) override;
    std::pair<pair, pair> getShortcutsWidowDimensions() override;
    std::unique_ptr<SubWindowInterface> createTopWindow(std::shared_ptr<MainWindowInterface>) override;
    std::pair<pair, pair> getTopWidowDimensions() override;
    std::shared_ptr<utils::logger::policy::FileLogPolicy> getLog() override;
    ~NCursesWindowsFactory() override;

  protected:
    NCursesWindowsFactory(std::shared_ptr<utils::logger::policy::FileLogPolicy>)
    {
      this->screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
    }

  private:
    std::shared_ptr<utils::logger::policy::FileLogPolicy> log;
    pair screenDimensions;
  };
} // namespace feather::windows