#pragma once

#include "utils/Direction.hpp"
#include "utils/datatypes/Colors.hpp"
#include "utils/datatypes/DeletionsSnapshot.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/Uuid.hpp"
#include "utils/storage/AbstractStorage.hpp"

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>

namespace feather::printer
{
  class PrintingOrchestratorInterface
  {
  public:
    virtual bool isAllCharactersRemoved(utils::datatypes::Uuid const &, size_t) = 0;
    virtual bool isAnyChangesMade(utils::datatypes::Uuid const &) = 0;
    virtual bool isCharAtFirstPosition(pair, utils::datatypes::Uuid const &) const = 0;
    virtual bool isCharAtPosDeleted(pair, utils::datatypes::Uuid const &) const = 0;
    virtual bool isCharAtPosWithChanges(size_t, utils::datatypes::Uuid const &) const = 0;
    virtual bool isPreviousCharacterDeleted(pair, utils::datatypes::Uuid const &) const = 0;
    virtual pair getDeletionStart(utils::datatypes::Uuid const &, pair) = 0;
    virtual pair getDeletionEnd(utils::datatypes::Uuid const &, pair) = 0;
    virtual pair getFirstInvalidPosition(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>) = 0;
    virtual pair getFirstValidPosition(utils::datatypes::Uuid const &) const = 0;
    virtual pair getLastValidPosition(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>) = 0;
    virtual pair getNextVirtualPosition(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, pair, bool = false) = 0;
    virtual pair getPreviousVirtualPosition(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, pair) = 0;
    virtual std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &getHighlightedStrings(utils::datatypes::Uuid const &) const = 0;
    virtual std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &getHighlitedTextChunks(utils::datatypes::Uuid const &) const = 0;
    virtual std::optional<std::pair<pair, pair>> getTextHighlight(utils::datatypes::Uuid const &) const = 0;
    virtual std::set<utils::datatypes::COLOR> getTextHighlightColor(utils::datatypes::Uuid const &) const = 0;
    virtual std::optional<std::pair<pair, size_t>> getSearchedText(utils::datatypes::Uuid const &) const = 0;
    virtual pair removeCharacters(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, pair, pair, bool = true, std::optional<pair> = std::nullopt, std::optional<pair> = std::nullopt) = 0;
    virtual unsigned short getCharacterBytesTaken(pair, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &) const = 0;
    virtual std::optional<std::chrono::nanoseconds> getLastStorageModificationTimestamp(utils::datatypes::Uuid const &) const = 0;
    virtual size_t convertVirtualPositionToByteOffset(pair, utils::datatypes::Uuid const &) const = 0;
    virtual size_t dumpCache(utils::datatypes::Uuid const &) = 0;
    virtual size_t getDiffBytes(pair, pair, utils::datatypes::Uuid const &) const = 0;
    virtual size_t getDiffBytesWithoutDeletions(pair, pair, utils::datatypes::Uuid const &, std::map<pair, pair> const & = std::map<pair, pair>()) const = 0;
    virtual size_t getDiffToNextChange(pair, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &) = 0;
    virtual size_t getDiffToNextDeletion(pair, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &) = 0;
    virtual size_t getNumberOfChangesBytesAtPos(size_t, utils::datatypes::Uuid const &) const = 0;
    virtual size_t getTotalBytesOfDeletions(utils::datatypes::Uuid const &) const = 0;
    virtual size_t getTotalBytesOfNewChanges(utils::datatypes::Uuid const &) const = 0;
    virtual void addChanges(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, pair, std::vector<char> const &, size_t, bool = false, std::optional<pair> = std::nullopt, std::optional<pair> = std::nullopt) = 0;
    virtual void applyDeletions(std::map<pair, pair>, utils::datatypes::Uuid const &) = 0;
    virtual void highlightSelection(std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>>, utils::datatypes::Uuid const &) = 0;
    virtual std::set<utils::datatypes::COLOR> highlightSelection(std::pair<pair, pair> const &, utils::datatypes::Uuid const &, std::set<utils::datatypes::COLOR> = {}) = 0;
    virtual void highlightText(std::u32string, std::set<utils::datatypes::COLOR>, utils::datatypes::Uuid const &) = 0;
    virtual bool isLineCopied() const = 0;
    virtual bool isTextCopied() const = 0;
    virtual bool isTextCut() const = 0;
    virtual bool isLineCopied(utils::datatypes::Uuid const &) const = 0;
    virtual bool isTextCopied(utils::datatypes::Uuid const &) const = 0;
    virtual bool isTextCut(utils::datatypes::Uuid const &) const = 0;
    virtual std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> getLineToCopy() const = 0;
    virtual std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &getTextToCopy() const = 0;
    virtual std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> getTextToCopyPositionsToSkip() const = 0;
    virtual std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> getTextToCut() const = 0;
    virtual void setLineToCopy(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &&, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&) = 0;
    virtual void setTextToCopy(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> const &, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&) = 0;
    virtual void setTextToCopyPositionsToSkip(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> const &) = 0;
    virtual void setTextToCut(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &&, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&) = 0;
    virtual std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightTextChunk(std::pair<pair, pair> const &, utils::datatypes::Uuid const &) = 0;
    virtual void normalizeAfterSave(utils::datatypes::Uuid const &)  = 0;
    virtual void undelete(pair, pair, utils::datatypes::Uuid const &) = 0;
    virtual void unhighlightSelection(std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>>, utils::datatypes::Uuid const &) = 0;
    virtual void updateHighlightedTextChunks(pair, size_t, utils::datatypes::Uuid const &) = 0;
    virtual void removeChangesRelatedToWindow(utils::datatypes::Uuid const &) = 0;
    virtual void removeCharacters(utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::map<pair, pair>, bool = true) = 0;
    virtual void setSearchedText(utils::datatypes::Uuid const &, std::optional<std::pair<pair, size_t>>) = 0;
    virtual void unhighlightSelection(utils::datatypes::Uuid const &) = 0;
    virtual void unhighlightText(std::u32string, utils::datatypes::Uuid const &) = 0;
    virtual void unhighlightAllSelection(utils::datatypes::Uuid const &) = 0;
    virtual std::map<pair, pair> getChangesPositions(utils::datatypes::Uuid const &) = 0;
    virtual std::map<pair, pair> getDeletionsPositions(utils::datatypes::Uuid const &) = 0;
    virtual std::map<pair, pair> getReverseDeletionsPositions(utils::datatypes::Uuid const &) = 0;
    virtual std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> getTextToCutSnapshot() const = 0;
    virtual std::pair<std::basic_string<char>, pair> getContinousCharactersAsString(pair, size_t, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &, bool = false, std::map<pair, pair> const & = {}) = 0;
    virtual std::pair<std::vector<char>, pair> getContinousCharacters(pair, size_t, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &, bool = false, std::map<pair, pair> const & = {}) = 0;
    virtual std::pair<std::vector<char>, pair> getContinousCharactersBackward(pair, size_t, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &) = 0;
    virtual std::vector<char> getNewChanges(utils::datatypes::Uuid const &, size_t, size_t, size_t) const = 0;
    virtual std::pair<pair, pair> getRangeIncludingDeletions(std::pair<pair, pair>, utils::datatypes::Uuid const &) = 0;
    virtual void swapDeletions(utils::datatypes::DeletionsSnapshot const &, utils::datatypes::Uuid const &) = 0;
    virtual ~PrintingOrchestratorInterface();
  };
} // namespace feather::printer