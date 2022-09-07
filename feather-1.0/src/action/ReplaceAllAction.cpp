#include "action/RemoveAllAction.hpp"
#include "action/ReplaceAllAction.hpp"
#include "action/SearchSubstringAction.hpp"
#include "config/Config.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/datatypes/Strings.hpp"
#include "windows/WindowsManager.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    ReplaceAllAction::ReplaceAllAction(ReplaceAllActionInput &&input)
        : Action<ReplaceAllActionInput, ReplaceAllActionOutput>(input, ReplaceAllActionOutput())
    {
    }

    ReplaceAllActionOutput ReplaceAllAction::execute()
    {
        /*
        std::u32string replaceFrom(getActionInput().replaceFrom);
        action::SearchSubstringActionInput searchSubstringActionInput(getActionInput().windowUUID, std::move(replaceFrom), getActionInput().storageFactory, getActionInput().printingOrchestrator, getActionInput().bufferFiller, getActionInput().windowsManager, getActionInput().currFrameVisitor, utils::Direction::DOWN, getActionInput().windowsManager->getMainWindow(getActionInput().windowUUID)->getCursorRealPosition(getActionInput().printingOrchestrator.get()));
        auto result = std::make_unique<action::RemoveAllAction>(action::RemoveAllActionInput(searchSubstringActionInput, getActionInput().windowUUID, getActionInput().windowsManager, getActionInput().cursorPos, getActionInput().storage, getActionInput().printingOrchestrator, utils::helpers::Conversion::squeezeu32String(getActionInput().replaceFrom).length()))->execute();

        auto toStringSqueezed = utils::helpers::Conversion::squeezeu32String(getActionInput().replaceTo);
        for (auto const &e : result.deleted)
        {
#ifndef _FEATHER_TEST_
            timeout(0);
            if (KEY_RESIZE == getActionInput().windowsManager->getMainWindow(getActionInput().windowUUID)->getLibraryWindowHandler()->getCharacter())
            {
                getActionInput().windowsManager->recreateWindows();
                getActionInput().windowsManager->refreshAllWindows(getActionInput().currFrameVisitor);
                utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::WORKING_STRING, std::nullopt, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().currFrameVisitor);
                utils::NCursesWrapper::applyRefresh();
            }
            timeout(-1);
#endif
            getActionInput().printingOrchestrator->addChanges(getActionInput().windowUUID, getActionInput().storage, e.first, std::vector<char>(toStringSqueezed.begin(), toStringSqueezed.end()), utils::helpers::Conversion::squeezeu32String(getActionInput().replaceTo).length());
        }
        */
    }
}