#include "action/output/SaveChangesActionOutput.hpp"

namespace feather::action
{
    SaveChangesActionOutput::SaveChangesActionOutput(utils::datatypes::ERROR error, std::optional<std::string> junkFileArg)
        : result(error), junkFile(junkFileArg)
    {
    }

    utils::datatypes::ERROR SaveChangesActionOutput::getResult() const
    {
        return result;
    }
} // namespace feather::action