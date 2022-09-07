#include "printer/CurrFrameVisitor.hpp"
#include "utils/Filesystem.hpp"
#include "utils/NCursesWrapper.hpp"
#include "utils/logger/Logger.hpp"
#include "utils/exception/FeatherLogicalException.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"
#include "windows/NCursesExitWindowDecorator.hpp"
#include "windows/NCursesInfoWindowDecorator.hpp"
#include "windows/NCursesProgressWindowDecorator.hpp"
#include "windows/NCursesShortcutsWindowDecorator.hpp"
#include "windows/NCursesTopWindowDecorator.hpp"
#include "windows/WindowsManager.hpp"
#include "windows/WindowsManagerDummy.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::windows
{
    std::shared_ptr<WindowsManager> WindowsManager::getInstance(WindowsAbstractFactory *windowsFactory, std::shared_ptr<utils::logger::policy::FileLogPolicy> log)
    {
        return std::make_shared<WindowsManagerDummy>(windowsFactory, log);
    }

    std::shared_ptr<WindowsManager> WindowsManager::getInstance(WindowsAbstractFactory *windowsFactory, std::shared_ptr<utils::logger::policy::MemoryLogPolicy> log)
    {
        return std::make_shared<WindowsManagerDummy>(windowsFactory, log);
    }

    std::shared_ptr<WindowsManager> WindowsManager::getInstance(WindowsAbstractFactory *windowsFactory, std::shared_ptr<utils::logger::policy::OutputDeviceLogPolicy> log)
    {
        return std::make_shared<WindowsManagerDummy>(windowsFactory, log);
    }

    std::shared_ptr<MainWindowInterface> WindowsManager::addNCursesMainWindow(std::unique_ptr<utils::BufferFillerInterface> bufferFiller, std::optional<utils::datatypes::Uuid> const &existingWindowUUID)
    {
        bool isFirstWindowCreated = mainWindows.empty();
        std::string storageName = bufferFiller->getStorage()->getName();
        auto mainWindowHandler = addMainWindow(windowsFactory->createMainWindow(std::move(bufferFiller), existingWindowUUID));
        if (isFirstWindowCreated)
        {
            loadedWindows.push_back(storageName);
            mainWindowHandler->enableCursor();
        }
        mainWindowHandler->show();
        if (!mainWindowHandler->isSecondaryHexModeMainWindow() && !activeMainWindow.has_value())
        {
            activeMainWindow = mainWindowHandler->getUUID();
        }
        if (nullptr != log)
        {
            (*log)(utils::logger::ERROR, "Associated widow with ID :", mainWindowHandler->getUUID().getString(), "with file", storageName);
        }
        else
        {
            (*logOutput)(utils::logger::ERROR, "Associated widow with ID :", mainWindowHandler->getUUID().getString(), "with file", storageName);
        }
        isExitWindowVisible = false;
        return mainWindowHandler;
    }

    std::shared_ptr<MainWindowInterface> WindowsManager::addSecondaryNCursesMainWindowForHexMode(std::unique_ptr<utils::BufferFillerInterface> bufferFiller, std::optional<utils::datatypes::Uuid> const &siblingUUID)
    {
        std::string storageName = bufferFiller->getStorage()->getName();
        auto mainWindowHandler = addMainWindow(windowsFactory->createSecondaryHexModeMainWindow(std::move(bufferFiller), siblingUUID));
        mainWindowHandler->disableCursor();
        mainWindowHandler->show();
        secondaryMainWindowForHexMode[siblingUUID.value()] = mainWindowHandler->getUUID(true);
        if (nullptr != log)
        {
            (*log)(utils::logger::ERROR, "Associated widow with ID :", mainWindowHandler->getUUID().getString(), "with file", storageName);
        }
        else
        {
            (*logOutput)(utils::logger::ERROR, "Associated widow with ID :", mainWindowHandler->getUUID().getString(), "with file", storageName);
        }
        return mainWindowHandler;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::addNCursesAgreementWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto &window = addSubWindow(windowsFactory->createAgreementWindow(mainWindow), mainWindow->getUUID());
        window->disableCursor();
        window->hide();
        return window;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::addNCursesBottomBarWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto &window = addSubWindow(windowsFactory->createBottomBarWindow(mainWindow), mainWindow->getUUID());
        window->disableCursor();
        window->show();
        return window;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::addNCursesExitWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto &window = addSubWindow(windowsFactory->createExitWindow(mainWindow), mainWindow->getUUID());
        window->disableCursor();
        window->hide();
        return window;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::addNCursesInfoWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto &window = addSubWindow(windowsFactory->createInfoWindow(mainWindow), mainWindow->getUUID());
        window->disableCursor();
        window->show();
        return window;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::addNCursesProgressWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto &window = addSubWindow(windowsFactory->createProgressWindow(mainWindow), mainWindow->getUUID());
        window->disableCursor();
        window->show();
        return window;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::addNCursesShortcutsWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto &window = addSubWindow(windowsFactory->createShortcutsWindow(mainWindow), mainWindow->getUUID());
        window->disableCursor();
        window->hide();
        return window;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::addNCursesTopWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto &window = addSubWindow(windowsFactory->createTopWindow(mainWindow), mainWindow->getUUID());
        window->disableCursor();
        window->show();
        return window;
    }

    std::shared_ptr<MainWindowInterface> WindowsManager::getMainWindow(utils::datatypes::Uuid const &uuid)
    {
        return mainWindows.at(uuid);
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::getProgressSubWindow(utils::datatypes::Uuid const &uuid)
    {
        for (auto subWindow : mainToSubWindows[uuid])
        {
            if (dynamic_cast<windows::NCursesProgressWindowDecorator *>(subWindows[subWindow].get()))
            {
                return subWindows[subWindow];
            }
        }
        throw utils::exception::FeatherLogicalException("No window, with id", uuid.getString());
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::getSubWindow(utils::datatypes::Uuid const &uuid)
    {
        return subWindows.at(uuid);
    }

    void WindowsManager::replaceActiveWindow(std::shared_ptr<MainWindowInterface> window)
    {
        auto oldActiveMainWindow = getActiveMainWindow();
        if (configuration->isHexMode())
        {
            fileNameToWindowState[mainWindows[oldActiveMainWindow]->getStorage()->getAbsolutePath()].second = mainWindows[secondaryMainWindowForHexMode[oldActiveMainWindow]]->getState();
            mainWindows.erase(secondaryMainWindowForHexMode[oldActiveMainWindow]);
            secondaryMainWindowForHexMode.erase(oldActiveMainWindow);
        }
        fileNameToWindowState[mainWindows[oldActiveMainWindow]->getStorage()->getAbsolutePath()].first = mainWindows[oldActiveMainWindow]->getState();
        auto mainToSubWindowsRelation = mainToSubWindows[oldActiveMainWindow];
        mainToSubWindows[window->getUUID()] = mainToSubWindowsRelation;
        setActiveMainWindow(window->getUUID());
        for (auto &e : mainToSubWindows[window->getUUID()])
        {
            subWindows[e]->setNewParent(window);
        }
        mainWindows.erase(oldActiveMainWindow);
    }

    std::optional<pair> WindowsManager::reloadWindowContent(std::string const &newFileNamePath, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory)
    {
        if (newFileNamePath == mainWindows[getActiveMainWindow()]->getStorage()->getName())
        {
            return std::nullopt;
        }
        else if (fileNameToWindowState.count(newFileNamePath))
        {
            auto storage = storageFactory->getStorageWithUtf8Check(newFileNamePath);
            auto window = addNCursesMainWindow(std::make_unique<utils::BufferFiller>(storage, storageFactory), fileNameToWindowState[newFileNamePath].first.windowUUID);
            if (configuration->isHexMode())
            {
                auto secondaryMainWindow = addSecondaryNCursesMainWindowForHexMode(std::make_unique<utils::BufferFiller>(storage, storageFactory), window->getUUID());
                secondaryMainWindow->setState(fileNameToWindowState[newFileNamePath].second.value());
            }
            window->setState(fileNameToWindowState[newFileNamePath].first);
            replaceActiveWindow(window);
            return fileNameToWindowState[newFileNamePath].first.cursorRealPosition;
        }
        else
        {
            auto currWindowOpenedFile = std::find(loadedWindows.begin(), loadedWindows.end(), mainWindows[getActiveMainWindow()]->getStorage()->getName());
            loadedWindows.insert(std::next(currWindowOpenedFile), newFileNamePath);
            auto window = addNCursesMainWindow(std::make_unique<utils::BufferFiller>(storageFactory->getStorageWithUtf8Check(newFileNamePath), storageFactory), std::nullopt);
            if (configuration->isHexMode())
            {
                addSecondaryNCursesMainWindowForHexMode(std::make_unique<utils::BufferFiller>(window->getStorage(), storageFactory), window->getUUID());
            }
            replaceActiveWindow(window);
        }
        return pair(0, 0);
    }

    void WindowsManager::hideWindow(utils::datatypes::Uuid const &windowUUID, std::unique_ptr<printer::PrintingVisitorInterface> &visitor)
    {
        auto mainWindow = mainWindows.find(windowUUID);
        auto subWindow = subWindows.find(windowUUID);
        if (mainWindow != mainWindows.end())
        {
            mainWindows[windowUUID]->hide();
        }
        else if (subWindow != subWindows.end())
        {
            if (dynamic_cast<NCursesExitWindowDecorator *>(subWindows[windowUUID].get()))
            {
                setExitWindowVisibility(false);
            }
            else if (dynamic_cast<NCursesShortcutsWindowDecorator *>(subWindows[windowUUID].get()))
            {
                dynamic_cast<NCursesShortcutsWindowDecorator *>(subWindows[windowUUID].get())->resetFirstLineNumber();
            }
            subWindows[windowUUID]->hide();
        }
        refreshAllWindows(visitor);
    }

    void WindowsManager::showWindow(utils::datatypes::Uuid const &windowUUID, std::unique_ptr<printer::PrintingVisitorInterface> &visitor)
    {
        auto mainWindow = mainWindows.find(windowUUID);
        auto subWindow = subWindows.find(windowUUID);
        if (mainWindow != mainWindows.end())
        {
            mainWindows[windowUUID]->show();
        }
        else if (subWindow != subWindows.end())
        {
            subWindows[windowUUID]->show();
        }
        refreshAllWindows(visitor);
        utils::NCursesWrapper::applyRefresh();
    }

    utils::datatypes::Uuid const &WindowsManager::getActiveMainWindow() const
    {
        if (!activeMainWindow.has_value())
        {
            throw utils::exception::FeatherLogicalException("No active windows...");
        }
        return activeMainWindow.value();
    }

    utils::datatypes::Uuid const &WindowsManager::getSecondaryActiveMainWindowForHexMode() const
    {
        for (auto const &e : mainWindows)
        {
            if (dynamic_cast<NCursesMainWindowDecorator *>(e.second.get()) && dynamic_cast<NCursesMainWindowDecorator *>(e.second.get())->isSecondaryHexModeMainWindow())
            {
                return e.first;
            }
        }
        throw utils::exception::FeatherLogicalException("Feather crashed...");
    }

    std::optional<utils::datatypes::Uuid> WindowsManager::getAgreementWindow(utils::datatypes::Uuid const &mainWindowUUID) const
    {
        for (auto s : mainToSubWindows.find(mainWindowUUID)->second)
        {
            if (dynamic_cast<NCursesAgreementWindowDecorator *>(subWindows.find(s)->second.get()))
            {
                return s;
            }
        }
        return std::nullopt;
    }

    std::optional<utils::datatypes::Uuid> WindowsManager::getBottomBarWindow(utils::datatypes::Uuid const &mainWindowUUID) const
    {
        for (auto s : mainToSubWindows.find(mainWindowUUID)->second)
        {
            if (dynamic_cast<NCursesBottomBarWindowDecorator *>(subWindows.find(s)->second.get()))
            {
                return s;
            }
        }
        return std::nullopt;
    }

    std::optional<utils::datatypes::Uuid> WindowsManager::getExitWindow(utils::datatypes::Uuid const &mainWindowUUID) const
    {
        for (auto s : mainToSubWindows.find(mainWindowUUID)->second)
        {
            if (dynamic_cast<NCursesExitWindowDecorator *>(subWindows.find(s)->second.get()))
            {
                return s;
            }
        }
        return std::nullopt;
    }

    std::optional<utils::datatypes::Uuid> WindowsManager::getProgressBarWindow(utils::datatypes::Uuid const &mainWindowUUID) const
    {
        for (auto s : mainToSubWindows.find(mainWindowUUID)->second)
        {
            if (dynamic_cast<NCursesProgressWindowDecorator *>(subWindows.find(s)->second.get()))
            {
                return s;
            }
        }
        return std::nullopt;
    }

    std::optional<utils::datatypes::Uuid> WindowsManager::getShortcutsWindow(utils::datatypes::Uuid const &mainWindowUUID) const
    {
        for (auto s : mainToSubWindows.find(mainWindowUUID)->second)
        {
            if (dynamic_cast<NCursesShortcutsWindowDecorator *>(subWindows.find(s)->second.get()))
            {
                return s;
            }
        }
        return std::nullopt;
    }

    std::optional<std::string> WindowsManager::getNexWindowToLoad(std::string const &activeWindowFile, std::shared_ptr<utils::Filesystem> filesystem)
    {
        size_t noWindowsLoaded = loadedWindows.size();
        if (1UL == noWindowsLoaded)
        {
            return loadedWindows.front();
        }

        auto currWindow = std::find(loadedWindows.begin(), loadedWindows.end(), activeWindowFile);
        while (noWindowsLoaded--)
        {
            if (std::next(currWindow) == loadedWindows.end())
            {
                currWindow = loadedWindows.begin();
            }
            else
            {
                std::advance(currWindow, 1);
            }

            if (filesystem->isFileExist(*currWindow))
            {
                return *currWindow;
            }
        }
        return std::nullopt;
    }

    std::optional<std::string> WindowsManager::getPrevWindowToLoad(std::string const &activeWindowFile, std::shared_ptr<utils::Filesystem> filesystem)
    {
        size_t noWindowsLoaded = loadedWindows.size();
        if (1UL == noWindowsLoaded)
        {
            return loadedWindows.front();
        }

        auto currWindow = std::find(loadedWindows.begin(), loadedWindows.end(), activeWindowFile);
        while (noWindowsLoaded--)
        {
            if (currWindow == loadedWindows.begin())
            {
                currWindow = std::prev(loadedWindows.end());
            }
            else
            {
                std::advance(currWindow, -1);
            }

            if (filesystem->isFileExist(*currWindow))
            {
                return *currWindow;
            }
        }
        return std::nullopt;
    }

    void WindowsManager::recreateWindows()
    {
        for (auto &w : mainWindows)
        {
            w.second->getLibraryWindowHandler()->destroyDisplayedWindow();
        }

        for (auto &w : subWindows)
        {
            w.second->getLibraryWindowHandler()->destroyDisplayedWindow();
        }

        // Recreate windows
        utils::NCursesWrapper::initNCurses();
        for (auto &w : mainWindows)
        {
            auto dimensions = (w.first == activeMainWindow) ? windowsFactory->getMainWindowDimensions() : windowsFactory->getSecondaryHexModeMainWindowDimensions();
            w.second->getLibraryWindowHandler()->switchWrapper(new utils::NCursesWrapper(dimensions, windowsFactory->getLog()));
        }

        for (auto &w : subWindows)
        {
            if (dynamic_cast<NCursesBottomBarWindowDecorator *>(w.second.get()))
            {
                auto dimensions = windowsFactory->getBottomBarWidowDimensions();
                w.second->getLibraryWindowHandler()->switchWrapper(new utils::NCursesWrapper(dimensions, windowsFactory->getLog()));
            }
            else if (dynamic_cast<NCursesExitWindowDecorator *>(w.second.get()))
            {
                auto dimensions = windowsFactory->getExitWidowDimensions();
                w.second->getLibraryWindowHandler()->switchWrapper(new utils::NCursesWrapper(dimensions, windowsFactory->getLog()));
            }
            else if (dynamic_cast<NCursesInfoWindowDecorator *>(w.second.get()))
            {
                auto dimensions = windowsFactory->getInfoWidowDimensions();
                w.second->getLibraryWindowHandler()->switchWrapper(new utils::NCursesWrapper(dimensions, windowsFactory->getLog()));
            }
            else if (dynamic_cast<NCursesProgressWindowDecorator *>(w.second.get()))
            {
                auto dimensions = windowsFactory->getProgressWidowDimensions();
                w.second->getLibraryWindowHandler()->switchWrapper(new utils::NCursesWrapper(dimensions, windowsFactory->getLog()));
            }
            else if (dynamic_cast<NCursesShortcutsWindowDecorator *>(w.second.get()))
            {
                auto dimensions = windowsFactory->getShortcutsWidowDimensions();
                w.second->getLibraryWindowHandler()->switchWrapper(new utils::NCursesWrapper(dimensions, windowsFactory->getLog()));
            }
            else if (dynamic_cast<NCursesAgreementWindowDecorator *>(w.second.get()))
            {
                auto dimensions = windowsFactory->getAgreementWindowDimensions();
                w.second->getLibraryWindowHandler()->switchWrapper(new utils::NCursesWrapper(dimensions, windowsFactory->getLog()));
            }
            else if (dynamic_cast<NCursesTopWindowDecorator *>(w.second.get()))
            {
                auto dimensions = windowsFactory->getTopWidowDimensions();
                w.second->getLibraryWindowHandler()->switchWrapper(new utils::NCursesWrapper(dimensions, windowsFactory->getLog()));
            }
        }
    }

    void WindowsManager::refreshWindowWithItsDependants(utils::datatypes::Uuid const &mainWindowUUID, utils::datatypes::Uuid const &bottomBarUUID, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, pair oldCursorPosition, std::unique_ptr<printer::PrintingVisitorInterface> &visitor, utils::FEATHER_MODE const &currentFeatherMode)
    {
        for (auto &w : mainWindows)
        {
            w.second->refreshBuffer(visitor.get());
        }
        std::unique_ptr<printer::PrintingVisitorInterface> currFrameVisitor = std::make_unique<printer::CurrFrameVisitor>(printingOrchestrator);
        for (auto &subWindow : mainToSubWindows[mainWindowUUID])
        {
            if (!dynamic_cast<NCursesShortcutsWindowDecorator *>(subWindows[subWindow].get()) && !dynamic_cast<NCursesAgreementWindowDecorator *>(subWindows[subWindow].get()))
            {
                if (dynamic_cast<NCursesExitWindowDecorator *>(subWindows[subWindow].get()) && isExitWindowVisible)
                {
                    if (isExitWindowVisible)
                    {
                        subWindows[subWindow]->refreshBuffer(currFrameVisitor.get(), std::nullopt);
                    }
                }
                else if (dynamic_cast<NCursesProgressWindowDecorator *>(subWindows[subWindow].get()))
                {
                    subWindows[subWindow]->refreshBuffer(currFrameVisitor.get(), std::make_pair(mainWindows[mainWindowUUID]->getFramePosition(), oldCursorPosition));
                }
                else
                {
                    subWindows[subWindow]->refreshBuffer(currFrameVisitor.get(), std::nullopt);
                }
            }
        }
        // mainWindows[mainWindowUUID]->refreshBuffer(visitor.get());
        if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode || utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode || utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode || utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_MODE == currentFeatherMode || utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_UP_MODE == currentFeatherMode || utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE == currentFeatherMode)
        {
            mainWindows[mainWindowUUID]->setCursorFromRealPosition(oldCursorPosition, printingOrchestrator, visitor);
            mainWindows[mainWindowUUID]->enableCursor();
        }
        else if (utils::FEATHER_MODE::ACTION_MODE == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_COLOR == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_SAVE == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_UNCOLOR == currentFeatherMode || utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
        {
            size_t newWindowRows = mainWindows[mainWindowUUID]->getWindowDimensions().first;
            mainWindows[mainWindowUUID]->setCursorFromRealPosition(pair(newWindowRows, oldCursorPosition.second), printingOrchestrator, currFrameVisitor);
            subWindows[bottomBarUUID]->enableCursor();
        }
    }

    void WindowsManager::refreshAllWindows(std::unique_ptr<printer::PrintingVisitorInterface> &&visitor, std::set<utils::datatypes::Uuid> skipRefresh)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        std::pair<utils::datatypes::Uuid, bool> windowWithEnabledCursor = std::make_pair(utils::datatypes::Uuid(""), false);
        // Refresh main windows
        for (auto &mainWindow : mainWindows)
        {
            if (mainWindow.second->isSecondaryHexModeMainWindow())
            {
                continue;
            }
            mainWindow.second->refreshBuffer(visitor.get());
            if (mainWindow.second->getLibraryWindowHandler()->isCursorEnabled())
            {
                windowWithEnabledCursor = std::make_pair(mainWindow.first, true);
            }
        }
        // Refresh secondary main windows
        for (auto &mainWindow : mainWindows)
        {
            if (mainWindow.second->isSecondaryHexModeMainWindow())
            {
                mainWindows[mainWindow.first]->refreshBuffer(visitor.get());
            }
        }

        for (auto &subWindow : subWindows)
        {
            if (skipRefresh.count(subWindow.first))
            {
                continue;
            }

            if (subWindow.second->getLibraryWindowHandler()->isCursorEnabled())
            {
                windowWithEnabledCursor = std::make_pair(subWindow.first, false);
            }
            subWindow.second->refreshBuffer(visitor.get());
        }

        // Last window refreshed should be the one with cursor
        if (windowWithEnabledCursor.second)
        {
            mainWindows[windowWithEnabledCursor.first]->refreshBuffer(visitor.get());
            mainWindows[windowWithEnabledCursor.first]->enableCursor();
        }
        else
        {
            subWindows[windowWithEnabledCursor.first]->refreshBuffer(visitor.get());
            subWindows[windowWithEnabledCursor.first]->enableCursor();
        }
    }

    void WindowsManager::refreshAllWindows(std::unique_ptr<printer::PrintingVisitorInterface> &visitor, std::set<utils::datatypes::Uuid> skipRefresh)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        std::optional<std::pair<utils::datatypes::Uuid, bool>> windowWithEnabledCursor = std::nullopt;
        for (auto &mainWindow : mainWindows)
        {
            if (mainWindow.second->isSecondaryHexModeMainWindow())
            {
                continue;
            }
            if (mainWindow.second->getLibraryWindowHandler()->isCursorEnabled())
            {
                windowWithEnabledCursor = std::make_pair(mainWindow.first, true);
            }
            mainWindow.second->refreshBuffer(visitor.get());
        }

        for (auto &mainWindow : mainWindows)
        {
            if (mainWindow.second->isSecondaryHexModeMainWindow())
            {
                mainWindows[mainWindow.first]->refreshBuffer(visitor.get());
            }
        }

        /* Always refresh bottom bar before progress bar window */
        bool bottomBarWindowRefreshed = false;
        windows::SubWindowInterface *progressBarSubWindow = nullptr;
        for (auto &subWindow : subWindows)
        {
            if (skipRefresh.count(subWindow.first))
            {
                continue;
            }
            if (!bottomBarWindowRefreshed && dynamic_cast<windows::NCursesProgressWindowDecorator *>(subWindow.second.get()))
            {
                progressBarSubWindow = subWindow.second.get();
                continue;
            }
            if (subWindow.second->getLibraryWindowHandler()->isCursorEnabled())
            {
                windowWithEnabledCursor = std::make_pair(subWindow.first, false);
            }
            if (dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(subWindow.second.get()))
            {
                bottomBarWindowRefreshed = true;
            }
            subWindow.second->refreshBuffer(visitor.get());
        }
        if (nullptr != progressBarSubWindow && !skipRefresh.count(progressBarSubWindow->getUUID()))
        {
            progressBarSubWindow->refreshBuffer(visitor.get());
        }
        // Last window refreshed should be the one with cursor
        if (windowWithEnabledCursor.has_value() && windowWithEnabledCursor->second)
        {
            mainWindows[windowWithEnabledCursor->first]->setCursorPosition(mainWindows[windowWithEnabledCursor->first]->getCursorPosition());
            mainWindows[windowWithEnabledCursor->first]->enableCursor();
        }
        else if (windowWithEnabledCursor.has_value())
        {
            subWindows[windowWithEnabledCursor->first]->getLibraryWindowHandler()->setCursorAtPos(subWindows[windowWithEnabledCursor->first]->getLibraryWindowHandler()->getCursorPosition());
            subWindows[windowWithEnabledCursor->first]->enableCursor();
        }
        utils::NCursesWrapper::applyRefresh();
    }

    void WindowsManager::refreshExitWindow(std::unique_ptr<printer::PrintingVisitorInterface> &visitor, utils::datatypes::Uuid const &mainWindowUUID, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }

        for (auto &subWindow : mainToSubWindows[mainWindowUUID])
        {
            if (dynamic_cast<NCursesExitWindowDecorator *>(subWindows[subWindow].get()))
            {
                subWindows[subWindow]->refreshBuffer(visitor.get(), arbitraryPosition);
                return;
            }
        }
        (*log)(utils::logger::ERROR, "Tried to refresh help window for main window", mainWindowUUID.getString(), "but not found");
    }

    void WindowsManager::refreshInfoWindow(std::unique_ptr<printer::PrintingVisitorInterface> &visitor, utils::datatypes::Uuid const &mainWindowUUID, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }

        for (auto &subWindow : mainToSubWindows[mainWindowUUID])
        {
            if (dynamic_cast<NCursesInfoWindowDecorator *>(subWindows[subWindow].get()))
            {
                subWindows[subWindow]->refreshBuffer(visitor.get(), arbitraryPosition);
                return;
            }
        }
        (*log)(utils::logger::ERROR, "Tried to refresh help window for main window", mainWindowUUID.getString(), "but not found");
    }

    void WindowsManager::refreshMainWindow(std::unique_ptr<printer::PrintingVisitorInterface> &visitor, utils::datatypes::Uuid const &mainWindowUUID)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }

        if (mainWindows.count(mainWindowUUID))
        {
            mainWindows[mainWindowUUID]->refreshBuffer(visitor.get());
            return;
        }
        (*log)(utils::logger::ERROR, "Tried to refresh main window", mainWindowUUID.getString(), "but not found");
    }

    void WindowsManager::refreshProgresWindow(std::unique_ptr<printer::PrintingVisitorInterface> &visitor, utils::datatypes::Uuid const &mainWindowUUID, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }

        for (auto &subWindow : mainToSubWindows[mainWindowUUID])
        {
            if (dynamic_cast<NCursesProgressWindowDecorator *>(subWindows[subWindow].get()))
            {
                subWindows[subWindow]->refreshBuffer(visitor.get(), arbitraryPosition);
                return;
            }
        }
        (*log)(utils::logger::ERROR, "Tried to refresh progress for main window", mainWindowUUID.getString(), "but not found");
    }

    void WindowsManager::refreshShortcutsWindow(std::unique_ptr<printer::PrintingVisitorInterface> &visitor, utils::datatypes::Uuid const &mainWindowUUID, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }

        for (auto &subWindow : mainToSubWindows[mainWindowUUID])
        {
            if (dynamic_cast<NCursesShortcutsWindowDecorator *>(subWindows[subWindow].get()))
            {
                subWindows[subWindow]->refreshBuffer(visitor.get(), arbitraryPosition);
                return;
            }
        }
    }

    std::shared_ptr<MainWindowInterface> WindowsManager::addMainWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        return mainWindows.insert(std::make_pair(mainWindow->getUUID(true), mainWindow)).first->second;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::addSubWindow(std::unique_ptr<SubWindowInterface> subWindow, utils::datatypes::Uuid const &mainWindowUUID)
    {
        auto subWindowUUID = subWindow->getUUID();
        auto &newSubWindow = subWindows.emplace(subWindowUUID, std::move(subWindow)).first->second;
        if (mainToSubWindows.find(mainWindowUUID) == mainToSubWindows.end())
        {
            mainToSubWindows.emplace(mainWindowUUID, std::set<utils::datatypes::Uuid>{subWindowUUID});
        }
        else
        {
            mainToSubWindows[mainWindowUUID].emplace(subWindowUUID);
        }
        return newSubWindow;
    }

    std::unique_ptr<SubWindowInterface> &WindowsManager::getSubWindowById(utils::datatypes::Uuid const &uuid)
    {
        auto windowIter = subWindows.find(uuid);
        if (windowIter == subWindows.end())
        {
            throw utils::exception::FeatherLogicalException("No window, with id", uuid.getString());
        }
        return windowIter->second;
    }

    void WindowsManager::setActiveMainWindow(utils::datatypes::Uuid const &newActiveWindow)
    {
        activeMainWindow = newActiveWindow;
    }

    void WindowsManager::setExitWindowVisibility(bool visibility)
    {
        isExitWindowVisible = visibility;
    }

    WindowsManager::WindowsManager(WindowsAbstractFactory *window, std::shared_ptr<utils::logger::policy::FileLogPolicy> logArg)
        : activeMainWindow(std::nullopt), windowsFactory(window), log(logArg)
    {
        utils::NCursesWrapper::initNCurses();
    }

    WindowsManager::WindowsManager(WindowsAbstractFactory *window, std::shared_ptr<utils::logger::policy::MemoryLogPolicy> logArg)
        : activeMainWindow(std::nullopt), windowsFactory(window), logMemory(logArg)
    {
        utils::NCursesWrapper::initNCurses();
    }

    WindowsManager::WindowsManager(WindowsAbstractFactory *window, std::shared_ptr<utils::logger::policy::OutputDeviceLogPolicy> logArg)
        : activeMainWindow(std::nullopt), windowsFactory(window), logOutput(logArg)
    {
        utils::NCursesWrapper::initNCurses();
    }

    WindowsManager::~WindowsManager()
    {
        auto filesystem = std::make_unique<utils::Filesystem>();
        for (auto const &mainWindow : mainWindows)
        {
            auto folderName = configuration->getStorageDirectory() + mainWindow.first.getString();
            if (filesystem->isFolderExist(folderName) && filesystem->isFolderEmpty(folderName))
            {
                filesystem->removeDir(folderName);
            }
        }
    }
} // namespace feather::windows
