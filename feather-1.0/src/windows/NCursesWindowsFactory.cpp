#include "utils/NCursesWrapper.hpp"
#include "windows/NCursesAgreementWindowDecorator.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"
#include "windows/NCursesExitWindowDecorator.hpp"
#include "windows/NCursesInfoWindowDecorator.hpp"
#include "windows/NCursesMainWindowDecorator.hpp"
#include "windows/NCursesProgressWindowDecorator.hpp"
#include "windows/NCursesShortcutsWindowDecorator.hpp"
#include "windows/NCursesTopWindowDecorator.hpp"
#include "windows/NCursesWindow.hpp"
#include "windows/NCursesWindowsFactory.hpp"

namespace feather::windows
{
    WindowsAbstractFactory *NCursesWindowsFactory::getInstance(std::shared_ptr<utils::logger::policy::FileLogPolicy> log)
    {
        return new NCursesWindowsFactory(log);
    }

    std::unique_ptr<SubWindowInterface> NCursesWindowsFactory::createAgreementWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto dimensions = getAgreementWindowDimensions();
        return std::make_unique<NCursesAgreementWindowDecorator>(mainWindow, new NCursesWindow(new utils::NCursesWrapper(dimensions, log)));
    }

    std::pair<pair, pair> NCursesWindowsFactory::getAgreementWindowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        auto leftTopCorner = pair(screenDimensions.first / 2UL - AGREEMENT_WINDOW_HEIGHT / 2UL, screenDimensions.second / 2UL - AGREEMENT_WINDOW_WIDTH / 2UL);
        auto dimensions = pair(AGREEMENT_WINDOW_HEIGHT, AGREEMENT_WINDOW_WIDTH);
        return std::make_pair(leftTopCorner, dimensions);
    }

    std::shared_ptr<MainWindowInterface> NCursesWindowsFactory::createMainWindow(std::unique_ptr<utils::BufferFillerInterface> bufferFiller, std::optional<utils::datatypes::Uuid> const &existingWindowUUID)
    {
        auto dimensions = getMainWindowDimensions();
        return std::make_shared<NCursesMainWindowDecorator>(std::move(bufferFiller), new NCursesWindow(new utils::NCursesWrapper(dimensions, log), existingWindowUUID), false);
    }

    std::pair<pair, pair> NCursesWindowsFactory::getMainWindowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        auto hexModeScreenSplitRatio = utils::NCursesWrapper::getHexWindowsScreenSplitRatio();
        //first - row, second- column
        pair leftTopCorner = pair(1UL, 0);
        //first - height, second - width
        auto dimensions = pair(screenDimensions.first - 3UL, configuration->isHexMode() ? hexModeScreenSplitRatio.first : screenDimensions.second);
        return std::make_pair(leftTopCorner, dimensions);
    }

    std::unique_ptr<MainWindowInterface> NCursesWindowsFactory::createSecondaryHexModeMainWindow(std::unique_ptr<utils::BufferFillerInterface> bufferFiller, std::optional<utils::datatypes::Uuid> const &siblingUUID)
    {
        auto dimensions = getSecondaryHexModeMainWindowDimensions();
        return std::make_unique<NCursesMainWindowDecorator>(std::move(bufferFiller), new NCursesWindow(new utils::NCursesWrapper(dimensions, log)), true, siblingUUID);
    }

    std::pair<pair, pair> NCursesWindowsFactory::getSecondaryHexModeMainWindowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        //first - height, second - width
        auto hexModeScreenSplitRatio = utils::NCursesWrapper::getHexWindowsScreenSplitRatio();
        //first - row, second - column
        auto leftTopCorner = pair(1UL, hexModeScreenSplitRatio.first + hexModeScreenSplitRatio.second);
        //first - height, second - width
        auto dimensions = pair(screenDimensions.first - 3UL, screenDimensions.second - (hexModeScreenSplitRatio.first + hexModeScreenSplitRatio.second));
        return std::make_pair(leftTopCorner, dimensions);
    }

    std::unique_ptr<SubWindowInterface> NCursesWindowsFactory::createBottomBarWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto dimensions = getBottomBarWidowDimensions();
        return std::make_unique<NCursesBottomBarWindowDecorator>(mainWindow, new NCursesWindow(new utils::NCursesWrapper(dimensions, log)));
    }

    std::pair<pair, pair> NCursesWindowsFactory::getBottomBarWidowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        auto leftTopCorner = pair(screenDimensions.first - 1UL, 0);
        auto dimensions = pair(1UL, screenDimensions.second);
        return std::make_pair(leftTopCorner, dimensions);
    }

    std::unique_ptr<SubWindowInterface> NCursesWindowsFactory::createExitWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto dimensions = getExitWidowDimensions();
        return std::make_unique<NCursesExitWindowDecorator>(mainWindow, new NCursesWindow(new utils::NCursesWrapper(dimensions, log)));
    }

    std::pair<pair, pair> NCursesWindowsFactory::getExitWidowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        auto leftTopCorner = pair(screenDimensions.first / 2UL - EXIT_WINDOW_HEIGHT / 2UL, screenDimensions.second / 2UL - EXIT_WINDOW_WIDTH / 2UL);
        auto dimensions = pair(EXIT_WINDOW_HEIGHT, EXIT_WINDOW_WIDTH);
        return std::make_pair(leftTopCorner, dimensions);
    }

    std::unique_ptr<SubWindowInterface> NCursesWindowsFactory::createInfoWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto dimensions = getInfoWidowDimensions();
        return std::make_unique<NCursesInfoWindowDecorator>(mainWindow, new NCursesWindow(new utils::NCursesWrapper(dimensions, log)));
    }

    std::pair<pair, pair> NCursesWindowsFactory::getInfoWidowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        auto leftTopCorner = pair(screenDimensions.first - 2UL, 0);
        auto dimensions = pair(1UL, screenDimensions.second);
        return make_pair(leftTopCorner, dimensions);
    }

    std::unique_ptr<SubWindowInterface> NCursesWindowsFactory::createProgressWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto dimensions = getProgressWidowDimensions();
        return std::make_unique<NCursesProgressWindowDecorator>(mainWindow, new NCursesWindow(new utils::NCursesWrapper(dimensions, log)));
    }

    std::pair<pair, pair> NCursesWindowsFactory::getProgressWidowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        auto leftTopCorner = pair(screenDimensions.first - 1UL, screenDimensions.second - PROGRESS_WINDOW_WIDTH);
        auto dimensions = pair(PROGRESS_WINDOW_HEIGHT, PROGRESS_WINDOW_WIDTH);
        return make_pair(leftTopCorner, dimensions);
    }

    std::unique_ptr<SubWindowInterface> NCursesWindowsFactory::createShortcutsWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto dimensions = getShortcutsWidowDimensions();
        return std::make_unique<NCursesShortcutsWindowDecorator>(mainWindow, new NCursesWindow(new utils::NCursesWrapper(dimensions, log)));
    }

    std::pair<pair, pair> NCursesWindowsFactory::getShortcutsWidowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        auto leftTopCorner = pair(1UL, screenDimensions.second - SHORTCUTS_WINDOW_WIDTH);
        auto dimensions = pair(screenDimensions.first - 3UL, SHORTCUTS_WINDOW_WIDTH);
        return make_pair(leftTopCorner, dimensions);
    }

    std::unique_ptr<SubWindowInterface> NCursesWindowsFactory::createTopWindow(std::shared_ptr<MainWindowInterface> mainWindow)
    {
        auto dimensions = getTopWidowDimensions();
        return std::make_unique<NCursesTopWindowDecorator>(mainWindow, new NCursesWindow(new utils::NCursesWrapper(dimensions, log)));
    }

    std::pair<pair, pair> NCursesWindowsFactory::getTopWidowDimensions()
    {
        screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        if (screenDimensions.second < MIN_ACCEPTABLE_WINDOW_WIDTH || screenDimensions.first < MIN_ACCEPTABLE_WINDOW_HEIGHT)
        {
            return std::make_pair(pair(0, 0), pair(0, 0));
        }
        auto leftTopCorner = pair(0, 0);
        auto dimensions = pair(1UL, screenDimensions.second);
        return std::make_pair(leftTopCorner, dimensions);
    }

    std::shared_ptr<utils::logger::policy::FileLogPolicy> NCursesWindowsFactory::getLog()
    {
        return log;
    }

    NCursesWindowsFactory::~NCursesWindowsFactory() = default;
} // namespace feather::windows