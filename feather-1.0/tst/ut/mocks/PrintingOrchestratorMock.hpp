#include "printer/PrintingOrchestratorInterface.hpp"

#include <gmock/gmock.h>

using namespace feather::printer;

namespace testing::mocks
{
  class PrintingOrchestratorMock : public PrintingOrchestratorInterface
  {

    virtual void setLineToCopy(std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>> &&arg1, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, feather::utils::datatypes::DeletionsSnapshot>> &&arg2)
    {
      setLineToCopyMock(arg1, arg2);
    }

    virtual void setTextToCopy(std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>> const &arg1, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, feather::utils::datatypes::DeletionsSnapshot>> &&arg2)
    {
      setTextToCopyMock(arg1, arg2);
    }

    virtual void setTextToCut(std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>> &&arg1, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, feather::utils::datatypes::DeletionsSnapshot>> &&arg2)
    {
      setTextToCutMock(arg1, arg2);
    }
  public:
    MOCK_CONST_METHOD1(getFirstValidPosition, feather::pair(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getHighlightedStrings, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<feather::utils::datatypes::COLOR>>> const &(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getHighlitedTextChunks, std::multimap<std::pair<feather::pair, feather::pair>, std::pair<size_t, std::set<feather::utils::datatypes::COLOR>>> const &(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getLastStorageModificationTimestamp, std::optional<std::chrono::nanoseconds>(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getNumberOfDeletedBytesAtDummyDeletions, size_t(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getNumberOfDummyDeletions, size_t(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getSearchedText, std::optional<std::pair<feather::pair, size_t>>(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getTextHighlight, std::optional<std::pair<feather::pair, feather::pair>>(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getTextHighlightColor, std::set<feather::utils::datatypes::COLOR>(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getTotalBytesOfDeletions, size_t(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(getTotalBytesOfNewChanges, size_t(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(convertVirtualPositionToByteOffset, size_t(feather::pair, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(getLastNotDeletedRealPosition, size_t(feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>));
    MOCK_CONST_METHOD2(getNumberOfChangesAtPos, size_t(size_t, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(getNumberOfChangesBytesAtPos, size_t(size_t, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(getNumberOfDeletionsAtPos, size_t(size_t, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(isCharAtFirstPosition, bool(feather::pair, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(isCharAtPosDeleted, bool(feather::pair, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(isCharAtPosWithChanges, bool(size_t, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(isCharAtPosWithDeletions, bool(size_t, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD2(isPreviousCharacterDeleted, bool(feather::pair, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD3(getCharacterBytesTaken, unsigned short(feather::pair, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD3(getDiffBytes, size_t(feather::pair, feather::pair, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD3(getNumberOfChangesBytesTillPos, size_t(size_t, size_t, feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD4(getDiffBytesWithoutDeletions, size_t(feather::pair, feather::pair, feather::utils::datatypes::Uuid const &, std::map<feather::pair, feather::pair> const &));
    MOCK_CONST_METHOD4(getNewChanges, std::vector<char>(feather::utils::datatypes::Uuid const &, size_t, size_t, size_t));
    MOCK_CONST_METHOD5(getDiffBytesWithoutLocalDeletions, size_t(feather::pair, feather::pair, feather::utils::datatypes::Uuid const &, std::map<feather::pair, feather::pair> const &, std::map<feather::pair, feather::pair> const &));
    MOCK_METHOD1(dumpCache, size_t(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD1(getChangesPositions, std::map<feather::pair, feather::pair>(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD1(getDeletionsPositions, std::map<feather::pair, feather::pair>(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD1(isAnyChangesMade, bool(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD1(normalizeColorsAfterSave, void(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD1(removeChangesRelatedToWindow, void(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD1(unhighlightAllSelection, void(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD1(unhighlightSelection, void(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD2(applyDeletions, void(std::map<feather::pair, feather::pair>, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD2(getDeletionEnd, feather::pair(feather::utils::datatypes::Uuid const &, feather::pair));
    MOCK_METHOD2(getDeletionStart, feather::pair(feather::utils::datatypes::Uuid const &, feather::pair));
    MOCK_METHOD2(getFirstInvalidPosition, feather::pair(feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>));
    MOCK_METHOD2(getLastValidPosition, feather::pair(feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>));
    MOCK_METHOD2(highlightSelection, void(std::pair<std::pair<feather::pair, feather::pair>, std::pair<size_t, std::set<feather::utils::datatypes::COLOR>>>, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD2(highlightTextChunk, std::pair<std::pair<feather::pair, feather::pair>, std::pair<size_t, std::set<feather::utils::datatypes::COLOR>>>(std::pair<feather::pair, feather::pair> const &, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD2(isAllCharactersRemoved, bool(feather::utils::datatypes::Uuid const &, size_t));
    MOCK_METHOD2(setSearchedText, void(feather::utils::datatypes::Uuid const &, std::optional<std::pair<feather::pair, size_t>>));
    MOCK_METHOD2(unhighlightSelection, void(std::pair<std::pair<feather::pair, feather::pair>, std::pair<size_t, std::set<feather::utils::datatypes::COLOR>>>, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD2(unhighlightText, void(std::u32string, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD3(getDiffToNextChange, size_t(feather::pair, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD3(getDiffToNextDeletion, size_t(feather::pair, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD3(getPreviousVirtualPosition, feather::pair(feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::pair));
    MOCK_METHOD3(highlightSelection, std::set<feather::utils::datatypes::COLOR>(std::pair<feather::pair, feather::pair> const &, feather::utils::datatypes::Uuid const &, std::set<feather::utils::datatypes::COLOR>));
    MOCK_METHOD3(highlightText, void(std::u32string, std::set<feather::utils::datatypes::COLOR>, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD3(removeCharacter, void(std::unique_ptr<feather::windows::MainWindowInterface> &, std::shared_ptr<feather::utils::storage::AbstractStorage>, std::optional<feather::pair>));
    MOCK_METHOD3(undelete, void(feather::pair, feather::pair, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD3(updateHighlightedTextChunks, void(feather::pair, size_t, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD4(getContinousCharactersBackward, std::pair<std::vector<char>, feather::pair>(feather::pair, size_t, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD4(getNextVirtualPosition, feather::pair(feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::pair, bool));
    MOCK_METHOD4(isCharAtPos, bool(feather::pair, std::function<bool(char32_t)>, feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>));
    MOCK_METHOD4(removeCharacters, void(feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>, std::map<feather::pair, feather::pair>, bool));
    MOCK_METHOD6(getContinousCharacters, std::pair<std::vector<char>, feather::pair>(feather::pair, size_t, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::utils::datatypes::Uuid const &, bool, std::map<feather::pair, feather::pair> const &));
    MOCK_METHOD6(getContinousCharactersAsString, std::pair<std::basic_string<char>, feather::pair>(feather::pair, size_t, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::utils::datatypes::Uuid const &, bool, std::map<feather::pair, feather::pair> const &));
    MOCK_METHOD7(removeCharacters, feather::pair(feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::pair, feather::pair, bool, std::optional<feather::pair>, std::optional<feather::pair>));
    MOCK_METHOD8(addChanges, void(feather::utils::datatypes::Uuid const &, std::shared_ptr<feather::utils::storage::AbstractStorage>, feather::pair, std::vector<char> const &, size_t, bool, std::optional<feather::pair>, std::optional<feather::pair>));
    MOCK_CONST_METHOD0(isLineCopied, bool());
    MOCK_CONST_METHOD0(isTextCopied, bool());
    MOCK_CONST_METHOD0(isTextCut, bool());
    MOCK_CONST_METHOD1(isLineCopied, bool(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(isTextCopied, bool(feather::utils::datatypes::Uuid const &));
    MOCK_CONST_METHOD1(isTextCut, bool(feather::utils::datatypes::Uuid const &));
    MOCK_METHOD2(swapDeletions, void(feather::utils::datatypes::DeletionsSnapshot const &, feather::utils::datatypes::Uuid const &));
    MOCK_METHOD2(getRangeIncludingDeletions, std::pair<feather::pair, feather::pair>(std::pair<feather::pair, feather::pair>, feather::utils::datatypes::Uuid const &));

    MOCK_CONST_METHOD0(getTextToCutSnapshot, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, feather::utils::datatypes::DeletionsSnapshot>>());

    MOCK_METHOD1(getReverseDeletionsPositions, std::map<feather::pair, feather::pair>(feather::utils::datatypes::Uuid const &));

    MOCK_METHOD1(normalizeAfterSave, void(feather::utils::datatypes::Uuid const &));

    MOCK_METHOD2(setTextToCutMock, void(std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>> &, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, feather::utils::datatypes::DeletionsSnapshot>> &));

    MOCK_METHOD1(setTextToCopyPositionsToSkip, void(std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::map<feather::pair, feather::pair>>> const &));

    MOCK_METHOD2(setTextToCopyMock, void(std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>> const &, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, feather::utils::datatypes::DeletionsSnapshot>> &));

    MOCK_METHOD2(setLineToCopyMock, void(std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>> &, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, feather::utils::datatypes::DeletionsSnapshot>> &));
    MOCK_CONST_METHOD0(getLineToCopy, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>>());
    MOCK_CONST_METHOD0(getTextToCopy, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>> &());
    MOCK_CONST_METHOD0(getTextToCopyPositionsToSkip, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::map<feather::pair, feather::pair>>> ());
    MOCK_CONST_METHOD0(getTextToCut, std::optional<std::pair<std::pair<feather::utils::datatypes::Uuid, std::string>, std::pair<feather::pair, feather::pair>>> ());
 
  };
} // namespace testing::mocks
