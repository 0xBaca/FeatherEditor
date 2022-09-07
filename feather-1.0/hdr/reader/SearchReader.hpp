#pragma once

#include "reader/Reader.hpp"

namespace feather::reader
{
  class SearchReader : public Reader
  {
  public:
    SearchReader(std::unique_ptr<windows::SubWindowInterface> &, std::shared_ptr<action::ActionHandler>, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<windows::WindowsManager>, std::shared_ptr<utils::helpers::KeyReader>, std::list<pair> &, std::set<pair> &, std::list<pair>::iterator &, std::pair<utils::windows::FramePositions, pair>, pair, utils::Direction);
    bool isAtHistoryBegin() const override;
    bool isAtHistoryEnd() const override;
    std::optional<std::u32string> getCurrFromHistory() const override;
    std::optional<std::u32string> getNextFromHistory() const override;
    std::optional<std::u32string> getPreviousFromHistory() const override;
    pair operator()();
    ~SearchReader();

  private:
    char32_t searchToken;
    pair cursorPos;
    std::chrono::time_point<std::chrono::steady_clock> lastAddBreakpointPressed;
    std::chrono::time_point<std::chrono::steady_clock> trialVersionTimeout;
    utils::Direction direction;
    void addBreakpoint();
    pair handleCommand(std::u32string const &);
    action::SearchSubstringActionOutput handleSearchImpl(action::SearchSubstringActionInput);
  };
} // namespace feather::reader