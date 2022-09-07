#include "action/MoveToFileBeginAction.hpp"
#include "printer/PrintingOrchestratorInterface.hpp"

namespace feather::action
{
    MoveToFileBeginAction::MoveToFileBeginAction(MoveToFileBeginActionInput &&input)
        : Action<MoveToFileBeginActionInput, MoveToFileBeginActionOutput>(input, MoveToFileBeginActionOutput())
    {
    }

    MoveToFileBeginActionOutput MoveToFileBeginAction::execute()
    {
        return MoveToFileBeginActionOutput{getActionInput().printingOrchestrator->getFirstValidPosition(getActionInput().window->getUUID())};
    }
} // namespace feather::action
