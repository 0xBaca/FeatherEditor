#include "printer/WindowStatus.hpp"
#include "utils/BufferFiller.hpp"
#include "utils/helpers/Conversion.hpp"

namespace feather::printer
{
    std::u32string WindowStatus::getCharAbsolutePosition(feather::windows::SubWindowInterface *window, std::shared_ptr<PrintingOrchestratorInterface> printingOrchestrator, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        auto parentWindow = window->getParentWindowHandler();
        pair realPosition = parentWindow->getCursorRealPosition(printingOrchestrator.get());
        return sizeToString(printingOrchestrator->convertVirtualPositionToByteOffset(arbitraryPosition.has_value() ? arbitraryPosition->second : realPosition, parentWindow->getUUID())).append(U" B");
    }

    std::u32string WindowStatus::getPrintedProgress(feather::windows::SubWindowInterface *window, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        auto parentWindow = window->getParentWindowHandler();
        auto &framePositions = parentWindow->getBufferFiller()->getFramePositions();
        size_t fileSize = parentWindow->getStorage()->getSize();
        size_t lastValidPos = printingOrchestrator->convertVirtualPositionToByteOffset(printingOrchestrator->getLastValidPosition(parentWindow->getUUID(), parentWindow->getStorage()), parentWindow->getUUID());
        size_t fileSizeWithChanges = fileSize + printingOrchestrator->getTotalBytesOfNewChanges(parentWindow->getUUID()) - printingOrchestrator->getTotalBytesOfDeletions(parentWindow->getUUID());
        //Avoid dividing by 0, it can happen when creating new file with no changes yet
        fileSizeWithChanges = !fileSizeWithChanges ? 1UL : fileSizeWithChanges;
        size_t startFrameWithChanges = printingOrchestrator->convertVirtualPositionToByteOffset(arbitraryPosition.has_value() ? arbitraryPosition->first.startFramePosition : framePositions.startFramePosition, parentWindow->getUUID());
        size_t endFrameWithChanges = printingOrchestrator->convertVirtualPositionToByteOffset(arbitraryPosition.has_value() ? arbitraryPosition->first.endFramePosition : framePositions.endFramePosition, parentWindow->getUUID());
        size_t checkpoit = startFrameWithChanges > endFrameWithChanges ? startFrameWithChanges : endFrameWithChanges;
        size_t progress = (lastValidPos == endFrameWithChanges) ? 100UL : std::ceil(static_cast<double>(checkpoit) / static_cast<double>(fileSizeWithChanges) * 100);
        return sizeToString(progress ? progress : 100UL);
    }

    std::u32string WindowStatus::sizeToString(size_t val)
    {
        std::u32string stringVal = utils::helpers::Conversion::size_tToU32String(val);
        if (stringVal.size() > 12LL)
        {
            return {INFINITY_CHARACTER};
        }
        return stringVal;
    }

    char32_t WindowStatus::INFINITY_CHARACTER = 0x9e88e2;
} // namespace feather::printer