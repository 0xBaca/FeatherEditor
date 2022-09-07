#include "reader/KeyStroke.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    EditLineAboveAction::EditLineAboveAction(EditLineAboveActionInput &&input)
        : Action<EditLineAboveActionInput, EditLineAboveActionOutput>(input, EditLineAboveActionOutput())
    {
    }

    EditLineAboveActionOutput EditLineAboveAction::execute()
    {
        auto cursorPosition = getActionInput().cursorPos;
        std::vector<char> newLine(1, '\n');
        if (utils::helpers::Lambda::isCharAtPos(cursorPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            getActionInput().printingOrchestrator->addChanges(getActionInput().windowUUID, getActionInput().storage, cursorPosition, newLine, 1UL);
            getActionInput().keyStroke->updateBreakPoints(cursorPosition, 1UL, getActionInput().windowUUID);
            return EditLineAboveActionOutput{std::make_pair(cursorPosition, 1UL), cursorPosition};
        }
        auto previousNewLine = utils::helpers::Lambda::findPrevious(cursorPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
        if (previousNewLine.second.has_value())
        {
            cursorPosition = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, previousNewLine.first);
        }
        else
        {
            cursorPosition = previousNewLine.first;
        }
        std::vector<char> changes;
        auto rightSideCursorRealPos = utils::helpers::Lambda::findNext(cursorPosition, [&](char32_t c) {
            if (utils::helpers::Lambda::isSpace(c))
            {
                changes.push_back(' ');
                
            } else if (utils::helpers::Lambda::isTabChar(c))
            {
                changes.push_back('\t');
            } return !utils::helpers::Lambda::isSpace(c) && !utils::helpers::Lambda::isTabChar(c);},
        getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true);

        bool shouldFillPadding = false;
        if (rightSideCursorRealPos.second.has_value())
        {
            if (!utils::helpers::Lambda::isNewLineChar(rightSideCursorRealPos.second.value()))
            {
                shouldFillPadding = true;
            }
        }
        if (!shouldFillPadding)
        {
            changes.clear();
        }
        changes.push_back('\n');
        getActionInput().printingOrchestrator->addChanges(getActionInput().windowUUID, getActionInput().storage, cursorPosition, changes, changes.size(), false, getActionInput().cursorPos, configuration->isHexMode() ? pair(cursorPosition.first, cursorPosition.second + changes.size()) : pair(cursorPosition.first, cursorPosition.second + changes.size() - 1UL));
        getActionInput().keyStroke->updateBreakPoints(cursorPosition, changes.size(), getActionInput().windowUUID);
        auto newCursorPos = pair(cursorPosition.first, cursorPosition.second + changes.size());
        return EditLineAboveActionOutput{std::make_pair(cursorPosition, changes.size()), pair(newCursorPos.first, newCursorPos.second - 1UL)};
    }
} // namespace feather::action