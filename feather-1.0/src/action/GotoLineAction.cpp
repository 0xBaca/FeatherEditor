#include "action/GotoLineAction.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    GotoLineAction::GotoLineAction(GotoLineActionInput &&input)
        : Action<GotoLineActionInput, GotoLineActionOutput>(input, GotoLineActionOutput())
    {
    }

    GotoLineActionOutput GotoLineAction::execute()
    {
        auto currPos = pair(getActionInput().printingOrchestrator->getFirstValidPosition(getActionInput().windowUUID));
        size_t lineNumber = getActionInput().lineNumber;
        if (lineNumber <= 1)
        {
            return GotoLineActionOutput{currPos};
        }
        auto result = utils::helpers::Lambda::findNthNext(currPos, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, lineNumber - 1UL, true, true);
        if (!result.second.has_value())
        {
            return GotoLineActionOutput{std::nullopt};
        }
        return GotoLineActionOutput{getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, result.first)};
    }
} // namespace feather::action