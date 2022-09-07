#include "action/ActionHandler.hpp"

namespace feather::action
{
    ActionHandler::ActionHandler(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg)
        : arbitraryPosition(std::make_unique<utils::windows::FramePositions>()), printingOrchestrator(printingOrchestratorArg), windowsManager(windowsManagerArg)
    {
    }

    AlignToMiddleOfScreenActionOutput ActionHandler::handle(std::unique_ptr<AlignToMiddleOfScreenAction> action)
    {
        return action->execute();
    }

    CopyLineActionOutput ActionHandler::handle(std::unique_ptr<CopyLineAction> action)
    {
        return action->execute();
    }

    EditLineAboveActionOutput ActionHandler::handle(std::unique_ptr<EditLineAboveAction> action)
    {
        return action->execute();
    }

    EditLineBelowActionOutput ActionHandler::handle(std::unique_ptr<EditLineBelowAction> action)
    {
        return action->execute();
    }

    EditNextCharacterActionOutput ActionHandler::handle(std::unique_ptr<EditNextCharacterAction> action)
    {
        return action->execute();
    }

    GetStringsActionOutput ActionHandler::handle(std::unique_ptr<GetStringsAction> action)
    {
        return action->execute();
    }

    GotoLineActionOutput ActionHandler::handle(std::unique_ptr<GotoLineAction> action)
    {
        return action->execute();
    }

    HighlightBetweenBracketsActionOutput ActionHandler::handle(std::unique_ptr<HighlightBetweenBracketsAction> action)
    {
        return action->execute();
    }

    JumpToBracketActionOutput ActionHandler::handle(std::unique_ptr<JumpToBracketAction> action)
    {
        return action->execute();
    }

    MoveToByteActionOutput ActionHandler::handle(std::unique_ptr<MoveToByteAction> action)
    {
        return action->execute();
    }

    MoveToFileBeginActionOutput ActionHandler::handle(std::unique_ptr<MoveToFileBeginAction> action)
    {
        return action->execute();
    }

    MoveToFileEndActionOutput ActionHandler::handle(std::unique_ptr<MoveToFileEndAction> action)
    {
        return action->execute();
    }

    MoveToLineBeginActionOutput ActionHandler::handle(std::unique_ptr<MoveToLineBeginAction> action)
    {
        return action->execute();
    }

    MoveToLineEndActionOutput ActionHandler::handle(std::unique_ptr<MoveToLineEndAction> action)
    {
        return action->execute();
    }

    MoveToLineEndWithEditActionOutput ActionHandler::handle(std::unique_ptr<MoveToLineEndWithEditAction> action)
    {
        return action->execute();
    }

    MoveToNextWordActionOutput ActionHandler::handle(std::unique_ptr<MoveToNextWordAction> action)
    {
        return action->execute();
    }

    MoveToPreviousWordActionOutput ActionHandler::handle(std::unique_ptr<MoveToPreviousWordAction> action)
    {
        return action->execute();
    }

    PasteActionOutput ActionHandler::handle(std::unique_ptr<PasteAction> action)
    {
        return action->execute();
    }

    RemoveBetweenBracketsActionOutput ActionHandler::handle(std::unique_ptr<RemoveBetweenBracketsAction> action)
    {
        return action->execute();
    }

    RemoveBetweenWhiteCharactersActionOutput ActionHandler::handle(std::unique_ptr<RemoveBetweenWhiteCharactersAction> action)
    {
        return action->execute();
    }

    RemoveAllActionOutput ActionHandler::handle(std::unique_ptr<RemoveAllAction> action)
    {
        return action->execute();
    }

    RemoveCharacterUnderCursorActionOutput ActionHandler::handle(std::unique_ptr<RemoveCharacterUnderCursorAction> action)
    {
        return action->execute();
    }

    RemoveLineActionOutput ActionHandler::handle(std::unique_ptr<RemoveLineAction> action)
    {
        return action->execute();
    }

    RemoveUntilFileBeginActionOutput ActionHandler::handle(std::unique_ptr<RemoveUntilFileBeginAction> action)
    {
        return action->execute();
    }

    RemoveUntilFileEndActionOutput ActionHandler::handle(std::unique_ptr<RemoveUntilFileEndAction> action)
    {
        return action->execute();
    }

    RemoveUntilLineEndActionOutput ActionHandler::handle(std::unique_ptr<RemoveUntilLineEndAction> action)
    {
        return action->execute();
    }

    RemoveWordBackwardActionOutput ActionHandler::handle(std::unique_ptr<RemoveWordBackwardAction> action)
    {
        return action->execute();
    }

    RemoveWordForwardActionOutput ActionHandler::handle(std::unique_ptr<RemoveWordForwardAction> action)
    {
        return action->execute();
    }

    ReplaceAllActionOutput ActionHandler::handle(std::unique_ptr<ReplaceAllAction> action)
    {
        //return action->execute();
        return {};
    }

    SaveChangesActionOutput ActionHandler::handle(std::unique_ptr<SaveChangesAction> action)
    {
        auto filesystem = std::shared_ptr<utils::FilesystemInterface>(new utils::Filesystem());
        return action->execute(filesystem);
    }

    SearchSubstringActionOutput ActionHandler::handle(std::unique_ptr<SearchSubstringAction> action)
    {
        return SearchSubstringActionOutput{action->getActionInput().searchEngine->getNextMatch(action->getActionInput().windowsManager, action->getActionInput().currFrameVisitor, action->getActionInput().halfFrameUpVisitor, action->getActionInput().currBottomBarWindow, action->getActionInput().currentFeatherMode, action->getActionInput().searchDirection)};
    }

    ShiftRightActionOutput ActionHandler::handle(std::unique_ptr<ShiftRightAction> action)
    {
        return action->execute();
    }

    SkipEmptyCharactersActionOutput ActionHandler::handle(std::unique_ptr<SkipEmptyCharactersAction> action)
    {
        return action->execute();
    }

    SwitchModeActionOutput ActionHandler::handle(std::unique_ptr<SwitchModeAction> action)
    {
        return action->execute();
    }

    std::unique_ptr<utils::windows::FramePositions> const &ActionHandler::getArbitraryPosition() const
    {
        return arbitraryPosition;
    }
} // namespace feather::action
