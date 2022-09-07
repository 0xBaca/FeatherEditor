#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif

#include "printer/ChangesStack.hpp"
#include "printer/PrintingOrchestratorInterface.hpp"
#include "printer/PrintingStorageInterface.hpp"
#include "utils/FilesystemInterface.hpp"
#include "utils/datatypes/UuidHash.hpp"
#include "utils/storage/AbstractStorageFactory.hpp"

#include <unordered_map>

namespace feather::printer
{
  class PrintingOrchestrator : public PrintingOrchestratorInterface
  {
#ifdef _FEATHER_TEST_
    FRIEND_TEST(PrintingOrchestratorTest, getProperPreviousPositionWithChanges);
    FRIEND_TEST(PrintingOrchestratorTest, getProperFileLastPosition);
    FRIEND_TEST(PrintingOrchestratorTest, getProperFileLastPositionWithDummyDeletions);
    FRIEND_TEST(PrintingOrchestratorTest, getCorrectNumberOfNewChangesBytesBetweenPositions);
    FRIEND_TEST(PrintingOrchestratorTest, getCorrectNumberOfDeletionsBytesBetweenPositions);
    FRIEND_TEST(PrintingOrchestratorTest, getNumberOfBytesTakenByNewChanges);
    FRIEND_TEST(PrintingOrchestratorTest, getNumberOfBytesTakenByBetween);
    FRIEND_TEST(PrintingOrchestratorTest, loadExistingChanges);
    FRIEND_TEST(PrintingOrchestratorTest, loadExistingChangesFileConcurrentlyModified);
    FRIEND_TEST(PrintingOrchestratorTest, loadExistingChangesFileConcurrentlyModifiedDontLoadChangesGreaterThanFileSize);
    FRIEND_TEST(PrintingOrchestratorTest, isCharAtPosDeleted);
    FRIEND_TEST(PrintingOrchestratorTest, getLastValidPosition);
    FRIEND_TEST(PrintingOrchestratorTest, getLastValidPosition_2);
    FRIEND_TEST(PrintingOrchestratorTest, getCharacterBytesTaken);
    FRIEND_TEST(PrintingOrchestratorTest, getNextVirtualPosition);
    FRIEND_TEST(PrintingOrchestratorTest, getNextVirtualPositionWithDeletions);
    FRIEND_TEST(PrintingOrchestratorTest, getPreviousVirtualPosition);
    FRIEND_TEST(PrintingOrchestratorTest, getPreviousVirtualPositionWithDeletions);
    FRIEND_TEST(PrintingOrchestratorTest, getPreviousVirtualPosition_2);
    FRIEND_TEST(PrintingOrchestratorTest, removeCharacters);
    FRIEND_TEST(PrintingOrchestratorTest, removeCharacters_2);
    FRIEND_TEST(PrintingOrchestratorTest, addCharacterToDeletions);
    FRIEND_TEST(PrintingOrchestratorTest, addCharacterToDummyDeletions);
    FRIEND_TEST(PrintingOrchestratorTest, addCharacterToNewChanges);
    FRIEND_TEST(PrintingOrchestratorTest, removeCharacter);
    FRIEND_TEST(PrintingOrchestratorTest, removeChangesRelatedToWindow);
    FRIEND_TEST(PrintingOrchestratorTest, convertVirtualPositionToByteOffset);
    FRIEND_TEST(PrintingOrchestratorTest, getDiffBytes);
    FRIEND_TEST(PrintingOrchestratorTest, getDiffToNextChange);
    FRIEND_TEST(PrintingOrchestratorTest, getDiffToNextDeletion);
    FRIEND_TEST(PrintingOrchestratorTest, getNumberOfChangesBytesBetweenPos);
    FRIEND_TEST(PrintingOrchestratorTest, undelete);
    FRIEND_TEST(PrintingOrchestratorIntegTest, addChanges);
    FRIEND_TEST(PrintingOrchestratorIntegTest, addChangesShiftDeletionsProperly);
    FRIEND_TEST(PrintingOrchestratorIntegTest, addChangesCacheFull);
    FRIEND_TEST(PrintingOrchestratorIntegTest, mergeNewChangesBothInCacheAndEnoughCacheSpace);
    FRIEND_TEST(PrintingOrchestratorIntegTest, mergeNewChangesBothInCacheAndNotEnoughCacheSpace);
    FRIEND_TEST(PrintingOrchestratorIntegTest, mergeNewChangesBothNotInCache);
    FRIEND_TEST(PrintingOrchestratorIntegTest, mergeNewChangesLeftSideInCache);
    FRIEND_TEST(PrintingOrchestratorIntegTest, mergeNewChangesRightSideInCache);
    FRIEND_TEST(PrintingOrchestratorIntegTest, removeCharacters);
#endif
  private:
    static std::unordered_map<utils::datatypes::Uuid, std::chrono::nanoseconds, utils::datatypes::UuidHasher> lastStorageModificationTimestamp;
    static std::unordered_map<utils::datatypes::Uuid, std::chrono::nanoseconds, utils::datatypes::UuidHasher> lastStoragePositionCheck;
    static std::unordered_map<utils::datatypes::Uuid, pair, utils::datatypes::UuidHasher> lastValidPosition;
    static std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> copiedLine;
    static std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> textToCopy;
    static std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> textToCopyPositionsToSkip;
    static std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> textToCut;
    static std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> textToCutSnapshot; 
    static std::vector<utils::datatypes::COLOR> BACKGROUND_COLORS;
    std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
    std::shared_ptr<utils::FilesystemInterface> filesystem;
    std::shared_ptr<PrintingStorageInterface> primaryPrintingStorage, secondaryPrintingStorage;
    std::shared_ptr<printer::ChangesStack> changesStack;
    std::unordered_map<utils::datatypes::Uuid, std::map<size_t, size_t>, utils::datatypes::UuidHasher> changes;
    mutable std::unordered_map<utils::datatypes::Uuid, std::set<utils::datatypes::COLOR>, utils::datatypes::UuidHasher> textHighlightColor;
    mutable std::unordered_map<utils::datatypes::Uuid, std::map<pair, pair>, utils::datatypes::UuidHasher> deletions;
    mutable std::unordered_map<utils::datatypes::Uuid, std::map<pair, pair>, utils::datatypes::UuidHasher> reverseDeletions;
    mutable std::unordered_map<utils::datatypes::Uuid, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>>, utils::datatypes::UuidHasher> highlightedStrings;
    mutable std::unordered_map<utils::datatypes::Uuid, std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>>, utils::datatypes::UuidHasher> highlightedTextChunks;
    mutable std::unordered_map<utils::datatypes::Uuid, std::pair<pair, pair>, utils::datatypes::UuidHasher> textHighlight;
    mutable std::unordered_map<utils::datatypes::Uuid, std::optional<std::pair<pair, size_t>>, utils::datatypes::UuidHasher> searchedText;
    std::unordered_map<utils::datatypes::Uuid, size_t, utils::datatypes::UuidHasher> totalBytesOfDeletions;
    std::unordered_map<utils::datatypes::Uuid, size_t, utils::datatypes::UuidHasher> totalBytesOfNewChanges;
    std::unordered_map<utils::datatypes::Uuid, size_t, utils::datatypes::UuidHasher> colorPriority;
    std::unordered_map<utils::datatypes::Uuid, size_t, utils::datatypes::UuidHasher> highlightedTextChunkPriority;
    std::pair<std::vector<char>, pair> readDataWithProperAlignment(pair, size_t, utils::datatypes::Uuid const &);
    void loadExistingChanges(std::unique_ptr<utils::FilesystemInterface>, std::shared_ptr<utils::storage::AbstractStorage> const &);
    std::pair<size_t, pair> getDiffToPrevChanges(pair, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &);
    std::pair<size_t, pair> getDiffToPrevReverseDeletion(pair, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &);
    size_t getNumberOfChangesBytesBetweenPos(size_t, size_t, utils::datatypes::Uuid const &) const;
    void updateTextCopyPositionsToSkip(pair const &, size_t, utils::datatypes::Uuid const &);

  public:
    PrintingOrchestrator(std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<PrintingStorageInterface>, std::shared_ptr<PrintingStorageInterface>, std::shared_ptr<printer::ChangesStack>, std::shared_ptr<utils::FilesystemInterface>, std::shared_ptr<utils::storage::AbstractStorage> const & = nullptr);
    std::map<pair, pair> getChangesPositions(utils::datatypes::Uuid const &) override;
    std::map<pair, pair> getDeletionsPositions(utils::datatypes::Uuid const &) override;
    std::map<pair, pair> getReverseDeletionsPositions(utils::datatypes::Uuid const &) override;
    std::vector<char> getNewChanges(utils::datatypes::Uuid const &, size_t, size_t, size_t) const override;
    std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &getHighlightedStrings(utils::datatypes::Uuid const &) const override;
    std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &getHighlitedTextChunks(utils::datatypes::Uuid const &) const override;
    std::optional<std::pair<pair, pair>> getTextHighlight(utils::datatypes::Uuid const &) const override;
    std::set<utils::datatypes::COLOR> getTextHighlightColor(utils::datatypes::Uuid const &) const override;
    bool isAllCharactersRemoved(utils::datatypes::Uuid const &, size_t) override;
    bool isAnyChangesMade(utils::datatypes::Uuid const &) override;
    bool isCharAtFirstPosition(pair, utils::datatypes::Uuid const &) const override;
    bool isCharAtPosDeleted(pair, utils::datatypes::Uuid const &) const override;
    bool isCharAtPosWithChanges(size_t, utils::datatypes::Uuid const &) const override;
    bool isPreviousCharacterDeleted(pair, utils::datatypes::Uuid const &) const override;
    bool isLineCopied() const override;
    bool isTextCopied() const override;
    bool isTextCut() const override;
    bool isLineCopied(utils::datatypes::Uuid const &) const override;
    bool isTextCopied(utils::datatypes::Uuid const &) const override;
    bool isTextCut(utils::datatypes::Uuid const &) const override;
    void setLineToCopy(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &&, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&) override;
    void setTextToCopy(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> const &, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&) override;
    void setTextToCopyPositionsToSkip(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> const &) override;
    void setTextToCut(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &&, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&) override;
    pair getDeletionStart(utils::datatypes::Uuid const &, pair) override;
    pair getDeletionEnd(utils::datatypes::Uuid const &, pair) override;
    pair getLastValidPosition(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>) override;
    pair getNextVirtualPosition(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, pair, bool = false) override;
    pair getPreviousVirtualPosition(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, pair) override;
    pair removeCharacters(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, pair, pair, bool = true, std::optional<pair> = std::nullopt, std::optional<pair> = std::nullopt) override;
    void setSearchedText(utils::datatypes::Uuid const &, std::optional<std::pair<pair, size_t>>) override;
    std::optional<std::pair<pair, size_t>> getSearchedText(utils::datatypes::Uuid const &) const override;
    size_t convertVirtualPositionToByteOffset(pair, utils::datatypes::Uuid const &) const override;
    unsigned short getCharacterBytesTaken(pair, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &) const override;
    std::optional<std::chrono::nanoseconds> getLastStorageModificationTimestamp(utils::datatypes::Uuid const &) const override;
    size_t getDiffBytes(pair, pair, utils::datatypes::Uuid const &) const override;
    size_t getDiffBytesWithoutDeletions(pair, pair, utils::datatypes::Uuid const &, std::map<pair, pair> const & = std::map<pair, pair>()) const override;
    size_t getDiffToNextChange(pair, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &) override;
    size_t getDiffToNextDeletion(pair, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &) override;
    size_t getNumberOfChangesBytesAtPos(size_t, utils::datatypes::Uuid const &) const override;
    pair getFirstInvalidPosition(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>) override;
    pair getFirstValidPosition(utils::datatypes::Uuid const &) const override;
    size_t getTotalBytesOfDeletions(utils::datatypes::Uuid const &) const override;
    size_t getTotalBytesOfNewChanges(utils::datatypes::Uuid const &) const override;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> getLineToCopy() const override;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &getTextToCopy() const override;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> getTextToCopyPositionsToSkip() const override;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> getTextToCut() const override;
    void addChanges(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, pair, std::vector<char> const &, size_t, bool = false, std::optional<pair> = std::nullopt, std::optional<pair> = std::nullopt) override;
    std::set<utils::datatypes::COLOR> highlightSelection(std::pair<pair, pair> const &, utils::datatypes::Uuid const &, std::set<utils::datatypes::COLOR> = {}) override;
    void applyDeletions(std::map<pair, pair>, utils::datatypes::Uuid const &) override;
    size_t dumpCache(utils::datatypes::Uuid const &) override;
    void highlightSelection(std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>>, utils::datatypes::Uuid const &) override;
    void highlightText(std::u32string, std::set<utils::datatypes::COLOR>, utils::datatypes::Uuid const &) override;
    std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightTextChunk(std::pair<pair, pair> const &, utils::datatypes::Uuid const &) override;
    void normalizeAfterSave(utils::datatypes::Uuid const &) override;
    void undelete(pair, pair, utils::datatypes::Uuid const &) override;
    void unhighlightSelection(std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>>, utils::datatypes::Uuid const &) override;
    void unhighlightText(std::u32string, utils::datatypes::Uuid const &) override;
    void updateHighlightedTextChunks(pair, size_t, utils::datatypes::Uuid const &) override;
    void removeChangesRelatedToWindow(utils::datatypes::Uuid const &) override;
    void removeCharacters(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::map<pair, pair>, bool = true) override;
    void unhighlightSelection(utils::datatypes::Uuid const &) override;
    void unhighlightAllSelection(utils::datatypes::Uuid const &) override;
    std::pair<std::basic_string<char>, pair> getContinousCharactersAsString(pair, size_t, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &, bool = false, std::map<pair, pair> const & = {}) override;
    std::pair<std::vector<char>, pair> getContinousCharacters(pair, size_t, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &, bool = false, std::map<pair, pair> const & = {}) override;
    std::pair<std::vector<char>, pair> getContinousCharactersBackward(pair, size_t, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &) override;
    std::pair<pair, pair> getRangeIncludingDeletions(std::pair<pair, pair>, utils::datatypes::Uuid const &) override;
    void swapDeletions(utils::datatypes::DeletionsSnapshot const &, utils::datatypes::Uuid const &) override;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> getTextToCutSnapshot() const override;
    virtual ~PrintingOrchestrator();
  };
} // namespace feather::printer
