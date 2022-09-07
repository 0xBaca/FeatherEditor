#include "printer/CurrFrameVisitor.hpp"
#include "utils/algorithm/SearchEngineBase.hpp"
#include "utils/datatypes/Strings.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;
extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::utils::algorithm
{
    SearchEngineBase::SearchEngineBase(utils::datatypes::Uuid const &windowUUIDArg, std::u32string &&searchedStringArg, std::shared_ptr<utils::storage::AbstractStorageFactory> fileStorageFactoryArg, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorArg, std::unique_ptr<utils::BufferFillerInterface> const &bufferFillerArg, std::list<pair> &breakPointsArg, std::list<pair>::iterator &currentBreakPointArg, std::set<pair> &breakPointsAddedArg)
#ifdef _FEATHER_TEST_
        : windowUUID(windowUUIDArg), searchedString(std::move(searchedStringArg)), printingOrchestrator(printingOrchestratorArg), toCursorStorage(storageFactoryArg->getStorageWithUtf8Check(bufferFillerArg->getStorage()->getName())), fromCursorStorage(storageFactoryArg->getStorageWithUtf8Check(bufferFillerArg->getStorage()->getName())), secondaryCursorStorage(storageFactoryArg->getStorageWithUtf8Check(bufferFillerArg->getStorage()->getName())), fileStorageFactory(fileStorageFactoryArg), storageFactory(storageFactoryArg), currentFrameVisitor(std::make_unique<printer::CurrFrameVisitor>(printingOrchestratorArg)), bufferFiller(bufferFillerArg), lastPos(bufferFillerArg->getStorage()->getSize()), breakPoints(breakPointsArg), currentBreakPoint(currentBreakPointArg), breakPointsAdded(breakPointsAddedArg)
#else
        : windowUUID(windowUUIDArg), searchedString(std::move(searchedStringArg)), printingOrchestrator(printingOrchestratorArg), toCursorStorage(fileStorageFactoryArg->getStorageWithUtf8Check(bufferFillerArg->getStorage()->getName())), fromCursorStorage(fileStorageFactoryArg->getStorageWithUtf8Check(bufferFillerArg->getStorage()->getName())), secondaryCursorStorage(fileStorageFactoryArg->getStorageWithUtf8Check(bufferFillerArg->getStorage()->getName())), fileStorageFactory(fileStorageFactoryArg), storageFactory(storageFactoryArg), currentFrameVisitor(std::make_unique<printer::CurrFrameVisitor>(printingOrchestratorArg)), bufferFiller(bufferFillerArg), lastPos(bufferFillerArg->getStorage()->getSize()), breakPoints(breakPointsArg), currentBreakPoint(currentBreakPointArg), breakPointsAdded(breakPointsAddedArg)
#endif
    {
        lastAddBreakpointPressed = std::chrono::steady_clock::now();
        searchedStringWithoutLastChar[utils::helpers::Conversion::squeezeu32String(searchedString)] = {searchedString.begin(), std::prev(searchedString.end())};
    }

    void SearchEngineBase::addBreakpoint(std::unique_ptr<feather::windows::SubWindowInterface> &currBottomBarWindow, std::unique_ptr<printer::PrintingVisitorInterface> &halfFrameUpVisitor, std::shared_ptr<feather::windows::WindowsManager> &windowsManager)
    {
        pair cursorPos = currBottomBarWindow->getParentWindowHandler()->getCursorRealPosition(printingOrchestrator.get());
        if (!breakPointsAdded.insert(cursorPos).second)
        {
            return;
        }
        breakPoints.push_back(cursorPos);
        if (breakPoints.cend() == currentBreakPoint)
        {
            currentBreakPoint = breakPoints.begin();
        }
        currBottomBarWindow->getParentWindowHandler()->disableCursor();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastAddBreakpointPressed) > std::chrono::milliseconds(1000))
        {
            utils::helpers::Lambda::blinkPrint(utils::datatypes::Strings::ADDED_BREAKPOINT, (currentFeatherMode == utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE) ? utils::datatypes::Strings::BROWSE_SEARCH_DOWN_MODE : utils::datatypes::Strings::BROWSE_SEARCH_UP_MODE, ONE_SECOND, currBottomBarWindow);
        }
        lastAddBreakpointPressed = std::chrono::steady_clock::now();
        currBottomBarWindow->getParentWindowHandler()->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
        windowsManager->refreshProgresWindow(halfFrameUpVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), cursorPos));
        currBottomBarWindow->getParentWindowHandler()->enableCursor();
    }

    std::pair<std::optional<std::pair<pair, size_t>>, utils::datatypes::ERROR> SearchEngineBase::getNextMatch(std::shared_ptr<feather::windows::WindowsManager> windowsManager, std::unique_ptr<printer::PrintingVisitorInterface> &refreshVisitor, std::unique_ptr<printer::PrintingVisitorInterface> &halfFrameVisitor, std::unique_ptr<feather::windows::SubWindowInterface> &currBottomBarWindow, utils::FEATHER_MODE &currentFeatherMode, utils::Direction searchDirection)
    {
        // INTENTIONALY EMPTY
        return std::make_pair(std::nullopt, utils::datatypes::ERROR::NONE);
    }

    std::pair<pair, size_t> SearchEngineBase::getNextSearchInterval(std::pair<pair, pair> change, std::string searchedString, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID)
    {
        pair lastValidPosition = printingOrchestrator->getLastValidPosition(windowUUID, storage);
        size_t noBytesBetweenChanges = printingOrchestrator->getDiffBytesWithoutDeletions(change.first, change.second, windowUUID);

        // Check how many bytes can be taken
        pair tmpLeftAbsolutePosition = change.first;
        for (size_t idx = 0; idx < MAX_SEARCH_RESULT_DELTA && tmpLeftAbsolutePosition > printingOrchestrator->getFirstValidPosition(windowUUID); ++idx)
        {
            tmpLeftAbsolutePosition = printingOrchestrator->getPreviousVirtualPosition(windowUUID, storage, tmpLeftAbsolutePosition);
        }
        size_t leftSize = printingOrchestrator->getDiffBytesWithoutDeletions(tmpLeftAbsolutePosition, change.first, windowUUID);
        pair tmpRightAbsolutePosition = change.second;
        for (size_t idx = 0; idx < MAX_SEARCH_RESULT_DELTA && tmpRightAbsolutePosition <= lastValidPosition; ++idx)
        {
            tmpRightAbsolutePosition = printingOrchestrator->getNextVirtualPosition(windowUUID, storage, tmpRightAbsolutePosition);
        }
        size_t rightSize = printingOrchestrator->getDiffBytesWithoutDeletions(change.second, tmpRightAbsolutePosition, windowUUID);
        return std::make_pair(tmpLeftAbsolutePosition, leftSize + noBytesBetweenChanges + rightSize);
    }

    SearchEngineBase::~SearchEngineBase()
    {
    }

    std::unordered_map<std::string, std::u32string> SearchEngineBase::searchedStringWithoutLastChar;
} // namespace feather::utils::algorithm