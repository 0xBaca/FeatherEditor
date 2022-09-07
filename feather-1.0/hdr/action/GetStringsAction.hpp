#pragma once

#include "action/Action.hpp"
#include "action/input/GetStringsActionInput.hpp"
#include "action/output/GetStringsActionOutput.hpp"

namespace feather::action
{
    class GetStringsAction : public Action<GetStringsActionInput, GetStringsActionOutput>
    {
    private:
        void updateResult(std::vector<std::pair<pair, pair>> &);

    public:
        GetStringsAction(GetStringsActionInput &&);
        GetStringsActionOutput execute();
    };
} // namespace feather::action