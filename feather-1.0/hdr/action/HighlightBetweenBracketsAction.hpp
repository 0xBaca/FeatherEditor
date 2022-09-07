#pragma once

#include "action/Action.hpp"
#include "action/input/HighlightBetweenBracketsActionInput.hpp"
#include "action/output/HighlightBetweenBracketsActionOutput.hpp"

namespace feather::action
{
    class HighlightBetweenBracketsAction : public Action<HighlightBetweenBracketsActionInput, HighlightBetweenBracketsActionOutput>
    {
    private:
        std::pair<pair, std::optional<char32_t>> findOpeningBracket(char32_t, char32_t);

    public:
        HighlightBetweenBracketsAction(HighlightBetweenBracketsActionInput &&);
        HighlightBetweenBracketsActionOutput execute();
    };
} // namespace feather::action