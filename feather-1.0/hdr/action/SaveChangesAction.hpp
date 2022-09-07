#pragma once

#include "action/Action.hpp"
#include "action/input/SaveChangesActionInput.hpp"
#include "action/output/SaveChangesActionOutput.hpp"
#include "utils/FilesystemInterface.hpp"

namespace feather::action
{
  class SaveChangesAction : public Action<SaveChangesActionInput, SaveChangesActionOutput>
  {
  private:
    void cleanUp(utils::datatypes::Uuid const &);

  public:
    SaveChangesAction(SaveChangesActionInput &&);
    SaveChangesActionOutput execute(std::shared_ptr<utils::FilesystemInterface>);
  };
} // namespace feather::action