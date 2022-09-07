#include "action/MoveToByteAction.hpp"
#include "config/Config.hpp"
#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/helpers/Lambda.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
MoveToByteAction::MoveToByteAction(MoveToByteActionInput &&input)
    : Action<MoveToByteActionInput, MoveToByteActionOutput>(input, MoveToByteActionOutput())
{
}

MoveToByteActionOutput MoveToByteAction::execute()
{
    auto currPosition = pair(getActionInput().printingOrchestrator->getFirstValidPosition(getActionInput().windowUUID));
    pair lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().windowUUID, getActionInput().storage);
    size_t lastByte = getActionInput().printingOrchestrator->convertVirtualPositionToByteOffset(lastValidPosition, getActionInput().windowUUID);
    const size_t byteMoveTo = getActionInput().byteMoveTo;
    if (lastByte <= byteMoveTo)
    {
        pair beforePosToReturn = getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, lastValidPosition);
        bool isTwoNewLines = (beforePosToReturn != lastValidPosition) && utils::helpers::Lambda::isCharAtPos(beforePosToReturn, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first;
        if (!isTwoNewLines && !configuration->isHexMode() && utils::helpers::Lambda::isCharAtPos(lastValidPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            return MoveToByteActionOutput{getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, lastValidPosition)};
        }
        return MoveToByteActionOutput{lastValidPosition};
    }
    pair posToReturn = utils::helpers::Lambda::convertByteOffsetToPosition(currPosition, byteMoveTo, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, false);
    pair beforePosToReturn = getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, posToReturn);
    bool isTwoNewLines = (posToReturn != beforePosToReturn) && utils::helpers::Lambda::isCharAtPos(beforePosToReturn, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first;
    if (!isTwoNewLines && !configuration->isHexMode() && utils::helpers::Lambda::isCharAtPos(posToReturn, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
    {
        return MoveToByteActionOutput{getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, posToReturn)};
    }
    return MoveToByteActionOutput{posToReturn};
}
} // namespace feather::action
