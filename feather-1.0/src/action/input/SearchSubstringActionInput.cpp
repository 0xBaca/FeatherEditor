#include "action/input/SearchSubstringActionInput.hpp"
#include "utils/algorithm/FastSearch.hpp"

namespace feather::action
{
    SearchSubstringActionInput::SearchSubstringActionInput(utils::datatypes::Uuid const &windowUUID, std::u32string &&searchedString, std::shared_ptr<utils::storage::AbstractStorageFactory> fileStorageFactory, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::unique_ptr<utils::BufferFillerInterface> const &bufferFiller, std::shared_ptr<windows::WindowsManager> windowsManagerArg, std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitorArg, std::unique_ptr<printer::PrintingVisitorInterface> &halfFrameUpVisitorArg, std::unique_ptr<windows::SubWindowInterface> &currBottomBarWindowArg, utils::FEATHER_MODE &currentFeatherModeArg, utils::Direction searchDirectionArg, pair cursorPos, std::list<pair> &breakPoints, std::list<pair>::iterator &currentBreakPoint, std::set<pair> &breakPointsAdded, bool isHexSearch)
        : searchDirection(searchDirectionArg), windowsManager(windowsManagerArg), currFrameVisitor(currFrameVisitorArg), halfFrameUpVisitor(halfFrameUpVisitorArg), currBottomBarWindow(currBottomBarWindowArg), currentFeatherMode(currentFeatherModeArg)
    {
        searchEngine = std::make_shared<utils::algorithm::FastSearch>(windowUUID, std::move(searchedString), fileStorageFactory, storageFactory, printingOrchestrator, bufferFiller, cursorPos, breakPoints, currentBreakPoint, breakPointsAdded, isHexSearch);
    }
} // namespace feather::action