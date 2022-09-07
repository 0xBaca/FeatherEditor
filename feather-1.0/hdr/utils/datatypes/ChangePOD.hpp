#pragma once

#include "utils/datatypes/ChangeType.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::utils::datatypes
{
    struct ChangePOD
    {
        ChangePOD(ChangeType changeTypeArg, pair undoPositionArg, pair redoPositionArg, std::map<pair, pair> rangesArg)
            : changeType(changeTypeArg), undoPosition(undoPositionArg), redoPosition(redoPositionArg), ranges(rangesArg)
        {
        }

        ChangePOD(ChangeType changeTypeArg, pair undoPositionArg, pair redoPositionArg, std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> selectedTextArg)
            : changeType(changeTypeArg), undoPosition(undoPositionArg), redoPosition(redoPositionArg), selectedText(selectedTextArg)
        {
        }

        void updatePositions(pair pos, size_t size)
        {
            if (undoPosition.first == pos.first && pos.second <= undoPosition.second)
            {
                undoPosition.second += size;
            }

            if (redoPosition.first == pos.first && pos.second <= redoPosition.second)
            {
                redoPosition.second += size;
            }

            ranges = utils::helpers::Lambda::shiftIntervals(ranges, pos, size, std::nullopt);
            ranges = utils::helpers::Lambda::disjoin(ranges, pos, pair(pos.first, pos.second + size));

            if (selectedText.has_value())
            {
                if (selectedText->first.first.first == pos.first && pos.second <= selectedText->first.first.second)
                {
                    selectedText->first.first.second += size;
                }
                if (selectedText->first.second.first == pos.first && pos.second <= selectedText->first.second.second)
                {
                    selectedText->first.second.second += size;
                }
            }
        }

        void updatePositionsAfterSave(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartor, utils::datatypes::Uuid const &windowUUID)
        {
            undoPosition = pair(printingOrchestartor->convertVirtualPositionToByteOffset(undoPosition, windowUUID), 0);
            redoPosition = pair(printingOrchestartor->convertVirtualPositionToByteOffset(redoPosition, windowUUID), 0);
            std::map<pair, pair> tmpRanges;
            for (auto &e : ranges)
            {
                tmpRanges.insert(std::make_pair(pair(printingOrchestartor->convertVirtualPositionToByteOffset(e.first, windowUUID), 0), pair(printingOrchestartor->convertVirtualPositionToByteOffset(e.second, windowUUID), 0)));
            }
            ranges = tmpRanges;

            if (selectedText.has_value())
            {
                selectedText->first.first = std::make_pair(printingOrchestartor->convertVirtualPositionToByteOffset(selectedText->first.first, windowUUID), 0);
                selectedText->first.second = std::make_pair(printingOrchestartor->convertVirtualPositionToByteOffset(selectedText->first.second, windowUUID), 0);
            }
        }

        ChangeType changeType;
        pair undoPosition;
        pair redoPosition;
        std::map<pair, pair> ranges;
        std::optional<std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>>> selectedText;
    };
} // namespace feather::utils::datatypes