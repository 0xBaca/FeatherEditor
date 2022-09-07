#include "action/SearchSubstringAction.hpp"

namespace feather::action
{
    SearchSubstringAction::SearchSubstringAction(SearchSubstringActionInput &input)
        : Action<SearchSubstringActionInput, SearchSubstringActionOutput>(input, SearchSubstringActionOutput{std::make_pair(std::nullopt, utils::datatypes::ERROR::NONE)})
    {
    }
} // namespace feather::action