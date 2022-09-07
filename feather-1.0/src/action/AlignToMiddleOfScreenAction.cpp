#include "action/AlignToMiddleOfScreenAction.hpp"

namespace feather::action
{
    AlignToMiddleOfScreenAction::AlignToMiddleOfScreenAction(AlignToMiddleOfScreenActionInput &&input)
        : Action<AlignToMiddleOfScreenActionInput, AlignToMiddleOfScreenActionOutput>(input, AlignToMiddleOfScreenActionOutput())
    {
    }

    AlignToMiddleOfScreenActionOutput AlignToMiddleOfScreenAction::execute()
    {
        auto initialCursorPosition = getActionInput().window->getCursorPosition();
        auto initialCursorRealPosition = getActionInput().window->getCursorRealPosition(getActionInput().printingOrchestrator.get());
        auto firstFramePosition = getActionInput().window->getFramePosition().startFramePosition;
        size_t middleOfTheScreen = getActionInput().window->getWindowDimensions().first / 2UL;
        size_t diff = initialCursorPosition.first > middleOfTheScreen ? initialCursorPosition.first - middleOfTheScreen : middleOfTheScreen - initialCursorPosition.first;
        getActionInput().window->disableCursor();
        if (initialCursorPosition.first > middleOfTheScreen)
        {
            while (diff--)
            {
                getActionInput().windowsManager->refreshAllWindows(getActionInput().lineDownVisitor);
            }
        }
        else if (firstFramePosition != getActionInput().printingOrchestrator->getFirstValidPosition(getActionInput().window->getUUID()))
        {
            while (diff--)
            {
                getActionInput().windowsManager->refreshAllWindows(getActionInput().lineUpVisitor);
            }
        }
        return AlignToMiddleOfScreenActionOutput{initialCursorRealPosition};
    }
} // namespace feather::action
