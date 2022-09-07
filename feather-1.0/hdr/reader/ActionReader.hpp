#pragma once

#include "action/ActionHandler.hpp"
#include "reader/Reader.hpp"

namespace feather::reader
{
  class ActionReader : public Reader
  {
  public:
    ActionReader(std::unique_ptr<windows::SubWindowInterface> &, std::shared_ptr<action::ActionHandler>, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<windows::WindowsManager>, std::shared_ptr<printer::ChangesStack>, std::shared_ptr<utils::helpers::KeyReader> keyReader, std::pair<utils::windows::FramePositions, pair>, std::shared_ptr<utils::FilesystemInterface>, std::list<pair> &, std::set<pair> &, std::list<pair>::iterator &);
    bool isAtHistoryBegin() const override;
    bool isAtHistoryEnd() const override;
    std::optional<std::u32string> getCurrFromHistory() const override;
    std::optional<std::u32string> getNextFromHistory() const override;
    std::optional<std::u32string> getPreviousFromHistory() const override;
    bool operator()();
    ~ActionReader();

  private:
    bool fixUserInputPath(std::u32string const &, std::u32string const &);
    bool handleCommand();
    bool isInputAllDigits() const;
    void handleModeChange();
    std::set<utils::datatypes::COLOR> parseColors(std::u32string const &);
    static std::map<std::u32string, utils::datatypes::COLOR> colorMapper;
    static std::set<std::u32string> actionCommands;
    std::shared_ptr<printer::ChangesStack> changesStack;
    std::shared_ptr<utils::FilesystemInterface> filesystem;
  };
} // namespace feather::reader