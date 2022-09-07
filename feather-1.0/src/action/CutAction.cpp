#include "action/CutAction.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    CutAction::CutAction(CutActionInput &&input)
        : Action<CutActionInput, CutActionOutput>(input, CutActionOutput())
    {
    }

    CutActionOutput CutAction::execute()
    {
        /* TODO */
    }
} // namespace feather::action
