#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/FeatherMode.hpp"
#include "utils/FilesystemInterface.hpp"
#include "windows/SubWindowInterface.hpp"

#include <list>
#include <unordered_set>

namespace feather {
   namespace windows {
      class WindowsManager;
   }
}

namespace feather::utils::helpers
{
   struct Lambda
   {
      static std::function<bool()> isScreenBigEnough;
      static std::function<bool(char32_t)> characterBelongToWord;
      static std::function<bool(char32_t)> characterDontBelongToWord;
      static std::function<bool(char32_t)> characterIsNotWhiteCharacter;
      static std::function<bool(char32_t)> isBackspace;
      static std::function<bool(char32_t)> isBracket;
      static std::function<bool(char32_t)> isClosingBracket;
      static std::function<bool(char32_t)> isDigit;
      static std::function<bool(char32_t)> isNewLineChar;
      static std::function<bool(char32_t)> isNonSpacingUtf8;
      static std::function<bool(char32_t)> isNotNewLineChar;
      static std::function<bool(char32_t)> isNotWhiteChar;
      static std::function<bool(char32_t)> isOpenBracket;
      static std::function<bool(char32_t)> isReadModeChar;
      static std::function<bool(char32_t)> isSpace;
      static std::function<bool(char32_t)> isTabChar;
      static std::function<bool(char32_t)> isWhiteChar;
      static std::function<bool(char32_t)> isWhiteCharOrNewLine;
      static std::function<bool(char32_t, bool)> isControlOrUnhandledChar;
      static std::function<bool(char32_t, bool)> isRightToLeftUtf8;
      static std::function<bool(pair, size_t)> isCursorAtFirstPosition;
      static std::function<bool(std::pair<pair, pair>, std::map<pair, pair> const &)> intervalsOverlap;
      static std::function<bool(std::shared_ptr<utils::storage::AbstractStorage>)> isValidFeatherStorage;
      static std::function<bool(utils::FEATHER_MODE)> isSearchMode;
      static std::function<pair(pair, bool, bool, utils::FEATHER_MODE, utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorage>)> getLastValidPosInReadMode;
      static std::function<pair(pair, size_t, printer::PrintingOrchestratorInterface *, utils::datatypes::Uuid const &)> moveVirtualPositionBackward;
      static std::function<pair(pair, size_t, printer::PrintingOrchestratorInterface *, utils::datatypes::Uuid const &, bool)> moveVirtualPositionForward;
      static std::function<pair(pair, size_t, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, bool)> convertByteOffsetToPosition;
      static std::function<pair(pair, size_t, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::basic_string<char> const &)> convertBufferByteOffsetToPosition;
      static std::function<pair(pair, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::unique_ptr<utils::BufferFillerInterface> const &, utils::datatypes::Uuid const &, size_t, bool, bool)> getCursorCoordinatesFromRealPosition;
      static std::function<pair(pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &)> alignToProperPosition;
      static std::function<size_t(size_t, std::string)> convertToBytes;
      static std::function<size_t(std::u32string const &, size_t, size_t, bool, bool)> getStringWidth;
      static std::function<size_t(std::u32string const &, size_t, size_t, bool, bool)> getLastIndexInWindow;
      static std::function<size_t(std::u32string const &, size_t, size_t, size_t)> getIndexFromOffset;
      static std::function<std::pair<pair, pair>(std::pair<pair, pair> const &, pair const &, size_t)> shiftInterval;
      static std::function<std::map<pair, pair>(std::map<pair, pair> const &, pair const &, size_t, std::optional<pair>)> shiftIntervals;
      static std::function<std::map<pair, pair>(std::map<pair, pair> const &, pair, pair)> disjoin;
      static std::function<std::map<pair, pair>(std::map<pair, pair> const &, pair, pair)> disjoinDeletions;
      static std::function<std::map<pair, pair>(std::map<pair, pair> const &, std::map<pair, pair> const &)> mergeIntervals;
      static std::function<std::map<pair, pair>(std::map<pair, pair> const &, utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, size_t)> mergeInterval;
      static std::function<std::pair<bool, std::optional<char32_t>>(pair, std::function<bool(char32_t)>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<printer::PrintingOrchestratorInterface>)> isCharAtPos;
      static std::function<std::pair<pair, pair>(std::pair<pair, pair>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::unique_ptr<utils::BufferFillerInterface> const &, utils::datatypes::Uuid const &, size_t, bool)> getScreenBufferPositions;
      static std::function<std::pair<pair, std::optional<char32_t>>(pair, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::windows::WindowsManager>, bool)> findPrevious;
      static std::function<std::pair<pair, std::optional<char32_t>>(pair, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::windows::WindowsManager>, bool, bool)> findNext;
      static std::function<std::pair<pair, std::optional<char32_t>>(pair, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::windows::WindowsManager>, size_t, bool, bool)> findNthNext;
      static std::function<std::pair<size_t, std::string>(std::string)> getValueWithUnit;
      static std::function<std::pair<std::basic_string<wchar_t>, std::vector<unsigned int>>(std::basic_string<char> const &)> charToWchar;
      static std::function<std::set<pair>(std::set<pair> const &, pair const &, size_t)> shiftSetPairs;
      static std::function<std::set<std::u32string>(std::u32string const &)> getAllFilesInDirectory;
      static std::function<std::string(std::string &&)> alignBufferBeginingForString;
      static std::function<std::string(std::string const&, std::u32string, std::shared_ptr<utils::FilesystemInterface>)> getAbsolutePath;
      static std::function<std::string(std::string)> getFolderName;
      static std::function<std::u32string(std::string)> mapStringToWideString;
      static std::function<std::u32string(std::u32string const &)> fixPath;
      static std::function<std::u32string(std::u32string)> convertStrigifiedHexToHex;
      static std::function<std::vector<char>(std::string const &, size_t, size_t)> getSubVectorFromString;
      static std::function<std::vector<char>(std::vector<char> &&)> alignBufferBegining;
      static std::function<std::basic_string<char>(std::basic_string<char> const &, size_t, size_t)> getSubString;
      static std::function<std::vector<char>(std::vector<char> const &, size_t, size_t)> getSubVector;
      static std::function<unsigned long(char32_t, bool, bool)> getCharacterWidth;
      static std::function<void(std::list<pair> &, pair const &, size_t, std::list<pair>::iterator &)> shiftPairs;
      static std::function<void(std::shared_ptr<feather::windows::WindowsManager>, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &, pair)> setCursor;
      static std::function<void(std::shared_ptr<feather::windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &, utils::FEATHER_MODE)> restoreFeatherMode;
      static std::function<void(std::u32string const &, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &)> subWindowPrintWithoutDelay;
      static std::function<void(std::u32string const &, std::optional<std::chrono::milliseconds>, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &)> subWindowPrintMs;
      static std::function<void(std::u32string const &, std::optional<std::chrono::seconds>, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &)> subWindowPrint;
      static std::function<void(std::u32string const &, std::optional<std::chrono::seconds>, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &)> subWindowPrintWithoutDependent;
      static std::function<void(std::u32string const &, std::u32string const &, std::chrono::seconds, std::unique_ptr<feather::windows::SubWindowInterface> &)> blinkPrint;
      static std::function<void(utils::datatypes::Uuid const &, std::string)> removeFilesContainingSubstring;
      static std::map<char32_t, char32_t> closedToOpenBracket;
      static std::map<char32_t, char32_t> openToCloseBracket;
      static std::set<std::pair<int, int>> rightToLeftUtf8CodePointsPairs;
      static std::unordered_set<char32_t> parentheses;
      static std::unordered_set<char> charactersNotBelongingToWord;
      static std::unordered_set<int> rightToLeftUtf8CodePoints;
      static std::unordered_set<unsigned int> nonSpacingUtf8;
   };
} // namespace feather::utils::helpers