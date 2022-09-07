#include "helpers/TestBase.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/PrintingStorageMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/MemoryPrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/exception/FeatherCacheFullException.hpp"
#include "utils/exception/FeatherCacheMissException.hpp"
#include "utils/exception/FeatherMemoryException.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::printer
{
  class PrintingOrchestratorIntegTest : public ::testing::Test, public ::feather::test::TestBase
  {
  protected:
    std::unique_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
    std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
    NCursesWrapperMock *nCursesWrapperMock;
    std::shared_ptr<PrintingOrchestrator> orchestrator;

  public:
    void SetUp() override
    {
      nCursesWrapperMock = new NCursesWrapperMock();
      nCursesMainWindowDecoratorMock = std::make_unique<NCursesMainWindowDecoratorMock>();
      filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
      EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
          .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
      configuration->forceTextMode(testFile);
    }

    void TearDown() override
    {
    }
  };

  TEST_F(PrintingOrchestratorIntegTest, getPreviousCharacter)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
    utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
    configuration->overrideConfiguration();

    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getPreviousCharacter.fea", TWENTY_BYTE_UTF8_CHARACTER);
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(20UL, 0)), pair(16UL, 0));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0), pair(2UL, 0));

    // then
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(3UL, 0)), pair(2UL, 0));
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(1UL, 0)), pair(2UL, 0));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(6UL, 0), pair(8UL, 0));
    orchestrator->addChanges(TEST_UUID, storage, pair(8UL, 0), std::vector<char>{(char)0xF0, (char)0x9F}, 2UL);

    // then
    ASSERT_EQ(orchestrator->getPreviousVirtualPosition(TEST_UUID, storage, pair(10UL, 0)), pair(8UL, 0));
  }

  TEST_F(PrintingOrchestratorIntegTest, getNextCharacter)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
    utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
    configuration->overrideConfiguration();

    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextCharacter.fea", TWENTY_BYTE_UTF8_CHARACTER);
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(16UL, 0)), pair(20UL, 0));

    // when
    // then
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(1UL, 0)), pair(2UL, 0));
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(2UL, 0)), pair(6UL, 0));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(6UL, 0), pair(10UL, 0));

    // then
    ASSERT_EQ(orchestrator->getNextVirtualPosition(TEST_UUID, storage, pair(2UL, 0)), pair(10UL, 0));
  }

  TEST_F(PrintingOrchestratorIntegTest, addChanges)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("addChanges");
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);

    // when
    // then
    ASSERT_THROW(memoryPrintingStorage->getNewChanges(TEST_UUID, 1UL, 0UL, 2UL), utils::exception::FeatherCacheMissException);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(1UL, 0UL), TWO_BYTE_UTF8_CHARACTER, 2UL);

    // then
    ASSERT_EQ(orchestrator->getNewChanges(TEST_UUID, 1UL, 0UL, 1UL), std::vector<char>{(char)0xC5});
    ASSERT_EQ(orchestrator->getNewChanges(TEST_UUID, 1UL, 0UL, 2UL), TWO_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(orchestrator->getNumberOfChangesBytesAtPos(1UL, TEST_UUID), 2UL);
    ASSERT_THROW(memoryPrintingStorage->getNewChanges(TEST_UUID, 1UL, 0UL, 3UL), utils::exception::FeatherCacheMissException);
    ASSERT_THROW(memoryPrintingStorage->getNewChanges(TEST_UUID, 2UL, 0UL, 1UL), utils::exception::FeatherCacheMissException);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(1UL, 0UL), FOUR_BYTE_UTF8_CHARACTER, 4UL);

    // then
    ASSERT_EQ(orchestrator->getNewChanges(TEST_UUID, 1UL, 0UL, 4UL), FOUR_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(orchestrator->getNumberOfChangesBytesAtPos(1UL, TEST_UUID), 6UL);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(1UL, 0UL), FOUR_BYTE_UTF8_CHARACTER, 4UL);

    // then BP
    // ASSERT_THROW(memoryPrintingStorage->addToNewChanges(TEST_UUID, pair(1UL, 0UL), FOUR_BYTE_UTF8_CHARACTER, 4UL), utils::exception::FeatherMemoryException);
    ASSERT_NO_THROW(memoryPrintingStorage->getNewChanges(TEST_UUID, 1UL, 0UL, 4UL));
    ASSERT_EQ(orchestrator->getNewChanges(TEST_UUID, 1UL, 0UL, 4UL), FOUR_BYTE_UTF8_CHARACTER);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(2UL, 0UL), TEN_BYTE_UTF8_CHARACTER, 10UL);

    // then
    ASSERT_NO_THROW(memoryPrintingStorage->addToNewChanges(TEST_UUID, pair(2UL, 0), TEN_BYTE_UTF8_CHARACTER, 10UL));
    ASSERT_EQ(orchestrator->totalBytesOfNewChanges[TEST_UUID], 20UL);
    ASSERT_EQ(orchestrator->changes[TEST_UUID][1], 10UL);
    ASSERT_EQ(orchestrator->changes[TEST_UUID][2], 10UL);
    ASSERT_EQ(orchestrator->getNewChanges(TEST_UUID, 2UL, 0UL, 10UL), TEN_BYTE_UTF8_CHARACTER);
  }

  TEST_F(PrintingOrchestratorIntegTest, addChangesShiftDeletionsProperly)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("addChangesShiftDeletionsProperly", FIVE_BYTE_UTF8_CHARACTER);
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(0, 0), FOUR_BYTE_UTF8_CHARACTER, 4UL);
    orchestrator->addChanges(TEST_UUID, storage, pair(0, 0), FOUR_BYTE_UTF8_CHARACTER, 4UL);
    orchestrator->removeCharacters(TEST_UUID, storage, pair(0, 4UL), pair(0, 8UL));
    orchestrator->removeCharacters(TEST_UUID, storage, pair(0, 8UL), pair(1UL, 0));
    orchestrator->addChanges(TEST_UUID, storage, pair(0, 0), FOUR_BYTE_UTF8_CHARACTER, 4UL);
    orchestrator->addChanges(TEST_UUID, storage, pair(0, 0), FOUR_BYTE_UTF8_CHARACTER, 4UL);

    // then
    ASSERT_THAT(orchestrator->deletions[TEST_UUID], ::testing::ElementsAre(std::make_pair(pair(0, 12), pair(1, 0))));
  }

  TEST_F(PrintingOrchestratorIntegTest, addChangesCacheFull)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("addChangesCacheFull");
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(1UL, 0UL), TWO_BYTE_UTF8_CHARACTER, 2UL);

    // then
    ASSERT_EQ(orchestrator->getNewChanges(TEST_UUID, 1UL, 0UL, 2UL), TWO_BYTE_UTF8_CHARACTER);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(2UL, 0UL), TWO_BYTE_UTF8_CHARACTER, 2UL);

    // then
    // ASSERT_THROW(memoryPrintingStorage->addToNewChanges(TEST_UUID, pair(2UL, 0), 0UL, TWO_BYTE_UTF8_CHARACTER, 2UL), utils::exception::FeatherCacheFullException);
    ASSERT_EQ(orchestrator->totalBytesOfNewChanges[TEST_UUID], 4UL);
    ASSERT_EQ(orchestrator->changes[TEST_UUID][2], 2UL);
    ASSERT_EQ(orchestrator->getNewChanges(TEST_UUID, 1UL, 0UL, 2UL), TWO_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(orchestrator->getNewChanges(TEST_UUID, 2UL, 0UL, 2UL), TWO_BYTE_UTF8_CHARACTER);
  }

  TEST_F(PrintingOrchestratorIntegTest, getContinousCharacters)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);
    std::vector<char> storageData{(char)0x18, 'F', 'E', 'A', 'T', 'H', 'E', 'R', '_', 'F', 'I', 'L', 'E', 0x00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 02, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 00, 00, 00, 00, 00, 00, 00, (char)0xDC, 00, 00, 00, 00, 00, 00, 00, 01, 00, 'P', 0x0C, (char)0xB4, 0x7F, 00, 00};
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("Test1.fea", storageData);

    // when
    // then
    ASSERT_EQ(orchestrator->getContinousCharacters(pair(0, 0), 5, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0x18, 'F', 'E', 'A', 'T'}, pair(5, 0)));
  }

  TEST_F(PrintingOrchestratorIntegTest, getContinousCharacters_2)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);
    std::vector<char> storageData{(char)0x18, 'F', 'E', 'A', 'T', 'H', 'E', 'R', '_', 'F', 'I', 'L', 'E', 0x00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 02, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 00, 00, 00, 00, 00, 00, 00, (char)0xDC, 00, 00, 00, 00, 00, 00, 00, 01, 00, 'P', 0x0C, (char)0xB4, 0x7F, 00, 00};
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getContinousCharacters_2", storageData);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(1, 0), TWO_BYTE_UTF8_CHARACTER, 2UL);

    // then
    ASSERT_EQ(orchestrator->getContinousCharacters(pair(0, 0), 5, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0x18, (char)0xC5, (char)0x9B, 'F', 'E'}, pair(3, 0)));
  }

  TEST_F(PrintingOrchestratorIntegTest, getContinousCharacters_3)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);
    std::vector<char> storageData{(char)0x18, 'F', 'E', 'A', 'T', 'H', 'E', 'R', '_', 'F', 'I', 'L', 'E', 0x00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 02, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 00, 00, 00, 00, 00, 00, 00, (char)0xDC, 00, 00, 00, 00, 00, 00, 00, 01, 00, 'P', 0x0C, (char)0xB4, 0x7F, 00, 00};
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("Test1.fea", storageData);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(1, 0), TWO_BYTE_UTF8_CHARACTER, 2UL);
    orchestrator->removeCharacters(TEST_UUID, storage, pair(1, 0), pair(3, 0));

    // then
    ASSERT_EQ(orchestrator->getContinousCharacters(pair(0, 0), 5, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0x18, 'A', 'T', 'H', 'E'}, pair(7, 0)));
  }

  TEST_F(PrintingOrchestratorIntegTest, getContinousCharacters_4)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getContinousCharacters_4.fea", TEN_BYTE_UTF8_CHARACTER);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(10, 0), FIVE_BYTE_UTF8_CHARACTER, 5UL);
    orchestrator->removeCharacters(TEST_UUID, storage, pair(10, 0), pair(10, 1));

    // then
    ASSERT_EQ(orchestrator->getContinousCharacters(pair(0, 0), 5, storage, TEST_UUID), std::make_pair(std::vector<char>{'t', 'e', (char)0xF0, (char)0x9F, (char)0x98}, pair(5, 0)));
    ASSERT_EQ(orchestrator->getContinousCharacters(pair(6, 0), 7, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, (char)0xC5, (char)0x9B, (char)0xC5}, pair(10, 4)));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(6, 0), pair(10, 5));

    // then
    ASSERT_EQ(orchestrator->getContinousCharacters(pair(2, 0), 8, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}, pair(6UL, 0)));
  }

  TEST_F(PrintingOrchestratorIntegTest, getContinousCharacters_6)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getContinousCharacters_6.fea", TEN_BYTE_UTF8_CHARACTER);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(10, 0), std::vector<char>{(char)0xC5}, 1UL);

    // then
    ASSERT_EQ(orchestrator->getContinousCharacters(pair(6, 0), 5, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, (char)0xC5}, pair(10UL, 1UL)));
  }

  TEST_F(PrintingOrchestratorIntegTest, getContinousCharacters_5)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=32"};
    utils::ProgramOptionsParser::parseArguments(3, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("getContinousCharacters_5");

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(0, 0), {'a', 'b', 'c', 'd', 'e', 'f', '\n', '\n'}, 8UL);
    orchestrator->removeCharacters(TEST_UUID, storage, pair(0, 4), pair(0, 6));

    // then
    ASSERT_EQ(orchestrator->getContinousCharacters(pair(0, 0), 5, storage, TEST_UUID), std::make_pair(std::vector<char>{'a', 'b', 'c', 'd', '\n'}, pair(0, 7)));
  }

  TEST_F(PrintingOrchestratorIntegTest, getContinousCharactersBackward_1)
  {
    // before
    configuration->forceTextMode(testFile);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getContinousCharactersBackward_1.fea", TWENTY_BYTE_UTF8_CHARACTER);
    const char *commandLineArguments[] = {"./feather", "getContinousCharactersBackward_1.fea", "--cache=6", "--relax=4"};
    utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);

    // when
    // then
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(16, 0), 4UL, storage, TEST_UUID), std::make_pair(FOUR_BYTE_UTF8_CHARACTER, pair(12, 0)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(12, 0), 4UL, storage, TEST_UUID), std::make_pair(std::vector{(char)0x98, (char)0x9A, 't', 'e'}, pair(8UL, 0)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(2, 0), 4UL, storage, TEST_UUID), std::make_pair(std::vector{'t', 'e'}, pair(0, 0)));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(10, 0), pair(11, 0));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(12, 0), 2UL, storage, TEST_UUID), std::make_pair(std::vector{(char)0x9A, 'e'}, pair(9UL, 0)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(11, 0), 4UL, storage, TEST_UUID), std::make_pair(FOUR_BYTE_UTF8_CHARACTER, pair(6, 0)));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(6, 0), pair(11, 0));

    // thwn
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(11, 0), 4UL, storage, TEST_UUID), std::make_pair(FOUR_BYTE_UTF8_CHARACTER, pair(2, 0)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(11, 0), 5UL, storage, TEST_UUID), std::make_pair(std::vector<char>{'e', (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}, pair(1, 0)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(11, 0), 6UL, storage, TEST_UUID), std::make_pair(std::vector<char>{'t', 'e', (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}, pair(0, 0)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(11, 0), 7UL, storage, TEST_UUID), std::make_pair(std::vector<char>{'t', 'e', (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}, pair(0, 0)));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0), pair(1, 0));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(11, 0), 7UL, storage, TEST_UUID), std::make_pair(std::vector<char>{'e', (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}, pair(1, 0)));
  }

  TEST_F(PrintingOrchestratorIntegTest, getContinousCharactersBackward_2)
  {
    // before
    configuration->forceTextMode(testFile);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getContinousCharactersBackward_1.fea", TWENTY_BYTE_UTF8_CHARACTER);
    const char *commandLineArguments[] = {"./feather", "getContinousCharactersBackward_1.fea", "--cache=6", "--relax=4"};
    utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(20UL, 0), FIVE_BYTE_UTF8_CHARACTER, 5UL);

    // then
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(20UL, 3UL), 3UL, storage, TEST_UUID), std::make_pair(std::vector<char>{'t', (char)0xC5, (char)0x9B}, pair(20, 0)));

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(16UL, 0), FIVE_BYTE_UTF8_CHARACTER, 5UL);

    // then
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(20UL, 3UL), 9UL, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0xC5, (char)0x9B, (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, 't', (char)0xC5, (char)0x9B}, pair(16, 3)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(20UL, 3UL), 10UL, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0x9B, (char)0xC5, (char)0x9B, (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, 't', (char)0xC5, (char)0x9B}, pair(16, 2)));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(16, 3), pair(20, 0));

    // then
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(20UL, 3UL), 9UL, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0x9F, (char)0x98, (char)0x9A, 't', (char)0xC5, (char)0x9B, 't', (char)0xC5, (char)0x9B}, pair(13UL, 0)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(20UL, 3UL), 10UL, storage, TEST_UUID), std::make_pair(std::vector<char>{'\xF0', '\x9F', '\x98', '\x9A', 't', (char)0xC5, (char)0x9B, 't', (char)0xC5, (char)0x9B}, pair(12, 0)));
    ASSERT_EQ(orchestrator->getContinousCharactersBackward(pair(20UL, 3UL), 4UL, storage, TEST_UUID), std::make_pair(std::vector<char>{(char)0x9B, 't', (char)0xC5, (char)0x9B}, pair(16UL, 2UL)));
  }

  TEST_F(PrintingOrchestratorIntegTest, getLastValidPosition)
  {
    // before
    configuration->forceTextMode(testFile);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getContinousCharactersBackward_1.fea", TWENTY_BYTE_UTF8_CHARACTER);
    const char *commandLineArguments[] = {"./feather", "getContinousCharactersBackward_1.fea", "--cache=6", "--relax=4"};
    utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0), pair(20UL, 0));

    // then
    ASSERT_EQ(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(20UL, 0));
  }

  TEST_F(PrintingOrchestratorIntegTest, getDiffBytes)
  {
    // before
    const char *commandLineArguments[] = {"./feather", "file", "--cache=6", "--relax=4"};
    utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
    configuration->overrideConfiguration();
    std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
    orchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, memoryPrintingStorage, filePrintingStorage, changesStack, filesystemMock);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getDiffBytes.fea", TEN_BYTE_UTF8_CHARACTER);

    // then
    ASSERT_EQ(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(6UL, 0));
    ASSERT_EQ(orchestrator->getDiffBytes(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(storage->getSize(), orchestrator->getNumberOfChangesBytesAtPos(storage->getSize(), TEST_UUID)), TEST_UUID), 4UL);
    ASSERT_EQ(orchestrator->getFirstInvalidPosition(TEST_UUID, storage), pair(10UL, 0));

    // when
    orchestrator->removeCharacters(TEST_UUID, storage, pair(6UL, 0), pair(10UL, 0));

    // then
    ASSERT_EQ(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(2UL, 0));
    ASSERT_EQ(orchestrator->getDiffBytesWithoutDeletions(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(storage->getSize(), orchestrator->getNumberOfChangesBytesAtPos(storage->getSize(), TEST_UUID)), TEST_UUID), 4UL);
    ASSERT_EQ(orchestrator->getFirstInvalidPosition(TEST_UUID, storage), pair(6UL, 0));

    // when
    orchestrator->addChanges(TEST_UUID, storage, pair(2UL, 0), FIVE_BYTE_UTF8_CHARACTER, 5UL);

    // then
    ASSERT_EQ(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(2UL, 5UL));
    ASSERT_EQ(orchestrator->getDiffBytesWithoutDeletions(orchestrator->getLastValidPosition(TEST_UUID, storage), pair(storage->getSize(), orchestrator->getNumberOfChangesBytesAtPos(storage->getSize(), TEST_UUID)), TEST_UUID), 4UL);
    ASSERT_EQ(orchestrator->getFirstInvalidPosition(TEST_UUID, storage), pair(2UL, 9UL));
  }
} // namespace feather::printer