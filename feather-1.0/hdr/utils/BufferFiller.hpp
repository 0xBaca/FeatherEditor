#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif
#include "utils/BufferFillerInterface.hpp"
#include "utils/ScreenBuffer.hpp"
#include "utils/algorithm/SearchBuffer.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/storage/AbstractStorageFactory.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::utils
{
  class BufferFiller : public BufferFillerInterface
  {
#ifdef _FEATHER_TEST_
    FRIEND_TEST(BufferFillerTest, readWholeFileWithChangesAsLessThanLineSize);
    FRIEND_TEST(BufferFillerTest, readPartOfFileAsMoreThanLineSize);
    FRIEND_TEST(BufferFillerTest, readPartOfFileAsMoreThanLineSizeUTF8);
    FRIEND_TEST(BufferFillerTest, readWholeFileWithChanges);
    FRIEND_TEST(BufferFillerTest, readPartOfFileAsFileHasNewLine);
    FRIEND_TEST(BufferFillerTest, forwardOneLine);
    FRIEND_TEST(BufferFillerTest, forwardOneLineByHitEndOfFile);
    FRIEND_TEST(BufferFillerTest, forwardOneLineButHitNewLineInFile);
    FRIEND_TEST(BufferFillerTest, rewindWhenNoNewChangesFitsNoColumns);
    FRIEND_TEST(BufferFillerTest, rewindChnagesWhereNoColumnsLessThanExactNumberOfChanges);
    FRIEND_TEST(BufferFillerTest, rewindLineWithChangesGraterThanLineSize);
    FRIEND_TEST(BufferFillerTest, rewindLineWithChangesGraterThanLineSize2);
    FRIEND_TEST(BufferFillerTest, rewindLineWithChangesGraterThanLineSize3);
    FRIEND_TEST(BufferFillerTest, rewindLineWithChangesGraterThanLineSize4);
    FRIEND_TEST(BufferFillerTest, rewindLineWhenChangesLessThanLineSize);
    FRIEND_TEST(BufferFillerTest, rewindLineWhenChangesLessThanLineSize2);
    FRIEND_TEST(BufferFillerTest, rewindLineWhenNewChnagesAndOnlyNewLines);
    FRIEND_TEST(BufferFillerTest, rewindLineWhenDummyDeletions);
    FRIEND_TEST(BufferFillerTest, readChanges_1);
    FRIEND_TEST(BufferFillerTest, readChanges_2);
    FRIEND_TEST(BufferFillerTest, readChanges_3);
    FRIEND_TEST(BufferFillerTest, readChanges_4);
    FRIEND_TEST(BufferFillerTest, readCorrectNumberOfLines);
    FRIEND_TEST(BufferFillerTest, readCorrectNumberOfLines_UTF8);
    FRIEND_TEST(BufferFillerTest, readCorrectNumberOfLinesWithDummyDeletions);
    FRIEND_TEST(BufferFillerTest, addNewLineCharacterIfFileIsEmptyOrOnlyNewChanges);
    FRIEND_TEST(BufferFillerTest, UTF8ForwardThenUndoOneLine);
    FRIEND_TEST(BufferFillerTest, rewindLineWhenNewLineWithChangesAbove);
    FRIEND_TEST(BufferFillerIntegTest, fillCacheBuffer);
    FRIEND_TEST(BufferFillerIntegTest, fillCacheBufferBackward);
    FRIEND_TEST(BufferFillerIntegTest, fillCacheBufferBackward_2);
    FRIEND_TEST(BufferFillerIntegTest, fillCacheBufferBackward_3);
    FRIEND_TEST(BufferFillerIntegTest, rewindLineWhenNewChanges);
    FRIEND_TEST(BufferFillerIntegTest, rewindLineWhenNewChanges_2);
    FRIEND_TEST(BufferFillerIntegTest, changeStorageBecameSmaller);
#endif
  public:
    BufferFiller(std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<utils::storage::AbstractStorageFactory>);
    ScreenBuffer const &getNewWindowBuffer(feather::windows::MainWindowInterface *, FilledChunk, std::shared_ptr<printer::PrintingOrchestratorInterface> = nullptr) override;
    ScreenBuffer const &getScreenBuffer() const override;
    utils::windows::FramePositions const &getFramePositions() const override;
    std::shared_ptr<utils::storage::AbstractStorage> const &getStorage() const override;
    ~BufferFiller();

  private:
    bool isFrameAtLastPosition(std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &);
    void forwardLines(size_t, feather::windows::MainWindowInterface *, std::shared_ptr<printer::PrintingOrchestratorInterface>);
    void fillFrameBuffer(ScreenBuffer &, feather::windows::MainWindowInterface *, std::shared_ptr<printer::PrintingOrchestratorInterface>);
    void rewindLines(size_t, size_t, feather::windows::MainWindowInterface *, std::shared_ptr<printer::PrintingOrchestratorInterface>);
    void replaceMappedFile(std::shared_ptr<utils::storage::AbstractStorage> &, size_t) override;
    void setStartFramePosition(pair) override;
    void setEndFramePosition(pair) override;
    std::unique_ptr<utils::algorithm::SearchBuffer> fillCacheBuffer(utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, size_t, pair);
    std::unique_ptr<utils::algorithm::SearchBuffer> fillCacheBufferBackward(utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, size_t, pair);
    utils::windows::FramePositions framePosition;
    std::shared_ptr<utils::storage::AbstractStorage> storage;
    std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
    std::unique_ptr<ScreenBuffer> screenBuffer;
  };
} // namespace feather::utils
