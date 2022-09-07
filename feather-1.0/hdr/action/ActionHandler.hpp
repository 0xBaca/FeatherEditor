#pragma once

#include "action/ActionType.hpp"
#include "action/AlignToMiddleOfScreenAction.hpp"
#include "action/CopyLineAction.hpp"
#include "action/EditLineAboveAction.hpp"
#include "action/EditLineBelowAction.hpp"
#include "action/EditNextCharacterAction.hpp"
#include "action/GetStringsAction.hpp"
#include "action/GotoLineAction.hpp"
#include "action/HighlightBetweenBracketsAction.hpp"
#include "action/JumpToBracketAction.hpp"
#include "action/MoveToByteAction.hpp"
#include "action/MoveToFileBeginAction.hpp"
#include "action/MoveToFileEndAction.hpp"
#include "action/MoveToLineBeginAction.hpp"
#include "action/MoveToLineEndAction.hpp"
#include "action/MoveToLineEndWithEditAction.hpp"
#include "action/MoveToNextWordAction.hpp"
#include "action/MoveToPreviousWordAction.hpp"
#include "action/PasteAction.hpp"
#include "action/RemoveBetweenBracketsAction.hpp"
#include "action/RemoveBetweenWhiteCharactersAction.hpp"
#include "action/RemoveAllAction.hpp"
#include "action/RemoveCharacterUnderCursorAction.hpp"
#include "action/RemoveLineAction.hpp"
#include "action/RemoveUntilFileBeginAction.hpp"
#include "action/RemoveUntilFileEndAction.hpp"
#include "action/RemoveUntilLineEndAction.hpp"
#include "action/RemoveWordBackwardAction.hpp"
#include "action/RemoveWordForwardAction.hpp"
#include "action/ReplaceAllAction.hpp"
#include "action/SaveChangesAction.hpp"
#include "action/SearchSubstringAction.hpp"
#include "action/SkipEmptyCharactersAction.hpp"
#include "action/ShiftRightAction.hpp"
#include "action/SwitchModeAction.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
  class ActionHandler
  {
  private:
    std::unique_ptr<utils::windows::FramePositions> arbitraryPosition;
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
    std::shared_ptr<windows::WindowsManager> windowsManager;

  public:
    ActionHandler(std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<windows::WindowsManager>);
    AlignToMiddleOfScreenActionOutput handle(std::unique_ptr<AlignToMiddleOfScreenAction>);
    CopyLineActionOutput handle(std::unique_ptr<CopyLineAction>);
    EditLineAboveActionOutput handle(std::unique_ptr<EditLineAboveAction>);
    EditLineBelowActionOutput handle(std::unique_ptr<EditLineBelowAction>);
    EditNextCharacterActionOutput handle(std::unique_ptr<EditNextCharacterAction>);
    GetStringsActionOutput handle(std::unique_ptr<GetStringsAction>);
    GotoLineActionOutput handle(std::unique_ptr<GotoLineAction>);
    HighlightBetweenBracketsActionOutput handle(std::unique_ptr<HighlightBetweenBracketsAction>);
    JumpToBracketActionOutput handle(std::unique_ptr<JumpToBracketAction>);
    MoveToByteActionOutput handle(std::unique_ptr<MoveToByteAction>);
    MoveToFileBeginActionOutput handle(std::unique_ptr<MoveToFileBeginAction>);
    MoveToFileEndActionOutput handle(std::unique_ptr<MoveToFileEndAction>);
    MoveToLineBeginActionOutput handle(std::unique_ptr<MoveToLineBeginAction>);
    MoveToLineEndActionOutput handle(std::unique_ptr<MoveToLineEndAction>);
    MoveToLineEndWithEditActionOutput handle(std::unique_ptr<MoveToLineEndWithEditAction>);
    MoveToNextWordActionOutput handle(std::unique_ptr<MoveToNextWordAction>);
    MoveToPreviousWordActionOutput handle(std::unique_ptr<MoveToPreviousWordAction>);
    PasteActionOutput handle(std::unique_ptr<PasteAction>);
    RemoveAllActionOutput handle(std::unique_ptr<RemoveAllAction>);
    RemoveBetweenBracketsActionOutput handle(std::unique_ptr<RemoveBetweenBracketsAction>);
    RemoveBetweenWhiteCharactersActionOutput handle(std::unique_ptr<RemoveBetweenWhiteCharactersAction>);
    RemoveCharacterUnderCursorActionOutput handle(std::unique_ptr<RemoveCharacterUnderCursorAction>);
    RemoveLineActionOutput handle(std::unique_ptr<RemoveLineAction>);
    RemoveUntilFileBeginActionOutput handle(std::unique_ptr<RemoveUntilFileBeginAction>);
    RemoveUntilFileEndActionOutput handle(std::unique_ptr<RemoveUntilFileEndAction>);
    RemoveUntilLineEndActionOutput handle(std::unique_ptr<RemoveUntilLineEndAction>);
    RemoveWordBackwardActionOutput handle(std::unique_ptr<RemoveWordBackwardAction>);
    RemoveWordForwardActionOutput handle(std::unique_ptr<RemoveWordForwardAction>);
    ReplaceAllActionOutput handle(std::unique_ptr<ReplaceAllAction>);
    SaveChangesActionOutput handle(std::unique_ptr<SaveChangesAction>);
    SearchSubstringActionOutput handle(std::unique_ptr<SearchSubstringAction>);
    ShiftRightActionOutput handle(std::unique_ptr<ShiftRightAction>);
    SkipEmptyCharactersActionOutput handle(std::unique_ptr<SkipEmptyCharactersAction>);
    SwitchModeActionOutput handle(std::unique_ptr<SwitchModeAction>);
    std::unique_ptr<utils::windows::FramePositions> const &getArbitraryPosition() const;
  };
} // namespace feather::action