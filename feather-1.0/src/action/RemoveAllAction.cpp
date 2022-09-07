#include "action/RemoveAllAction.hpp"
#include "action/SearchSubstringAction.hpp"
#include "config/Config.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Lambda.hpp"
#include "windows/WindowsManager.hpp"

#include <csignal>

extern std::unique_ptr<const feather::config::Configuration> configuration;

static sig_atomic_t sigIntReceived = 0;
static sig_atomic_t sigBusReceived = 0;

namespace feather::action
{
    RemoveAllAction::RemoveAllAction(RemoveAllActionInput &&input)
        : Action<RemoveAllActionInput, RemoveAllActionOutput>(input, RemoveAllActionOutput())
    {
    }

    RemoveAllActionOutput RemoveAllAction::execute()
    {
#ifndef _FEATHER_TEST_
        utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::WORKING_STRING, std::nullopt, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().searchSubstringActionInput.currFrameVisitor);
        utils::NCursesWrapper::applyRefresh();
#endif
        //auto firstResult = getActionInput().searchSubstringActionInput.searchEngine->getNextMatch(getActionInput().windowsManager, getActionInput().searchSubstringActionInput.currFrameVisitor, utils::Direction::DOWN);
        //if (!firstResult.first.has_value())
        {
            return RemoveAllActionOutput{{}};
        }
        //std::pair<std::optional<pair>, utils::datatypes::ERROR> result = firstResult;
        std::map<pair, pair> toDelete;
        //do
        {
            unblockSignals();
            //toDelete.insert(std::make_pair(result.first.value(), getShiftedPos(result.first.value(), getActionInput().bytesToRemove)));
            //result = getActionInput().searchSubstringActionInput.searchEngine->getNextMatch(getActionInput().windowsManager, getActionInput().searchSubstringActionInput.currFrameVisitor, utils::Direction::DOWN);
#ifndef _FEATHER_TEST_
            timeout(0);
            if (KEY_RESIZE == getActionInput().windowsManager->getMainWindow(getActionInput().windowUUID)->getLibraryWindowHandler()->getCharacter())
            {
                getActionInput().windowsManager->recreateWindows();
                getActionInput().windowsManager->refreshAllWindows(getActionInput().searchSubstringActionInput.currFrameVisitor);
                utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::WORKING_STRING, std::nullopt, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().searchSubstringActionInput.currFrameVisitor);
                utils::NCursesWrapper::applyRefresh();
            }
            timeout(-1);
#endif
        } //while (result.first.has_value() && result.first != firstResult.first);
        //getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, toDelete);
        //return RemoveAllActionOutput{std::move(toDelete)};
    }

    pair RemoveAllAction::getShiftedPos(pair pos, size_t offset)
    {
        while (offset)
        {
            if (getActionInput().printingOrchestrator->isCharAtPosDeleted(pos, getActionInput().windowUUID))
            {
                pos = getActionInput().printingOrchestrator->getDeletionEnd(getActionInput().windowUUID, pos);
            }
            else if (getActionInput().printingOrchestrator->getNumberOfChangesBytesAtPos(pos.first, getActionInput().windowUUID) == pos.second)
            {
                pos.first++;
                pos.second = 0;
                --offset;
            }
            else
            {
                size_t toMove = std::min(offset, getActionInput().printingOrchestrator->getNumberOfChangesBytesAtPos(pos.first, getActionInput().windowUUID) - pos.second);
                pos.second += toMove;
                offset -= toMove;
            }
        }
        return pos;
    }

    void RemoveAllAction::sigBusHandler(int signum)
    {
        sigBusReceived = 1;
    }

    void RemoveAllAction::sigIntHandler(int signum)
    {
        sigIntReceived = 1;
    }

    void RemoveAllAction::unblockSignals()
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGBUS);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
        std::signal(SIGBUS, sigBusHandler);
        std::signal(SIGINT, sigIntHandler);
    }
} // namespace feather::action