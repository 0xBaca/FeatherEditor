#include "utils/datatypes/Strings.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"

namespace feather::windows
{
    NCursesBottomBarWindowDecorator::NCursesBottomBarWindowDecorator(std::shared_ptr<MainWindowInterface> mainWindowArg, WindowImplInterface *nCursesLibraryWindowArg)
        : SubWindowInterface(mainWindowArg, nCursesLibraryWindowArg), currentReader(nullptr)
    {
        actionHistoryIterator = actionHistory.cend();
        searchHistoryIterator = searchHistory.cend();
        searchHexHistoryIterator = searchHexHistory.cend();
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::ADDED_BREAKPOINT, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREEN})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::CACHE_NOTHING_TO_STASH, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::CACHE_STASHED, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREEN})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::COPIED_LINE, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREEN})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::COPIED_TEXT, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREEN})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::CUT_TEXT, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREEN})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::DISK_NO_SPACE_LEFT, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::DROPPPED_ALL_BREAKPOINTS, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREEN})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::FILE_ALREADY_EXIST, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::FILE_CONTENT_CHANGED, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::FILE_DELETED_DURING_SAVING, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::FILE_DOES_NOT_EXIST, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::FILE_IS_FOLDER, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::FILE_WAS_MODIFIED, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::FOLDER_DOES_NOT_EXIST, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::FOLDER_NO_PERMISSIONS_TO_WRITE, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::INTERRUPTED, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::INVALID_HEX_STRING, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::INVALID_REGEX, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::NOT_ENOUGH_ARGUMENTS, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::NOT_ENOUGH_OR_WRONG_ARGUMENTS, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::NOT_ENOUGH_PERMISSIONS, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::NO_BREAKPOINTS_SET, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::SAVE_SUCCESS, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREEN})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::UNRECOGNIZED_COMMAND, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
        highlightedStrings.insert(std::make_pair(utils::datatypes::Strings::UNSUPPORTED_COLOR, std::make_pair(0, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED})));
    }

    bool NCursesBottomBarWindowDecorator::isActionHistoryEmpty() const
    {
        return actionHistory.empty();
    }

    bool NCursesBottomBarWindowDecorator::isActionHistoryIteratorAtBegining() const
    {
        return actionHistoryIterator == getActionHistoryBegin();
    }

    bool NCursesBottomBarWindowDecorator::isActionHistoryIteratorAtLastPos() const
    {
        return actionHistoryIterator == getActionHistoryEnd();
    }

    bool NCursesBottomBarWindowDecorator::isSearchHistoryEmpty() const
    {
        return searchHistory.empty();
    }

    bool NCursesBottomBarWindowDecorator::isSearchHistoryIteratorAtBegining() const
    {
        return searchHistoryIterator == getSearchHistoryBegin();
    }

    bool NCursesBottomBarWindowDecorator::isSearchHistoryIteratorAtLastPos() const
    {
        return searchHistoryIterator == getSearchHistoryEnd();
    }

    bool NCursesBottomBarWindowDecorator::isSearchHexHistoryEmpty() const
    {
        return searchHexHistory.empty();
    }

    bool NCursesBottomBarWindowDecorator::isSearchHexHistoryIteratorAtBegining() const
    {
        return searchHexHistoryIterator == getSearchHexHistoryBegin();
    }

    bool NCursesBottomBarWindowDecorator::isSearchHexHistoryIteratorAtLastPos() const
    {
        return searchHexHistoryIterator == getSearchHexHistoryEnd();
    }

    void NCursesBottomBarWindowDecorator::addToActionHistory(std::u32string &&entry)
    {
        actionHistory.emplace_back(std::move(entry));
        if (1UL == actionHistory.size())
        {
            actionHistoryIterator = actionHistory.cbegin();
        }
    }

    void NCursesBottomBarWindowDecorator::addToSearchHistory(std::u32string &&entry)
    {
        searchHistory.emplace_back(std::move(entry));
        if (1UL == searchHistory.size())
        {
            searchHistoryIterator = searchHistory.cbegin();
        }
    }

    void NCursesBottomBarWindowDecorator::addToSearchHexHistory(std::u32string &&entry)
    {
        searchHexHistory.emplace_back(std::move(entry));
        if (1UL == searchHexHistory.size())
        {
            searchHexHistoryIterator = searchHexHistory.cbegin();
        }
    }

    void NCursesBottomBarWindowDecorator::print(utils::ScreenBuffer const &newBuffer)
    {
        size_t windowDimWithoutProgressWindow = utils::NCursesWrapper::getScreenDimenstions().second - feather::windows::WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH;
        size_t tailoredBufferLength = 0ULL;
        for (char32_t c : newBuffer[0])
        {
            tailoredBufferLength += utils::helpers::Lambda::getCharacterWidth(c, false, false);
        }
        if (tailoredBufferLength >= windowDimWithoutProgressWindow - 2UL)
        {
            tailoredBufferLength = 0;
            auto currIdx = newBuffer[0].begin();
            while (currIdx != newBuffer[0].end())
            {
                auto charWidth = utils::helpers::Lambda::getCharacterWidth(*currIdx, false, false);
                if (tailoredBufferLength + charWidth > windowDimWithoutProgressWindow - 2UL)
                {
                    std::advance(currIdx, -1);
                    break;
                }
                tailoredBufferLength += charWidth;
                ++currIdx;
            }
            utils::ScreenBuffer tailoredBuffer({newBuffer[0].cbegin(), currIdx}, 1);
            utils::ScreenBuffer decoratedBuffer(tailoredBuffer.getFilledRows(), tailoredBuffer.getFilledRows(), windowDimWithoutProgressWindow - 2UL);
            decoratedBuffer.decorateAsBottomBarWindowBuffer(tailoredBuffer);
 
            if (configuration->isTerminalSupportColors())
            {
                decoratedBuffer.applyColors(highlightedStrings);
            }
            nCursesLibraryWindow->print(decoratedBuffer, false);
        }
        else
        {
            utils::ScreenBuffer tailoredBuffer({newBuffer[0].begin(), newBuffer[0].end()}, 1);
            utils::ScreenBuffer decoratedBuffer(tailoredBuffer.getFilledRows(), tailoredBuffer.getFilledRows(), windowDimWithoutProgressWindow - 2UL);
            decoratedBuffer.decorateAsBottomBarWindowBuffer(tailoredBuffer);
            if (configuration->isTerminalSupportColors())
            {
                decoratedBuffer.applyColors(highlightedStrings);
            }
            nCursesLibraryWindow->print(decoratedBuffer, false);
        }
        setCursor(std::make_pair(0, tailoredBufferLength));
    }

    void NCursesBottomBarWindowDecorator::refreshBuffer(printer::PrintingVisitorInterface *visitor, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        visitor->visit(this, arbitraryPosition);
    }

    void NCursesBottomBarWindowDecorator::setReader(reader::Reader *readerArg)
    {
        currentReader = readerArg;
    }

    void NCursesBottomBarWindowDecorator::setActionHistoryIteratorAtEnd()
    {
        actionHistoryIterator = getActionHistoryEnd();
    }

    void NCursesBottomBarWindowDecorator::setSearchHistoryIteratorAtEnd()
    {
        searchHistoryIterator = getSearchHistoryEnd();
    }

    void NCursesBottomBarWindowDecorator::setSearchHexHistoryIteratorAtEnd()
    {
        searchHexHistoryIterator = getSearchHexHistoryEnd();
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getCurrFromActionHistory()
    {
        if (!actionHistory.empty() && !isActionHistoryIteratorAtLastPos())
        {
            return std::optional<std::u32string>(*actionHistoryIterator);
        }
        return std::nullopt;
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getCurrFromSearchHistory()
    {
        if (!searchHistory.empty() && !isSearchHistoryIteratorAtLastPos())
        {
            return std::optional<std::u32string>(*searchHistoryIterator);
        }
        return std::nullopt;
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getCurrFromSearchHexHistory()
    {
        if (!searchHexHistory.empty() && !isSearchHexHistoryIteratorAtLastPos())
        {
            return std::optional<std::u32string>(*searchHexHistoryIterator);
        }
        return std::nullopt;
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getNextFromActionHistory()
    {
        if (actionHistory.empty())
        {
            return std::nullopt;
        }
        else if (actionHistoryIterator != actionHistory.cend() && (std::next(actionHistoryIterator) != actionHistory.cend()))
        {
            std::advance(actionHistoryIterator, 1);
        }
        return getCurrFromActionHistory();
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getNextFromSearchHistory()
    {
        if (searchHistory.empty())
        {
            return std::nullopt;
        }
        else if (searchHistoryIterator != searchHistory.cend() && (std::next(searchHistoryIterator) != searchHistory.cend()))
        {
            std::advance(searchHistoryIterator, 1);
        }
        return getCurrFromSearchHistory();
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getNextFromSearchHexHistory()
    {
        if (searchHexHistory.empty())
        {
            return std::nullopt;
        }
        else if (searchHexHistoryIterator != searchHexHistory.cend() && (std::next(searchHexHistoryIterator) != searchHexHistory.cend()))
        {
            std::advance(searchHexHistoryIterator, 1);
        }
        return getCurrFromSearchHexHistory();
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getPreviousFromActionHistory()
    {
        if (actionHistory.empty())
        {
            return std::nullopt;
        }
        else if (actionHistoryIterator != actionHistory.cbegin())
        {
            std::advance(actionHistoryIterator, -1);
        }
        return getCurrFromActionHistory();
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getPreviousFromSearchHistory()
    {
        if (searchHistory.empty())
        {
            return std::nullopt;
        }
        else if (searchHistoryIterator != searchHistory.cbegin())
        {
            std::advance(searchHistoryIterator, -1);
        }
        return getCurrFromSearchHistory();
    }

    std::optional<std::u32string> NCursesBottomBarWindowDecorator::getPreviousFromSearchHexHistory()
    {
        if (searchHexHistory.empty())
        {
            return std::nullopt;
        }
        else if (searchHexHistoryIterator != searchHexHistory.cbegin())
        {
            std::advance(searchHexHistoryIterator, -1);
        }
        return getCurrFromSearchHexHistory();
    }

    std::list<std::u32string>::const_iterator NCursesBottomBarWindowDecorator::getActionHistoryBegin() const
    {
        return actionHistory.cbegin();
    }

    std::list<std::u32string>::const_iterator NCursesBottomBarWindowDecorator::getSearchHistoryBegin() const
    {
        return searchHistory.cbegin();
    }

    std::list<std::u32string>::const_iterator NCursesBottomBarWindowDecorator::getSearchHexHistoryBegin() const
    {
        return searchHexHistory.cbegin();
    }

    std::list<std::u32string>::const_iterator NCursesBottomBarWindowDecorator::getActionHistoryEnd() const
    {
        return actionHistory.cend();
    }

    std::list<std::u32string>::const_iterator NCursesBottomBarWindowDecorator::getSearchHistoryEnd() const
    {
        return searchHistory.cend();
    }

    std::list<std::u32string>::const_iterator NCursesBottomBarWindowDecorator::getSearchHexHistoryEnd() const
    {
        return searchHexHistory.cend();
    }

    reader::Reader const *NCursesBottomBarWindowDecorator::getCurrentReader() const
    {
        return currentReader;
    }

    NCursesBottomBarWindowDecorator::~NCursesBottomBarWindowDecorator()
    {
    }
} // namespace feather::windows
