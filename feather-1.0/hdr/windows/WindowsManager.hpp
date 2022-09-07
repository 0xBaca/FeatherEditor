#pragma once

#include "config/Config.hpp"
#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif
#include "utils/FeatherMode.hpp"
#include "utils/datatypes/UuidHash.hpp"
#include "utils/BufferFillerInterface.hpp"
#include "utils/logger/policy/MemoryLogPolicy.hpp"
#include "utils/logger/policy/OutputDeviceLogPolicy.hpp"
#include "utils/windows/WindowStateContainer.hpp"
#include "windows/NCursesMainWindowDecorator.hpp"
#include "windows/WindowsAbstractFactory.hpp"

#include <atomic>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace feather::windows
{
    class WindowsManager
    {
#ifdef _FEATHER_TEST_
        FRIEND_TEST(WindowsManagerTest, successfullyCreatesMainWindow);
        FRIEND_TEST(WindowsManagerTest, successfullyCreateBottomBarWindow);
        FRIEND_TEST(WindowsManagerTest, successfullyCreateProgressWindow);
#endif
    public:
        static std::shared_ptr<WindowsManager> getInstance(WindowsAbstractFactory *, std::shared_ptr<utils::logger::policy::FileLogPolicy>);
        static std::shared_ptr<WindowsManager> getInstance(WindowsAbstractFactory *, std::shared_ptr<utils::logger::policy::MemoryLogPolicy>);
        static std::shared_ptr<WindowsManager> getInstance(WindowsAbstractFactory *, std::shared_ptr<utils::logger::policy::OutputDeviceLogPolicy>);
        std::shared_ptr<MainWindowInterface> addNCursesMainWindow(std::unique_ptr<utils::BufferFillerInterface>, std::optional<utils::datatypes::Uuid> const &);
        std::shared_ptr<MainWindowInterface> addSecondaryNCursesMainWindowForHexMode(std::unique_ptr<utils::BufferFillerInterface>, std::optional<utils::datatypes::Uuid> const &);
        std::unique_ptr<SubWindowInterface> &addNCursesAgreementWindow(std::shared_ptr<MainWindowInterface>);
        std::unique_ptr<SubWindowInterface> &addNCursesBottomBarWindow(std::shared_ptr<MainWindowInterface>);
        std::unique_ptr<SubWindowInterface> &addNCursesExitWindow(std::shared_ptr<MainWindowInterface>);
        std::unique_ptr<SubWindowInterface> &addNCursesInfoWindow(std::shared_ptr<MainWindowInterface>);
        std::unique_ptr<SubWindowInterface> &addNCursesProgressWindow(std::shared_ptr<MainWindowInterface>);
        std::unique_ptr<SubWindowInterface> &addNCursesShortcutsWindow(std::shared_ptr<MainWindowInterface>);
        std::unique_ptr<SubWindowInterface> &addNCursesTopWindow(std::shared_ptr<MainWindowInterface>);
        virtual std::shared_ptr<MainWindowInterface> getMainWindow(utils::datatypes::Uuid const &);
        std::unique_ptr<SubWindowInterface> &getProgressSubWindow(utils::datatypes::Uuid const &);
        std::unique_ptr<SubWindowInterface> &getSubWindow(utils::datatypes::Uuid const &);
        void replaceActiveWindow(std::shared_ptr<MainWindowInterface>);
        std::optional<pair> reloadWindowContent(std::string const &, std::shared_ptr<utils::storage::AbstractStorageFactory>);
        void hideWindow(utils::datatypes::Uuid const &, std::unique_ptr<printer::PrintingVisitorInterface> &);
        void showWindow(utils::datatypes::Uuid const &, std::unique_ptr<printer::PrintingVisitorInterface> &);
        utils::datatypes::Uuid const &getActiveMainWindow() const;
        utils::datatypes::Uuid const &getSecondaryActiveMainWindowForHexMode() const;
        std::optional<utils::datatypes::Uuid> getAgreementWindow(utils::datatypes::Uuid const &) const;
        std::optional<utils::datatypes::Uuid> getBottomBarWindow(utils::datatypes::Uuid const &) const;
        std::optional<utils::datatypes::Uuid> getExitWindow(utils::datatypes::Uuid const &) const;
        std::optional<utils::datatypes::Uuid> getProgressBarWindow(utils::datatypes::Uuid const &) const;
        std::optional<utils::datatypes::Uuid> getShortcutsWindow(utils::datatypes::Uuid const &) const;
        std::optional<std::string> getNexWindowToLoad(std::string const &, std::shared_ptr<utils::Filesystem>);
        std::optional<std::string> getPrevWindowToLoad(std::string const &, std::shared_ptr<utils::Filesystem>);
        void recreateWindows();
        void refreshWindowWithItsDependants(utils::datatypes::Uuid const &, utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::pair<size_t, size_t>, std::unique_ptr<printer::PrintingVisitorInterface> &, utils::FEATHER_MODE const &);
        virtual void refreshAllWindows(std::unique_ptr<printer::PrintingVisitorInterface> &, std::set<utils::datatypes::Uuid> = {});
        virtual void refreshAllWindows(std::unique_ptr<printer::PrintingVisitorInterface> &&, std::set<utils::datatypes::Uuid> = {});
        virtual void refreshExitWindow(std::unique_ptr<printer::PrintingVisitorInterface> &, utils::datatypes::Uuid const &, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt);
        virtual void refreshInfoWindow(std::unique_ptr<printer::PrintingVisitorInterface> &, utils::datatypes::Uuid const &, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt);
        virtual void refreshMainWindow(std::unique_ptr<printer::PrintingVisitorInterface> &, utils::datatypes::Uuid const &);
        virtual void refreshProgresWindow(std::unique_ptr<printer::PrintingVisitorInterface> &, utils::datatypes::Uuid const &, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt);
        virtual void refreshShortcutsWindow(std::unique_ptr<printer::PrintingVisitorInterface> &, utils::datatypes::Uuid const &, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt);
        void setActiveMainWindow(utils::datatypes::Uuid const &);
        void setExitWindowVisibility(bool);
        void fitToNewScreenSize();
        WindowsManager() {}
        virtual ~WindowsManager();

    protected:
        explicit WindowsManager(WindowsAbstractFactory *, std::shared_ptr<utils::logger::policy::FileLogPolicy> log);
        explicit WindowsManager(WindowsAbstractFactory *, std::shared_ptr<utils::logger::policy::MemoryLogPolicy> log);
        explicit WindowsManager(WindowsAbstractFactory *, std::shared_ptr<utils::logger::policy::OutputDeviceLogPolicy> log);
        // WindowsManager() {}

    private:
        WindowsManager(const WindowsManager &) = delete;
        WindowsManager(const WindowsManager &&) = delete;
        WindowsManager &operator=(const WindowsManager &) = delete;
        WindowsManager &operator=(const WindowsManager &&) = delete;
        std::shared_ptr<MainWindowInterface> addMainWindow(std::shared_ptr<MainWindowInterface>);
        std::unique_ptr<SubWindowInterface> &addSubWindow(std::unique_ptr<SubWindowInterface>, utils::datatypes::Uuid const &);
        std::unique_ptr<SubWindowInterface> &getSubWindowById(utils::datatypes::Uuid const &);
        std::unordered_map<utils::datatypes::Uuid, std::shared_ptr<MainWindowInterface>, utils::datatypes::UuidHasher> mainWindows;
        std::unordered_map<utils::datatypes::Uuid, std::unique_ptr<SubWindowInterface>, utils::datatypes::UuidHasher> subWindows;
        std::unordered_map<utils::datatypes::Uuid, std::set<utils::datatypes::Uuid>, utils::datatypes::UuidHasher> mainToSubWindows;
        std::unordered_map<std::string, std::pair<utils::windows::WindowStateContainer, std::optional<utils::windows::WindowStateContainer>>> fileNameToWindowState;
        std::unordered_map<utils::datatypes::Uuid, utils::datatypes::Uuid, utils::datatypes::UuidHasher> secondaryMainWindowForHexMode;
        std::optional<utils::datatypes::Uuid> activeMainWindow;
        std::unique_ptr<WindowsAbstractFactory> windowsFactory;
        std::shared_ptr<utils::logger::policy::FileLogPolicy> log;
        std::shared_ptr<utils::logger::policy::MemoryLogPolicy> logMemory;
        std::shared_ptr<utils::logger::policy::OutputDeviceLogPolicy> logOutput;
        std::list<std::string> loadedWindows;
        bool isExitWindowVisible;
    };
} // namespace feather::windows
