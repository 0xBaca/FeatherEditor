#include "helpers/TestBase.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/FilesystemMock.hpp"
#include "mocks/PrintingVisitorMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/FileStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::utils::algorithm
{
    class SearchBufferIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::unique_ptr<utils::BufferFillerInterface> bufferFillerMock;
        std::unique_ptr<utils::FilesystemInterface> filesystemMock;
        std::unique_ptr<printer::PrintingVisitorInterface> printingVisitorMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
        std::shared_ptr<WindowsManager> windowsManagerMock;

    public:
        void SetUp() override
        {
            bufferFillerMock = std::unique_ptr<BufferFillerMock>(new BufferFillerMock());
            filesystemMock = std::make_unique<FilesystemMock>();
            printingVisitorMock = std::unique_ptr<PrintingVisitorMock>(new PrintingVisitorMock());
            filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        }
    };

    TEST_F(SearchBufferIntegTest, getNextCharacter)
    {
        //before
        utils::windows::FramePositions framePositions = utils::windows::FramePositions();
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        auto twentyFourBytes = TWENTY_BYTE_UTF8_CHARACTER;
        std::copy(FOUR_BYTE_UTF8_CHARACTER.cbegin(), FOUR_BYTE_UTF8_CHARACTER.cend(), std::back_inserter(twentyFourBytes));
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextCharacter.fea", twentyFourBytes);

        //when
        std::unique_ptr<SearchBuffer> searchBuffer = std::make_unique<SearchBuffer>(TEST_UUID, printingOrchestrator, pair(0, 0));
        searchBuffer->addEntry(SearchBufferEntry(std::move(twentyFourBytes)));
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(20UL, 0), pair(22UL, 0));
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(26UL, 0), pair(30UL, 0));

        //then
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(0, 0));
        ASSERT_EQ(searchBuffer->getNextCharacter(), 't');
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(1UL, 0));
        ASSERT_EQ(searchBuffer->getNextCharacter(), 'e');
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(2UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getNextCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(6UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getNextCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(10UL, 0));
        ASSERT_EQ(searchBuffer->getNextCharacter(), 't');
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(11UL, 0));
        ASSERT_EQ(searchBuffer->getNextCharacter(), 'e');
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(12UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getNextCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(16UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getNextCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(22UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getNextCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(30UL, 0));
    }

    TEST_F(SearchBufferIntegTest, getPreviousCharacter)
    {
        //before
        utils::windows::FramePositions framePositions = utils::windows::FramePositions();
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        auto twentyFourBytes = TWENTY_BYTE_UTF8_CHARACTER;
        std::copy(FOUR_BYTE_UTF8_CHARACTER.cbegin(), FOUR_BYTE_UTF8_CHARACTER.cend(), std::back_inserter(twentyFourBytes));
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextCharacter.fea", twentyFourBytes);

        //when
        std::unique_ptr<SearchBuffer> searchBuffer = std::make_unique<SearchBuffer>(TEST_UUID, printingOrchestrator, pair(30, 0));
        searchBuffer->addEntry(SearchBufferEntry(std::move(twentyFourBytes)));
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(20UL, 0), pair(22UL, 0));
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(26UL, 0), pair(30UL, 0));

        //then
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(30UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getPreviousCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(22UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getPreviousCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(16UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getPreviousCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(12UL, 0));
        ASSERT_EQ(searchBuffer->getPreviousCharacter(), 'e');
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(11UL, 0));
        ASSERT_EQ(searchBuffer->getPreviousCharacter(), 't');
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(10UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getPreviousCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(6UL, 0));
        ASSERT_EQ(utils::helpers::Conversion::char32ToVector(searchBuffer->getPreviousCharacter()), FOUR_BYTE_UTF8_CHARACTER);
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(2UL, 0));
        ASSERT_EQ(searchBuffer->getPreviousCharacter(), 'e');
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(1UL, 0));
        ASSERT_EQ(searchBuffer->getPreviousCharacter(), 't');
        ASSERT_EQ(searchBuffer->getCurrentPosition(), pair(0UL, 0));
    }
} // namespace feather::utils::algorithm