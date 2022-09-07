#pragma once

#include "action/Action.hpp"
#include "action/input/SearchSubstringActionInput.hpp"
#include "action/output/SearchSubstringActionOutput.hpp"

namespace feather::action
{
  class SearchSubstringAction : public Action<SearchSubstringActionInput, SearchSubstringActionOutput>
  {
  public:
    SearchSubstringAction(SearchSubstringActionInput &);
  };
} // namespace feather::action