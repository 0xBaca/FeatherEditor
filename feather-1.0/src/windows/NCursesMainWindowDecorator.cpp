#include "printer/HalfFrameUpVisitor.hpp"
#include "utils/algorithm/RegexSearch.hpp"
#include "utils/MoveCursorOnScreenStateMachine.hpp"
#include "utils/windows/LinesAroundPositionInformation.hpp"

namespace feather::windows
{
    NCursesMainWindowDecorator::NCursesMainWindowDecorator(
        std::unique_ptr<utils::BufferFillerInterface> bufferFillerArg, WindowImplInterface *nCursesLibraryWindowArg, bool isSecondaryHexModeWindowArg, std::optional<utils::datatypes::Uuid> const &siblingUUIDArg)
        : MainWindowInterface(nCursesLibraryWindowArg), siblingUUID(siblingUUIDArg), bufferFiller(std::move(bufferFillerArg)), lastCursorRealPos(0, 0)
    {
        std::srand(std::time(0));
        this->isSecondaryHexModeWindow = isSecondaryHexModeWindowArg;
        this->secondaryWindowCursor = std::make_pair(pair(0, 0), pair(1, 0));
    }

    pair NCursesMainWindowDecorator::getCursorRealPosition(feather::printer::PrintingOrchestratorInterface *printingOrchestrator) const
    {
        pair currRealPosition = bufferFiller->getFramePositions().startFramePosition;
        pair targetScreenCursorOffset = getCursorPosition();
        pair currScreenCursorOffset{0, 0};
        for (size_t screenBufferRow = 0; screenBufferRow < bufferFiller->getScreenBuffer().getFilledRows(); ++screenBufferRow)
        {
            for (size_t screenBufferColumn = 0; screenBufferColumn < bufferFiller->getScreenBuffer()[screenBufferRow].size(); ++screenBufferColumn)
            {
                if (printingOrchestrator->isCharAtPosDeleted(currRealPosition, getUUID()))
                {
                    currRealPosition = printingOrchestrator->getDeletionEnd(getUUID(), currRealPosition);
                }
                char32_t currCharacter = bufferFiller->getScreenBuffer()[screenBufferRow][screenBufferColumn];
                if (currScreenCursorOffset == targetScreenCursorOffset)
                {
                    return lastCursorRealPos = currRealPosition;
                }
                size_t charBytesTaken = utils::Utf8Util::getCharacterBytesTakenLight(currCharacter);
                currScreenCursorOffset.second += utils::helpers::Lambda::getCharacterWidth(currCharacter, true, isSecondaryHexModeWindow);
                currRealPosition = utils::helpers::Lambda::moveVirtualPositionForward(currRealPosition, charBytesTaken, printingOrchestrator, getUUID(), false);
            }
            ++currScreenCursorOffset.first;
            currScreenCursorOffset.second = 0;
        }
        if (printingOrchestrator->isCharAtPosDeleted(currRealPosition, getUUID()))
        {
            currRealPosition = printingOrchestrator->getDeletionEnd(getUUID(), currRealPosition);
        }
        return lastCursorRealPos = currRealPosition;
    }

    pair NCursesMainWindowDecorator::getLastRealPosition() const
    {
        return lastCursorRealPos;
    }

    utils::windows::WindowStateContainer NCursesMainWindowDecorator::getState() const
    {
        return utils::windows::WindowStateContainer{getUUID(), siblingUUID, isSecondaryHexModeWindow, secondaryWindowCursor, getCursorRealPosition(printingOrchestratorWeak.lock().get())};
    }

    utils::windows::FramePositions const &NCursesMainWindowDecorator::getFramePosition() const
    {
        return this->bufferFiller->getFramePositions();
    }

    std::shared_ptr<utils::storage::AbstractStorage> const &NCursesMainWindowDecorator::getStorage() const
    {
        return getBufferFiller()->getStorage();
    }

    std::unique_ptr<utils::BufferFillerInterface> const &NCursesMainWindowDecorator::getBufferFiller() const
    {
        return this->bufferFiller;
    }

    utils::datatypes::Uuid const &NCursesMainWindowDecorator::getUUID(bool getNativeUUID) const
    {
        if (getNativeUUID)
        {
            return nCursesLibraryWindow->getUUID();
        }
        return this->isSecondaryHexModeWindow ? siblingUUID.value() : nCursesLibraryWindow->getUUID();
    }

    std::unique_ptr<utils::windows::WindowInformationInterface> NCursesMainWindowDecorator::accept(std::shared_ptr<utils::windows::WindowInformationVisitorInterface> visitor)
    {
        return visitor->visit(this);
    }

    void NCursesMainWindowDecorator::showSecondaryWindowCursor(std::pair<pair, pair> const &pos, std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestrator)
    {
        this->secondaryWindowCursor = utils::helpers::Lambda::getScreenBufferPositions(pos, printingOrchestrator, getBufferFiller(), getUUID(), getWindowDimensions().second, isSecondaryHexModeWindow);
    }

    void NCursesMainWindowDecorator::moveCursor(feather::utils::Direction direction, std::shared_ptr<utils::windows::WindowInformationVisitorInterface> visitor, std::optional<size_t> lastLineUpSize)
    {
        static bool movedToBracket = false;
        auto cursorMoveResult = nCursesLibraryWindow->moveCursor(direction, visitor->visit(this), lastLineUpSize, getBufferFiller()->getScreenBuffer(), isSecondaryHexModeMainWindow());
        if (cursorMoveResult.second)
        {
            movedToBracket = true;
            print(getBufferFiller()->getScreenBuffer());
            setCursorPosition(cursorMoveResult.first);
            getBufferFiller()->getScreenBuffer().unhighlightBrackets();
        }
        else if (movedToBracket)
        {
            movedToBracket = false;
            print(getBufferFiller()->getScreenBuffer());
            setCursorPosition(cursorMoveResult.first);
        }
    }

    void NCursesMainWindowDecorator::print(utils::ScreenBuffer const &screenBuffer)
    {
        utils::ScreenBuffer decoratedBuffer(screenBuffer.getFilledRows(), screenBuffer.getFilledRows(), getWindowDimensions().second);
        decoratedBuffer.decorateBuffer(screenBuffer, isSecondaryHexModeWindow);
        if (auto printingOrchestrator = printingOrchestratorWeak.lock())
        {
            utils::algorithm::RegexSearch regexSearch(decoratedBuffer.getBufferAsString());
            std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedStringsAfterApplyingRegex;
            for (auto const &e : printingOrchestrator->getHighlightedStrings(getUUID(true)))
            {
                for (auto const &re : regexSearch.search(e.first))
                {
                    highlightedStringsAfterApplyingRegex.insert({re, e.second});
                }
            }
            decoratedBuffer.applyColors(highlightedStringsAfterApplyingRegex);
            decoratedBuffer.setHighlightedBrackets(screenBuffer.getHighlightedBrackets());
            std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedTextChunksInCurrentFrame;
            for (auto const &e : printingOrchestrator->getHighlitedTextChunks(getUUID()))
            {
                auto chunkStart = printingOrchestrator->isCharAtPosDeleted(e.first.first, getUUID()) ? printingOrchestrator->getDeletionEnd(getUUID(), e.first.first) : e.first.first;
                auto chunkEnd = printingOrchestrator->isCharAtPosDeleted(e.first.second, getUUID()) ? printingOrchestrator->getDeletionEnd(getUUID(), e.first.second) : e.first.second;
                if (chunkEnd <= getBufferFiller()->getFramePositions().startFramePosition || chunkStart > getBufferFiller()->getFramePositions().endFramePosition || (chunkStart >= chunkEnd))
                {
                    continue;
                }
                std::pair<pair, pair> screenBufferPositions = utils::helpers::Lambda::getScreenBufferPositions(std::make_pair(chunkStart, chunkEnd), printingOrchestrator, getBufferFiller(), getUUID(), getWindowDimensions().second, isSecondaryHexModeWindow);
                highlightedTextChunksInCurrentFrame.insert({screenBufferPositions, e.second});
            }
            if (isSecondaryHexModeWindow)
            {
                highlightedTextChunksInCurrentFrame.insert({this->secondaryWindowCursor, {HEX_WINDOW_CURSOR_COLOR_PRIORITY, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_HEX_WINDOW_CURSOR_COLOR_BCKG}}});
            }
            if (!configuration->isHexMode())
            {
                /*
                auto result = action::GetStringsAction(action::GetStringsActionInput(this)).execute();
                for (auto &e : result.result)
                {
                    highlightedTextChunksInCurrentFrame.insert({e, {STRINGS_COLOR_PRIORITY, std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_STRINGS_HIGHLIGHT_COLOR}}});
                }
                */
            }
            decoratedBuffer.setHighlightedTextChunks(highlightedTextChunksInCurrentFrame);
            if (printingOrchestrator->getTextHighlight(getUUID()).has_value())
            {
                std::pair<pair, pair> screenBufferPositions = utils::helpers::Lambda::getScreenBufferPositions(printingOrchestrator->getTextHighlight(getUUID()).value(), printingOrchestrator, getBufferFiller(), getUUID(), getWindowDimensions().second, isSecondaryHexModeWindow);
                decoratedBuffer.setHighlightedText(screenBufferPositions, printingOrchestrator->getTextHighlightColor(getUUID()));
            }
            if (printingOrchestrator->getSearchedText(getUUID()))
            {
                auto searchedText = printingOrchestrator->getSearchedText(getUUID());
                int searchTextLen = searchedText->second;
                auto searchedTextEnd = searchedText->first;
                while (searchTextLen--)
                {
                    searchedTextEnd = printingOrchestrator->getNextVirtualPosition(getUUID(), getStorage(), searchedTextEnd);
                }
                auto cursorRealPositions = std::make_pair(searchedText->first, searchedTextEnd);
                decoratedBuffer.applyBlinking(utils::helpers::Lambda::getScreenBufferPositions(cursorRealPositions, printingOrchestrator, getBufferFiller(), getUUID(), getWindowDimensions().second, isSecondaryHexModeWindow));
            }
        }
        nCursesLibraryWindow->print(decoratedBuffer, true);
    }

    void NCursesMainWindowDecorator::refreshBuffer(printer::PrintingVisitorInterface *visitor)
    {
        visitor->visit(this);
    }

    void NCursesMainWindowDecorator::setCursorFromRealPosition(pair cursorRealPosition, std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestrator, std::unique_ptr<printer::PrintingVisitorInterface> &visitor, bool wasNewLineCharacterPressed)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        pair lastFramePosition = getBufferFiller()->getFramePositions().endFramePosition;
        if ((printingOrchestrator->getNextVirtualPosition(getUUID(), getStorage(), lastFramePosition) == cursorRealPosition) &&
            utils::helpers::Lambda::getCursorCoordinatesFromRealPosition(cursorRealPosition, printingOrchestrator, getBufferFiller(), getUUID(), getWindowDimensions().second, false, isSecondaryHexModeWindow).first < getWindowDimensions().first - 1UL)
        {
            setCursorPosition(utils::helpers::Lambda::getCursorCoordinatesFromRealPosition(cursorRealPosition, printingOrchestrator, getBufferFiller(), getUUID(), getWindowDimensions().second, wasNewLineCharacterPressed, isSecondaryHexModeWindow));
            return;
        }
        else if (!getBufferFiller()->getFramePositions().isInsideFrame(cursorRealPosition))
        {
            setFramePosition(cursorRealPosition);
            refreshBuffer(visitor.get());
        }
        setCursorPosition(utils::helpers::Lambda::getCursorCoordinatesFromRealPosition(cursorRealPosition, printingOrchestrator, getBufferFiller(), getUUID(), getWindowDimensions().second, wasNewLineCharacterPressed, isSecondaryHexModeWindow));
    }

    void NCursesMainWindowDecorator::setFramePosition(pair frameStart)
    {
        this->bufferFiller->setStartFramePosition(frameStart);
    }

    void NCursesMainWindowDecorator::setState(utils::windows::WindowStateContainer const &windowState)
    {
        this->siblingUUID = windowState.siblingUUID;
        this->isSecondaryHexModeWindow = windowState.isSecondaryHexModeWindow;
        this->secondaryWindowCursor = windowState.secondaryWindowCursor;
    }

    NCursesMainWindowDecorator::~NCursesMainWindowDecorator()
    {
    }
} // namespace feather::windows
