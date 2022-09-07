#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveBetweenBracketsActionInput.hpp"
#include "action/output/RemoveBetweenBracketsActionOutput.hpp"

namespace feather::action
{
    class RemoveBetweenBracketsAction : public Action<RemoveBetweenBracketsActionInput, RemoveBetweenBracketsActionOutput>
    {
    private:
        std::pair<pair, std::optional<char32_t>> findOpeningBracket(char32_t, char32_t);

    public:
        RemoveBetweenBracketsAction(RemoveBetweenBracketsActionInput &&);
        RemoveBetweenBracketsActionOutput execute();
    };
} // namespace feather::action