#include "helpers/TestBase.hpp"
#include "mocks/FilesystemMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/PrintingStorageMock.hpp"
#include "mocks/StorageMock.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/Filesystem.hpp"
#include "utils/Serializer.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/exception/FeatherFileConcurrentlyModifiedException.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::printer
{
  class PrintingOrchestratorTest : public ::testing::Test, public ::feather::test::TestBase
  {
  protected:
    std::unique_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
    std::shared_ptr<PrintingStorageInterface> filePrintingStorageMock, memoryPrintingStorageMock;
    NCursesWrapperMock *nCursesWrapperMock;
    std::shared_ptr<PrintingOrchestrator> orchestrator;

  public:
    void SetUp() override
    {
      nCursesWrapperMock = new NCursesWrapperMock();
      nCursesMainWindowDecoratorMock = std::make_unique<NCursesMainWindowDecoratorMock>();
      filePrintingStorageMock = std::make_shared<PrintingStorageMock>();
      memoryPrintingStorageMock = std::make_shared<PrintingStorageMock>();
      EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
          .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
      configuration->forceTextMode(testFile);
    }

    void TearDown() override
    {
      inMemoryStorageFactory->removeAllStorages();
    }
  };

  TEST_F(PrintingOrchestratorTest, loadExistingChanges)
  {
    std::unique_ptr<utils::FilesystemInterface> filesystem = std::make_unique<testing::mocks::FilesystemMock>();

    // expect
    EXPECT_CALL(*dynamic_cast<testing::mocks::FilesystemMock *>(filesystem.get()), isFileDirectory("/tmp/aaabbbcccddd"))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), addToNewChanges(TEST_UUID_2, pair(0, 0), FIVE_BYTE_UTF8_CHARACTER, 5UL));

    // before
    const char *commandLineArguments[] = {"./feather", "file", "--sessionId=aaabbbcccddd"};
    utils::ProgramOptionsParser::parseArguments(3UL, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("storage", FIVE_BYTE_UTF8_CHARACTER);
    std::shared_ptr<feather::utils::storage::AbstractStorage> changesStorage = createEmptyStorage("/tmp/aaabbbcccddd/1.fea");
    Serializer::serialize(0, FIVE_BYTE_UTF8_CHARACTER, changesStorage);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock, storage);

    // when
    orchestrator->loadExistingChanges(std::move(filesystem), storage);

    // then
    ASSERT_THAT(orchestrator->getChangesPositions(TEST_UUID_2), ::testing::ElementsAre(std::make_pair(pair(0, 0), pair(0, 5UL))));
    ASSERT_EQ(orchestrator->getTotalBytesOfNewChanges(TEST_UUID_2), 5UL);
    ASSERT_EQ(orchestrator->getTotalBytesOfDeletions(TEST_UUID_2), 0);
    ASSERT_EQ(orchestrator->getNumberOfChangesBytesAtPos(0UL, TEST_UUID_2), 5UL);
  }

  TEST_F(PrintingOrchestratorTest, loadExistingChangesFileConcurrentlyModified)
  {
    std::unique_ptr<utils::FilesystemInterface> filesystem = std::make_unique<testing::mocks::FilesystemMock>();

    // expect
    EXPECT_CALL(*dynamic_cast<testing::mocks::FilesystemMock *>(filesystem.get()), isFileDirectory("/tmp/aaabbbcccddd"))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), addToNewChanges(TEST_UUID_2, pair(0, 0), FIVE_BYTE_UTF8_CHARACTER, 5UL));

    // before
    const char *commandLineArguments[] = {"./feather", "file", "--sessionId=aaabbbcccddd"};
    utils::ProgramOptionsParser::parseArguments(3UL, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<feather::utils::storage::AbstractStorage> changesStorage = createEmptyStorage("/tmp/aaabbbcccddd/1.fea");
    std::shared_ptr<feather::utils::storage::AbstractStorage> invalidStorage = createEmptyStorage("/tmp/aaabbbcccddd/invalid.fea");
    Serializer::serialize(0, FIVE_BYTE_UTF8_CHARACTER, changesStorage);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("storage", FIVE_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    // then
    ASSERT_THROW(orchestrator->loadExistingChanges(std::move(filesystem), storage), feather::utils::exception::FeatherFileConcurrentlyModifiedException);

    std::unique_ptr<utils::FilesystemInterface> filesystem_2 = std::make_unique<testing::mocks::FilesystemMock>();

    // expect
    EXPECT_CALL(*dynamic_cast<testing::mocks::FilesystemMock *>(filesystem_2.get()), isFileDirectory("/tmp/aaabbbcccddd"))
        .WillOnce(::testing::Return(true));
 
    // when
    const char *commandLineArguments_2[] = {"./feather", "file", "--sessionId=aaabbbcccddd", "--force"};
    utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments_2);
    configuration->overrideConfiguration();
    orchestrator->loadExistingChanges(std::move(filesystem_2), storage);
 
    // then
    ASSERT_THAT(orchestrator->getChangesPositions(TEST_UUID_2), ::testing::ElementsAre(std::make_pair(pair(0, 0), pair(0, 5UL))));
    ASSERT_EQ(orchestrator->getTotalBytesOfNewChanges(TEST_UUID_2), 5UL);
    ASSERT_EQ(orchestrator->getTotalBytesOfDeletions(TEST_UUID_2), 0);
    ASSERT_EQ(orchestrator->getNumberOfChangesBytesAtPos(0UL, TEST_UUID_2), 5UL);
  }

  TEST_F(PrintingOrchestratorTest, loadExistingChangesFileConcurrentlyModifiedDontLoadChangesGreaterThanFileSize)
  {
    std::unique_ptr<utils::FilesystemInterface> filesystem = std::make_unique<testing::mocks::FilesystemMock>();

    // expect
    EXPECT_CALL(*dynamic_cast<testing::mocks::FilesystemMock *>(filesystem.get()), isFileDirectory("/tmp/aaabbbcccddd"))
        .WillOnce(::testing::Return(true));

    // before
    const char *commandLineArguments[] = {"./feather", "file", "--sessionId=aaabbbcccddd", "--force"};
    utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<feather::utils::storage::AbstractStorage> changesStorage = createEmptyStorage("/tmp/aaabbbcccddd/1.fea");
    std::shared_ptr<feather::utils::storage::AbstractStorage> invalidStorage = createEmptyStorage("/tmp/aaabbbcccddd/invalid.fea");
    Serializer::serialize(6, FIVE_BYTE_UTF8_CHARACTER, changesStorage);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("storage", FIVE_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    orchestrator->loadExistingChanges(std::move(filesystem), storage);

    // then
    ASSERT_THAT(orchestrator->getChangesPositions(TEST_UUID_2), ::testing::IsEmpty());
    ASSERT_EQ(orchestrator->getTotalBytesOfNewChanges(TEST_UUID_2), 0);
  }

  TEST_F(PrintingOrchestratorTest, isCharAtPosDeleted)
  {
    // before
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    // then
    ASSERT_FALSE(orchestrator->isCharAtPosDeleted(pair{0, 0}, TEST_UUID));
    ASSERT_FALSE(orchestrator->isCharAtPosDeleted(pair{1, 0}, TEST_UUID));

    // when
    orchestrator->deletions[TEST_UUID][pair(2UL, 0UL)] = pair(5, 0);
    orchestrator->deletions[TEST_UUID][pair(5UL, 1UL)] = pair(5, 8);
    orchestrator->totalBytesOfDeletions[TEST_UUID] = 10UL;

    // then
    ASSERT_FALSE(orchestrator->isCharAtPosDeleted(pair(1, 5), TEST_UUID));
    ASSERT_FALSE(orchestrator->isCharAtPosDeleted(pair(5, 0), TEST_UUID));
    ASSERT_FALSE(orchestrator->isCharAtPosDeleted(pair(5, 8), TEST_UUID));
    ASSERT_FALSE(orchestrator->isCharAtPosDeleted(pair(6, 0), TEST_UUID));
    ASSERT_TRUE(orchestrator->isCharAtPosDeleted(pair(2, 0), TEST_UUID));
    ASSERT_TRUE(orchestrator->isCharAtPosDeleted(pair(5, 1), TEST_UUID));
  }

  TEST_F(PrintingOrchestratorTest, getLastValidPosition)
  {
    // before
    auto data = FOUR_BYTE_UTF8_CHARACTER;
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("4ByteUtf8.fea", data);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(0, 0));

    // when
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), addToNewChanges(TEST_UUID, pair(4, 0), std::vector<char>{'a', 'b'}, 2));
    orchestrator->addChanges(TEST_UUID, storage, pair(4, 0), {'a', 'b'}, 2UL);
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 4UL, 0UL, 1UL))
        .WillRepeatedly(::testing::Return(std::vector<char>{'a', 'b'}));
    orchestrator->totalBytesOfDeletions[TEST_UUID] = 1UL;
    orchestrator->deletions[TEST_UUID][pair(4, 1)] = pair(4UL, 2UL);
    orchestrator->reverseDeletions[TEST_UUID][pair(4, 2)] = pair(4UL, 1UL);
    orchestrator->changes[TEST_UUID][4UL] = 2UL;

    // then
    ASSERT_EQ(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(4, 0));
  }

  TEST_F(PrintingOrchestratorTest, getLastValidPosition_2)
  {
    // before
    auto data = FOUR_BYTE_UTF8_CHARACTER;
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("4ByteUtf8.fea", data);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 4UL, 0UL, 1UL))
        .WillRepeatedly(::testing::Return(std::vector<char>{'a', 'b'}));
    orchestrator->totalBytesOfDeletions[TEST_UUID] = 6UL;
    orchestrator->deletions[TEST_UUID][pair(0, 0)] = pair(4UL, 2UL);
    orchestrator->reverseDeletions[TEST_UUID][pair(4, 2)] = pair(0UL, 0UL);
    orchestrator->changes[TEST_UUID][4UL] = 2UL;

    // then
    ASSERT_EQ(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(4, 2));
  }

  TEST_F(PrintingOrchestratorTest, getLastValidPositionZeroSize)
  {
    // before
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("newFile");
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(0, 0));
  }

  TEST_F(PrintingOrchestratorTest, getCharacterBytesTaken)
  {
    // before
    auto newChanges = FOUR_BYTE_UTF8_CHARACTER;
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);
    auto storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 2, 0, MAX_UTF8_CHAR_BYTES))
        .WillOnce(::testing::Return(newChanges));
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 2, 4, 1))
        .WillOnce(::testing::Return(std::vector<char>{'t'}));

    // when
    // then
    ASSERT_EQ(orchestrator->getCharacterBytesTaken(pair(0, 0), storage, TEST_UUID), 1UL);
    ASSERT_EQ(orchestrator->getCharacterBytesTaken(pair(2, 0), storage, TEST_UUID), 4UL);
    ASSERT_EQ(orchestrator->getCharacterBytesTaken(pair(6, 0), storage, TEST_UUID), 4UL);

    // when
    orchestrator->changes[TEST_UUID][2] = 5;

    // then
    ASSERT_EQ(orchestrator->getCharacterBytesTaken(pair(2, 0), storage, TEST_UUID), 4UL);
    ASSERT_EQ(orchestrator->getCharacterBytesTaken(pair(2, 4), storage, TEST_UUID), 1UL);
  }

  TEST_F(PrintingOrchestratorTest, getNextVirtualPosition)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(1, 0)), pair(2, 0));
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(2, 0)), pair(6, 0));

    // when
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 2, 0, MAX_UTF8_CHAR_BYTES))
        .WillOnce(::testing::Return(std::vector<char>{FOUR_BYTE_UTF8_CHARACTER}));
    orchestrator->changes[TEST_UUID][2] = 4;

    // then
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(2, 0)), pair(2, 4));
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(2, 4)), pair(6, 0));

    // when
    orchestrator->changes[TEST_UUID][10] = 4;
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 10, 0, MAX_UTF8_CHAR_BYTES))
        .WillOnce(::testing::Return(std::vector<char>{FOUR_BYTE_UTF8_CHARACTER}));

    // then
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(10, 0)), pair(10, 4));
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(10, 4)), pair(11, 0));
  }

  TEST_F(PrintingOrchestratorTest, getNextVirtualPositionWithDeletions)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    orchestrator->deletions[TEST_UUID][pair(1, 0)] = pair(2, 0);

    // then
    orchestrator->deletions[TEST_UUID][pair(6, 0)] = pair(10, 0);
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(2, 0)), pair(10, 0));
  }

  TEST_F(PrintingOrchestratorTest, getPreviousVirtualPosition)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(10, 0)), pair(6, 0));
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(0, 0)), pair(0, 0));

    // when
    orchestrator->changes[TEST_UUID][2UL] = 2UL;
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 2UL, 0, 2UL))
        .WillOnce(::testing::Return(std::vector<char>{TWO_BYTE_UTF8_CHARACTER}));

    // then
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(6UL, 0UL)), pair(2UL, 2UL));
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(2UL, 2UL)), pair(2UL, 0));
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(2UL, 0UL)), pair(1UL, 0));

    // when
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 2UL, 4UL, 4UL))
        .WillOnce(::testing::Return(std::vector<char>{FOUR_BYTE_UTF8_CHARACTER}));
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 2UL, 0UL, 4UL))
        .WillOnce(::testing::Return(std::vector<char>{FOUR_BYTE_UTF8_CHARACTER}));
    orchestrator->changes[TEST_UUID][2UL] = 8UL;

    // then
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(2UL, 8UL)), pair(2UL, 4UL));
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(2UL, 4UL)), pair(2UL, 0UL));
  }

  TEST_F(PrintingOrchestratorTest, getPreviousVirtualPositionWithDeletions)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    orchestrator->deletions[TEST_UUID][pair(6, 0)] = pair(10, 0);
    orchestrator->reverseDeletions[TEST_UUID][pair(10, 0)] = pair(6, 0);

    // then
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(10, 0)), pair(2, 0));
  }

  TEST_F(PrintingOrchestratorTest, getPreviousVirtualPosition_2)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", FIVE_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), getNewChanges(TEST_UUID, 2, 0UL, 3UL))
        .WillOnce(::testing::Return(THREE_BYTE_UTF8_CHARACTER));
    orchestrator->changes[TEST_UUID][2] = 5UL;

    // then
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(2, 3)), pair(2UL, 1UL));
  }

  TEST_F(PrintingOrchestratorTest, removeCharacters)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", FIVE_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0), pair(1, 0));
    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{{{0, 0}, {1, 0}}}));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(1, 0), pair(2, 0));
    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{{{0, 0}, {2, 0}}}));
    ASSERT_EQ(orchestrator->reverseDeletions[TEST_UUID], (std::map<pair, pair>{{{2, 0}, {0, 0}}}));
    ASSERT_EQ(orchestrator->totalBytesOfDeletions[TEST_UUID], 2UL);

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(4, 0), pair(5, 0));
    orchestrator->removeCharacters(TEST_UUID, storage, pair(7, 0), pair(10, 0));
    orchestrator->removeCharacters(TEST_UUID, storage, pair(3, 0), pair(11, 0));

    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{{{0, 0}, {2, 0}}, {{3, 0}, {11, 0}}}));
    ASSERT_EQ(orchestrator->reverseDeletions[TEST_UUID], (std::map<pair, pair>{{{11, 0}, {3, 0}}, {{2, 0}, {0, 0}}}));
    ASSERT_EQ(orchestrator->totalBytesOfDeletions[TEST_UUID], 10);
  }

  TEST_F(PrintingOrchestratorTest, removeCharacters_2)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", FIVE_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);
    orchestrator->removeCharacters(TEST_UUID, storage, pair(10304349259, 0), pair(10304350054, 0));
    orchestrator->removeCharacters(TEST_UUID, storage, pair(10304350055, 0), pair(10304350152, 0));
    orchestrator->removeCharacters(TEST_UUID, storage, pair(10304350193, 0), pair(10304350257, 0));
    orchestrator->removeCharacters(TEST_UUID, storage, pair(10304350428, 0), pair(10304356352, 0));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(10304350018, 0), pair(10304350026, 0));

    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{{pair(10304349259, 0), pair(10304350054, 0)}, {pair(10304350055, 0), pair(10304350152, 0)}, {pair(10304350193, 0), pair(10304350257, 0)}, {pair(10304350428, 0), pair(10304356352, 0)}}));
  }

  TEST_F(PrintingOrchestratorTest, undelete)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", FIVE_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);
    orchestrator->removeCharacters(TEST_UUID, storage, pair(10, 0), pair(10, 5));

    // when
    orchestrator->undelete(pair(10, 0), pair(10, 1), TEST_UUID);

    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{{{10, 1}, {10, 5}}}));

    // when
    orchestrator->undelete(pair(10, 1), pair(10, 2), TEST_UUID);

    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{{{10, 2}, {10, 5}}}));

    // when
    orchestrator->undelete(pair(10, 3), pair(10, 4), TEST_UUID);

    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{{{10, 2}, {10, 3}}, {{10, 4}, {10, 5}}}));

    // when
    orchestrator->undelete(pair(10, 2), pair(10, 3), TEST_UUID);

    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{{{10, 4}, {10, 5}}}));

    // when
    orchestrator->undelete(pair(10, 4), pair(10, 5), TEST_UUID);

    // then
    ASSERT_EQ(orchestrator->deletions[TEST_UUID], (std::map<pair, pair>{}));
  }

  TEST_F(PrintingOrchestratorTest, removeChangesRelatedToWindow)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), removeChanges(TEST_UUID, 1UL));
    EXPECT_CALL(*dynamic_cast<PrintingStorageMock *>(filePrintingStorageMock.get()), removeChanges(TEST_UUID, 10UL));
    orchestrator->changes[TEST_UUID][10] = 1UL;
    orchestrator->changes[TEST_UUID][1] = 1UL;
    orchestrator->deletions[TEST_UUID][pair(10, 0)] = pair(11UL, 1UL);
    orchestrator->totalBytesOfNewChanges[TEST_UUID] = 1UL;
    orchestrator->totalBytesOfDeletions[TEST_UUID] = 3UL;
    orchestrator->removeChangesRelatedToWindow(TEST_UUID);

    // then
    ASSERT_EQ(orchestrator->getTotalBytesOfNewChanges(TEST_UUID), 0UL);
    ASSERT_EQ(orchestrator->getTotalBytesOfDeletions(TEST_UUID), 0UL);
    ASSERT_EQ(orchestrator->getChangesPositions(TEST_UUID), (std::map<pair, pair>{}));
  }

  TEST_F(PrintingOrchestratorTest, convertVirtualPositionToByteOffset)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    orchestrator->changes[TEST_UUID][3] = 3UL;
    orchestrator->changes[TEST_UUID][2] = 5UL;
    orchestrator->changes[TEST_UUID][10] = 3UL;
    orchestrator->deletions[TEST_UUID][pair(2, 0)] = pair(2UL, 1UL);
    orchestrator->deletions[TEST_UUID][pair(3, 3)] = pair(3UL, 6UL);

    // then
    ASSERT_EQ(orchestrator->convertVirtualPositionToByteOffset(pair(10, 1), TEST_UUID), 15UL);
    ASSERT_EQ(orchestrator->convertVirtualPositionToByteOffset(pair(10, 3), TEST_UUID), 17UL);
  }

  TEST_F(PrintingOrchestratorTest, getDiffBytes)
  {
    // before
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    orchestrator->changes[TEST_UUID][2] = 5UL;
    orchestrator->deletions[TEST_UUID][pair(2, 0)] = pair(2UL, 4UL);
    orchestrator->changes[TEST_UUID][5] = 3UL;
    orchestrator->changes[TEST_UUID][10] = 3UL;
    orchestrator->deletions[TEST_UUID][pair(10, 0)] = pair(10UL, 4UL);

    // then
    ASSERT_EQ(orchestrator->getDiffBytes(pair(2, 5), pair(9, 0), TEST_UUID), 10UL);
  }

  TEST_F(PrintingOrchestratorTest, getDiffToNextChange)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getDiffToNextChange(pair(6, 0), storage, TEST_UUID), 0UL);
    ASSERT_EQ(orchestrator->getDiffToNextChange(pair(1, 0), storage, TEST_UUID), 9UL);

    // when
    orchestrator->changes[TEST_UUID][5] = 3UL;
    ASSERT_EQ(orchestrator->getDiffToNextChange(pair(1, 0), storage, TEST_UUID), 4UL);

    // when
    orchestrator->changes[TEST_UUID][1] = 3UL;
    // then
    ASSERT_EQ(orchestrator->getDiffToNextChange(pair(1UL, 2UL), storage, TEST_UUID), 5UL);
  }

  TEST_F(PrintingOrchestratorTest, getDiffToNextDeletion)
  {
    // before
    auto const storage = createStorage("10ByteUtf8", TEN_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getDiffToNextDeletion(pair(6, 0), storage, TEST_UUID), 4UL);
    ASSERT_EQ(orchestrator->getDiffToNextDeletion(pair(1, 0), storage, TEST_UUID), 9UL);

    // when
    orchestrator->deletions[TEST_UUID][pair(5, 0)] = pair(5, 3UL);
    ASSERT_EQ(orchestrator->getDiffToNextDeletion(pair(1, 0), storage, TEST_UUID), 4UL);

    // when
    orchestrator->deletions[TEST_UUID][pair(1, 2)] = pair(1UL, 3UL);
    // then
    ASSERT_EQ(orchestrator->getDiffToNextDeletion(pair(0UL, 0UL), storage, TEST_UUID), 3UL);
  }

  TEST_F(PrintingOrchestratorTest, getNumberOfChangesBytesBetweenPos)
  {
    // before
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorageMock, nullptr, changesStack, filesystemMock);

    // when
    orchestrator->changes[TEST_UUID][2] = 5UL;
    orchestrator->deletions[TEST_UUID][pair(2, 0)] = pair(2UL, 4UL);
    orchestrator->changes[TEST_UUID][5] = 3UL;
    orchestrator->changes[TEST_UUID][10] = 3UL;
    orchestrator->deletions[TEST_UUID][pair(10, 0)] = pair(10UL, 4UL);

    // then
    ASSERT_EQ(orchestrator->getNumberOfChangesBytesBetweenPos(2UL, 10UL, TEST_UUID), 3UL);
  }
} // namespace feather::printer
