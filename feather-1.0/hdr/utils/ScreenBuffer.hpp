#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif
#include "utils/FeatherMode.hpp"
#include "utils/datatypes/Colors.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/Hash.hpp"

#include <map>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace feather::utils
{
  class ScreenBuffer
  {
#ifdef _FEATHER_TEST_
    FRIEND_TEST(ScreenBufferTest, applyBlinkingForHexString);
#endif
  public:
    ScreenBuffer(size_t = 0);
    ScreenBuffer(size_t, size_t, size_t);
    ScreenBuffer(std::vector<char32_t> &&, size_t);
    ScreenBuffer(std::vector<char32_t> &&, size_t, size_t);
    ScreenBuffer(std::vector<char32_t> &, size_t);
    ScreenBuffer(ScreenBuffer &&);
    ScreenBuffer(ScreenBuffer const &);
    ScreenBuffer &operator=(ScreenBuffer const &);
    bool operator==(utils::ScreenBuffer &);
    std::vector<char32_t> &operator[](size_t);
    std::vector<char32_t> const &operator[](size_t) const;
    bool isBracket(pair) const;
    bool isBufferEmpty() const;
    bool isPosMappedToHex(pair) const;
    bool isSearchedTextPos(pair) const;
    void applyBlinking(std::pair<pair, pair> const &);
    void applyColors(std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &) const;
    bool highlightBrackets(pair, bool) const;
    void unhighlightBrackets() const;
    void decorateBuffer(ScreenBuffer const &, bool = false);
    void decorateAsBottomBarWindowBuffer(ScreenBuffer const &);
    void fillBufferLine(std::u32string &&);
    void fillBufferLine(std::u32string const &);
    std::optional<char32_t> getCharAt(pair) const;
    void rotateLine(size_t);
    size_t getFilledRows() const;
    size_t mapScreenBufferColumnToDecoratedColumnStart(size_t, size_t, bool) const;
    size_t mapScreenBufferColumnToDecoratedColumnEnd(size_t, size_t, utils::FEATHER_MODE, bool) const;
    std::u32string const &getBufferAsString() const;
    std::set<utils::datatypes::COLOR> getColor(size_t, size_t) const;
    std::unordered_set<pair, pair_hash> getHighlightedBrackets() const;
    void setHighlightedBrackets(std::unordered_set<pair, pair_hash> const &);
    void setHighlightedTextChunks(std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &);
    void setHighlightedText(std::pair<pair, pair> const &, std::set<utils::datatypes::COLOR>);
    ~ScreenBuffer();

  private:
    std::optional<std::pair<std::pair<pair, pair>, std::set<utils::datatypes::COLOR>>> highlightedText;
    std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedTextChunks;
    mutable std::unordered_map<pair, pair, utils::datatypes::PairHash> blinkMap;
    mutable std::unordered_multimap<pair, std::pair<pair, std::pair<size_t, std::set<utils::datatypes::COLOR>>>, utils::datatypes::PairHash> colorMap;
    std::unordered_set<pair, pair_hash> posMappedToHex;
    std::unordered_set<pair, pair_hash> posMappedToHexStart;
    mutable std::unordered_set<pair, pair_hash> highlightedBrackets;
    std::vector<std::vector<char32_t>> internalBuffer;
    std::u32string internalBufferAsString;
    size_t lastFilledLine, noRows;
  };
} // namespace feather::utils