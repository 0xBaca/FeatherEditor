#include "action/ShiftRightAction.hpp"
#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    ShiftRightAction::ShiftRightAction(ShiftRightActionInput &&input)
        : Action<ShiftRightActionInput, ShiftRightActionOutput>(input, ShiftRightActionOutput())
    {
    }

    ShiftRightActionOutput ShiftRightAction::execute()
    {
        utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::WORKING_STRING, std::nullopt, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().currFrameVisitor);
        utils::NCursesWrapper::applyRefresh();
        std::optional<pair> lastTabAddedPos = std::nullopt;
        //size_t toShift = 0;
        if (!utils::helpers::Lambda::isCharAtPos(getActionInput().range.first, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            auto prevNewLine = utils::helpers::Lambda::findPrevious(getActionInput().range.first, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
            auto prevPos = prevNewLine.second.has_value() ? getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, prevNewLine.first) : prevNewLine.first;
            getActionInput().printingOrchestrator->addChanges(getActionInput().windowUUID, getActionInput().storage, prevPos, {'\t'}, 1, false);
            if (configuration->isHexMode())
            {
                auto hexModeWindow = getActionInput().windowsManager->getMainWindow(getActionInput().windowsManager->getSecondaryActiveMainWindowForHexMode());
                getActionInput().printingOrchestrator->addChanges(hexModeWindow->getUUID(), hexModeWindow->getStorage(), prevPos, {'\t'}, 1, false);
            }
            lastTabAddedPos = prevPos;
            //toShift = 1;
        }
        pair chunkReadStartPos = getActionInput().range.first;
        bool shouldAddTab = false;
        size_t leftToRead = getActionInput().printingOrchestrator->getDiffBytesWithoutDeletions(getActionInput().range.first, getActionInput().range.second, getActionInput().windowUUID, {});
        size_t canRead = std::min(getActionInput().printingOrchestrator->getDiffBytesWithoutDeletions(getActionInput().range.first, getActionInput().range.second, getActionInput().windowUUID, {}), configuration->getMemoryBytesRelaxed());
        std::unique_ptr<utils::algorithm::SearchBuffer> localBuffer = std::make_unique<utils::algorithm::SearchBuffer>(getActionInput().windowUUID, getActionInput().printingOrchestrator, getActionInput().range.first);
        auto chunk = getActionInput().printingOrchestrator->getContinousCharacters(chunkReadStartPos, canRead, getActionInput().storage, getActionInput().windowUUID);
        chunkReadStartPos = chunk.second;
        localBuffer->addEntry(std::move(chunk.first));
        while (leftToRead)
        {
            if (localBuffer->isBufferEmpty())
            {
                leftToRead -= canRead;
                canRead = std::min(getActionInput().printingOrchestrator->getDiffBytesWithoutDeletions(getActionInput().range.first, getActionInput().range.second, getActionInput().windowUUID, {}), configuration->getMemoryBytesRelaxed());
                chunk = getActionInput().printingOrchestrator->getContinousCharacters(chunkReadStartPos, canRead, getActionInput().storage, getActionInput().windowUUID);
                chunkReadStartPos = chunk.second;
                localBuffer->addEntry(std::move(chunk.first));
                continue;
            }
            if (shouldAddTab)
            {
                auto currCursorPos = localBuffer->getCurrentPosition();
                pair toAddPos = currCursorPos;
                /*if (lastTabAddedPos.has_value() && lastTabAddedPos->first == currCursorPos.first)
                {
                    toAddPos = std::make_pair(currCursorPos.first, currCursorPos.second + toShift);
                    ++toShift;
                }
                else
                {
                    toShift = 1;
                }
                lastTabAddedPos = currCursorPos;
                */
                getActionInput().printingOrchestrator->addChanges(getActionInput().windowUUID, getActionInput().storage, toAddPos, {'\t'}, 1, false);
                localBuffer->updateCurrPos(std::make_pair(currCursorPos.first, currCursorPos.second + 1));
            }
            shouldAddTab = utils::helpers::Lambda::isNewLineChar(localBuffer->getNextCharacter());
            timeout(0);
            if (KEY_RESIZE == getActionInput().windowsManager->getMainWindow(getActionInput().windowUUID)->getLibraryWindowHandler()->getCharacter())
            {
                getActionInput().windowsManager->recreateWindows();
                getActionInput().windowsManager->refreshAllWindows(getActionInput().currFrameVisitor);
                utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::WORKING_STRING, std::nullopt, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().currFrameVisitor);
                utils::NCursesWrapper::applyRefresh();
            }
            timeout(-1);
        }
        return ShiftRightActionOutput{getActionInput().range.first};
    }
} // namespace feather::action