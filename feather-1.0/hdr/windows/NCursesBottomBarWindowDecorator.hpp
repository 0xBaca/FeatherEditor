#pragma once

#include "reader/Reader.hpp"
#include "utils/windows/WindowInformationVisitorInterface.hpp"
#include "windows/MainWindowInterface.hpp"
#include "windows/SubWindowInterface.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::windows
{
  class NCursesBottomBarWindowDecorator : public SubWindowInterface
  {
  public:
    explicit NCursesBottomBarWindowDecorator(std::shared_ptr<MainWindowInterface>, WindowImplInterface *);
    bool isActionHistoryEmpty() const;
    bool isActionHistoryIteratorAtBegining() const;
    bool isActionHistoryIteratorAtLastPos() const;
    bool isSearchHistoryEmpty() const;
    bool isSearchHistoryIteratorAtBegining() const;
    bool isSearchHistoryIteratorAtLastPos() const;
    bool isSearchHexHistoryEmpty() const;
    bool isSearchHexHistoryIteratorAtBegining() const;
    bool isSearchHexHistoryIteratorAtLastPos() const;
    void addToActionHistory(std::u32string &&);
    void addToSearchHistory(std::u32string &&);
    void addToSearchHexHistory(std::u32string &&);
    void print(utils::ScreenBuffer const &) override;
    void refreshBuffer(printer::PrintingVisitorInterface *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
    void setReader(reader::Reader *);
    void setActionHistoryIteratorAtEnd();
    void setSearchHistoryIteratorAtEnd();
    void setSearchHexHistoryIteratorAtEnd();
    std::optional<std::u32string> getCurrFromActionHistory();
    std::optional<std::u32string> getCurrFromSearchHistory();
    std::optional<std::u32string> getCurrFromSearchHexHistory();
    std::optional<std::u32string> getNextFromActionHistory();
    std::optional<std::u32string> getNextFromSearchHistory();
    std::optional<std::u32string> getNextFromSearchHexHistory();
    std::optional<std::u32string> getPreviousFromActionHistory();
    std::optional<std::u32string> getPreviousFromSearchHistory();
    std::optional<std::u32string> getPreviousFromSearchHexHistory();
    std::list<std::u32string>::const_iterator getActionHistoryBegin() const;
    std::list<std::u32string>::const_iterator getSearchHistoryBegin() const;
    std::list<std::u32string>::const_iterator getSearchHexHistoryBegin() const;
    std::list<std::u32string>::const_iterator getActionHistoryEnd() const;
    std::list<std::u32string>::const_iterator getSearchHistoryEnd() const;
    std::list<std::u32string>::const_iterator getSearchHexHistoryEnd() const;
    reader::Reader const *getCurrentReader() const;
    ~NCursesBottomBarWindowDecorator();

  private:
    reader::Reader const *currentReader;
    std::list<std::u32string>::const_iterator actionHistoryIterator;
    std::list<std::u32string>::const_iterator searchHistoryIterator;
    std::list<std::u32string>::const_iterator searchHexHistoryIterator;
    std::list<std::u32string> actionHistory;
    std::list<std::u32string> searchHistory;
    std::list<std::u32string> searchHexHistory;
    std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedStrings;
  };
} // namespace feather::windows
