#include "action/PasteAction.hpp"
#include "config/Config.hpp"
#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/helpers/Signal.hpp"

extern sig_atomic_t sigIntReceived;
extern sig_atomic_t sigBusReceived;
extern feather::utils::FEATHER_MODE currentFeatherMode;
extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    PasteAction::PasteAction(PasteActionInput &&input)
        : Action<PasteActionInput, PasteActionOutput>(input, PasteActionOutput())
    {
    }

    PasteActionOutput PasteAction::execute()
    {
        utils::helpers::unblockSignals();
        auto currCursorPos = getActionInput().windowsManager->getMainWindow(getActionInput().windowUUID)->getCursorRealPosition(getActionInput().printingOrchestrator.get());
        auto source = getActionInput().source;
        auto sourceWindowUUID = source.first.first;
        auto destination = getActionInput().currCursorPos;
        auto floatingDestination = destination;
        auto textCopyPositionsToSkipUpdated = getActionInput().textCopyPositionsToSkip;
        auto addedBreakPointsUpdated = getActionInput().addedBreakPoints;
        bool appeanCopiedChunk = false;
        auto startPosToSkip = getChunkToSkip(getActionInput().textCopyPositionsToSkip.second, destination);
        utils::datatypes::DeletionsSnapshot currDeletionsSnapshot{getActionInput().printingOrchestrator->getTotalBytesOfDeletions(sourceWindowUUID), getActionInput().printingOrchestrator->getDeletionsPositions(sourceWindowUUID), getActionInput().printingOrchestrator->getReverseDeletionsPositions(sourceWindowUUID)};
        if (getActionInput().deletionsSnapshot.has_value())
        {
            getActionInput().printingOrchestrator->swapDeletions(getActionInput().deletionsSnapshot->second, getActionInput().deletionsSnapshot->first.first);
        }
        size_t bytesCanCopy = std::min(configuration->getMemoryBytesRelaxed(), getActionInput().printingOrchestrator->getDiffBytesWithoutDeletions(source.second.first, source.second.second, source.first.first, textCopyPositionsToSkipUpdated.second));
        auto sourceStorage = getActionInput().storageFactory->getStorageWithUtf8Check(source.first.second);
        auto chunk = getActionInput().printingOrchestrator->getContinousCharacters(source.second.first, bytesCanCopy, sourceStorage, source.first.first, false, textCopyPositionsToSkipUpdated.second);
        auto savedFeatherMode = currentFeatherMode;
        currentFeatherMode = feather::utils::FEATHER_MODE::WORKING_MODE;
        getActionInput().windowsManager->refreshAllWindows(getActionInput().currFrameVisitor);

        while (!chunk.first.empty())
        {
            getActionInput().printingOrchestrator->addChanges(getActionInput().windowUUID, getActionInput().storage, floatingDestination, chunk.first, chunk.first.size(), appeanCopiedChunk, currCursorPos);
            if (getActionInput().deletionsSnapshot.has_value())
            {
                currDeletionsSnapshot.shift(floatingDestination, chunk.first.size());
            }
            source.second.first = chunk.second;
            if (floatingDestination.first == source.second.second.first && floatingDestination.second <= source.second.second.second)
            {
                source.second.second.second += chunk.first.size();
            }
            if (floatingDestination.first == source.second.first.first && floatingDestination.second <= source.second.first.second)
            {
                source.second.first.second += chunk.first.size();
            }
            if (std::end(textCopyPositionsToSkipUpdated.second) == textCopyPositionsToSkipUpdated.second.find(startPosToSkip))
            {
                textCopyPositionsToSkipUpdated.second[startPosToSkip] = startPosToSkip;
            }
            textCopyPositionsToSkipUpdated.second[startPosToSkip].second += chunk.first.size();
 
            floatingDestination.second += chunk.first.size();
            bytesCanCopy = std::min(configuration->getMemoryBytesRelaxed(), getActionInput().printingOrchestrator->getDiffBytesWithoutDeletions(source.second.first, source.second.second, source.first.first, textCopyPositionsToSkipUpdated.second));
#ifndef _FEATHER_TEST_
            timeout(0);
            if (KEY_RESIZE == getActionInput().windowsManager->getMainWindow(getActionInput().windowUUID)->getLibraryWindowHandler()->getCharacter())
            {
                getActionInput().windowsManager->recreateWindows();
                getActionInput().windowsManager->refreshAllWindows(getActionInput().currFrameVisitor);
                utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::WORKING_STRING, std::nullopt, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().currFrameVisitor);
                utils::NCursesWrapper::applyRefresh();
            }
            else if (sigIntReceived)
            {
                    timeout(-1);
                    utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::INTERRUPTED, ONE_SECOND, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().currFrameVisitor);
                    currentFeatherMode = savedFeatherMode;
                    getActionInput().windowsManager->refreshAllWindows(getActionInput().currFrameVisitor);
                    utils::NCursesWrapper::applyRefresh();
                    utils::helpers::blockSignals();
                    break;
            }
            timeout(-1);
#endif
            chunk = getActionInput().printingOrchestrator->getContinousCharacters(source.second.first, bytesCanCopy, sourceStorage, source.first.first, false, textCopyPositionsToSkipUpdated.second);
            appeanCopiedChunk = true;
        }
        utils::datatypes::DeletionsSnapshot updatedDeletionsSnapshot{getActionInput().printingOrchestrator->getTotalBytesOfDeletions(sourceWindowUUID), getActionInput().printingOrchestrator->getDeletionsPositions(sourceWindowUUID), getActionInput().printingOrchestrator->getReverseDeletionsPositions(sourceWindowUUID)};
        if (getActionInput().deletionsSnapshot.has_value())
        {
            getActionInput().printingOrchestrator->swapDeletions(currDeletionsSnapshot, sourceWindowUUID);
        }
        currentFeatherMode = savedFeatherMode;
        getActionInput().windowsManager->refreshAllWindows(getActionInput().currFrameVisitor);
        size_t delta = floatingDestination.second - destination.second;
        textCopyPositionsToSkipUpdated.second = utils::helpers::Lambda::shiftIntervals(textCopyPositionsToSkipUpdated.second, destination, delta, std::optional(startPosToSkip));
        utils::helpers::Lambda::shiftPairs(getActionInput().breakPoints, destination, delta, getActionInput().currentBreakPoint);
        addedBreakPointsUpdated = utils::helpers::Lambda::shiftSetPairs(addedBreakPointsUpdated, destination, delta);
        utils::helpers::blockSignals();
        return PasteActionOutput{
            getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, floatingDestination),
            updatedDeletionsSnapshot,
            addedBreakPointsUpdated,
            textCopyPositionsToSkipUpdated,
        };
    }

    pair PasteAction::getChunkToSkip(std::map<pair, pair> const &textCopyPositionsToSkip, pair const &pos)
    {
        for (auto const &e : textCopyPositionsToSkip)
        {
            if (pos >= e.first && pos <= e.second)
            {
                return e.first;
            }
        }
        return pos;
    }
} // namespace feather::action
