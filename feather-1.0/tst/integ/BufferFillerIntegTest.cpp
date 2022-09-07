#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "printer/MemoryPrintingStorage.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/BufferFiller.hpp"
#include "utils/FilledChunk.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/helpers/Conversion.hpp"
#include "utils/storage/InMemoryStorage.hpp"
#include "utils/windows/FramePositions.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing::mocks;
using namespace feather::printer;

namespace feather::utils
{
    class BufferFillerIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<PrintingOrchestrator> printingOrchestrator;
        std::shared_ptr<PrintingStorageInterface> filePrintingStorage;

    public:
        void SetUp() override
        {
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        }
    };

    TEST_F(BufferFillerIntegTest, fillCacheBuffer)
    {
        //before
        const char *commandLineArguments[] = {"./feather", "file", "--cache=4", "--relax=4"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("fillCacheBuffer.BufferFillerIntegTest", TWENTY_BYTE_UTF8_CHARACTER);
        std::unique_ptr<BufferFiller> bufferFiller = std::make_unique<BufferFiller>(storage, inMemoryStorageFactory);

        //when
        auto cacheBuffer = bufferFiller->fillCacheBuffer(TEST_UUID, printingOrchestrator, 20, pair(0, 0));

        //then
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(0, 0));
        ASSERT_EQ(cacheBuffer->getSize(), 20UL);
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 20UL);

        ASSERT_EQ(cacheBuffer->getNextCharacter(), 't');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(1, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 19UL);

        ASSERT_EQ(cacheBuffer->getNextCharacter(), 'e');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(2, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 18UL);

        ASSERT_EQ(cacheBuffer->getNextCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(6, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 14UL);

        ASSERT_EQ(cacheBuffer->getNextCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(10, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 10UL);

        ASSERT_EQ(cacheBuffer->getNextCharacter(), 't');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(11, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 9UL);

        ASSERT_EQ(cacheBuffer->getNextCharacter(), 'e');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(12, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 8UL);

        ASSERT_EQ(cacheBuffer->getNextCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(16, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 4UL);

        ASSERT_EQ(cacheBuffer->getNextCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(20, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 0UL);
    }

    TEST_F(BufferFillerIntegTest, fillCacheBufferBackward)
    {
        //before
        const char *commandLineArguments[] = {"./feather", "file", "--cache=4", "--relax=10"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("fillCacheBuffer.BufferFillerIntegTest", TWENTY_BYTE_UTF8_CHARACTER);
        std::unique_ptr<BufferFiller> bufferFiller = std::make_unique<BufferFiller>(storage, inMemoryStorageFactory);

        //when
        auto cacheBuffer = bufferFiller->fillCacheBufferBackward(TEST_UUID, printingOrchestrator, 16UL, pair(16UL, 0));

        //then
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(16, 0));
        ASSERT_EQ(cacheBuffer->getSize(), 16UL);
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 16UL);

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(12, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 12UL);

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 'e');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(11, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 11UL);

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 't');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(10, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 10UL);

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(6, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 6UL);

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(2, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 2UL);

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 'e');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(1, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 1UL);

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 't');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(0, 0));
        ASSERT_EQ(cacheBuffer->getNoUnreadBytes(), 0UL);
    }

    TEST_F(BufferFillerIntegTest, fillCacheBufferBackward_2)
    {
        //before
        const char *commandLineArguments[] = {"./feather", "file", "--cache=4", "--relax=4"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("fillCacheBuffer.BufferFillerIntegTest", TWENTY_BYTE_UTF8_CHARACTER);
        std::unique_ptr<BufferFiller> bufferFiller = std::make_unique<BufferFiller>(storage, inMemoryStorageFactory);

        //when
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(10, 0), pair(12, 0));
        auto cacheBuffer = bufferFiller->fillCacheBufferBackward(TEST_UUID, printingOrchestrator, 16UL, pair(16UL, 0));

        //then
        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(12, 0));

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(6, 0));

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), utils::helpers::Conversion::vectorOfCharToChar32_t(FOUR_BYTE_UTF8_CHARACTER));
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(2, 0));

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 'e');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(1, 0));

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 't');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(0, 0));
    }

    TEST_F(BufferFillerIntegTest, fillCacheBufferBackward_3)
    {
        //before
        const char *commandLineArguments[] = {"./feather", "file", "--cache=4", "--relax=4"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("fillCacheBuffer.BufferFillerIntegTest", std::vector<char>{'t', '\n', 'e', '\n', 'e'});
        std::unique_ptr<BufferFiller> bufferFiller = std::make_unique<BufferFiller>(storage, inMemoryStorageFactory);

        //when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(2, 0), std::vector<char>{'t', '\n', 'e'}, 3UL);
        auto cacheBuffer = bufferFiller->fillCacheBufferBackward(TEST_UUID, printingOrchestrator, 8UL, pair(4UL, 0));

        //then
        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), '\n');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(3, 0));

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 'e');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(2, 3));

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 'e');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(2, 2));

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), '\n');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(2, 1));

        ASSERT_EQ(cacheBuffer->getPreviousCharacter(), 't');
        ASSERT_EQ(cacheBuffer->getCurrentPosition(), pair(2, 0));
    }

    TEST_F(BufferFillerIntegTest, rewindLineWhenNewChanges)
    {
        //before
        const char *commandLineArguments[] = {"./feather", "file", "--cache=4", "--relax=4"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("fillCacheBuffer.BufferFillerIntegTest", TWENTY_BYTE_UTF8_CHARACTER);
        std::unique_ptr<BufferFiller> bufferFiller = std::make_unique<BufferFiller>(storage, inMemoryStorageFactory);
        std::unique_ptr<feather::windows::WindowImplInterface> nCursesWindowMock = std::make_unique<NCursesWindowMock>();
        ScreenBuffer expectedBuffer(2UL);
        expectedBuffer.fillBufferLine({0x9A});
        expectedBuffer.fillBufferLine({0x9a989ff0});

        //when
        bufferFiller->setStartFramePosition(pair(16, 0));
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(15, 0), std::vector<char>{'\n', 't', 't', 't', 'e', '\n'}, 6UL);
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getWindowDimensions())
            .WillRepeatedly(::testing::Return(pair(2, 2)));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), isSecondaryHexModeMainWindow())
            .WillRepeatedly(::testing::Return(false));
        auto resultBuffer = bufferFiller->getNewWindowBuffer(nCursesMainWindowDecoratorMock.get(), utils::FilledChunk::LINE_UP, printingOrchestrator);

        //then
        ASSERT_TRUE(expectedBuffer == resultBuffer);
    }

    /*
    t \n
    e \n
    */
    TEST_F(BufferFillerIntegTest, rewindLineWhenNewChanges_2)
    {
        //before
        const char *commandLineArguments[] = {"./feather", "file", "--cache=4", "--relax=4"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("rewindLineWhenNewChanges_2", std::vector<char>{'t', '\n', 'e', '\n', 'e'});
        std::unique_ptr<BufferFiller> bufferFiller = std::make_unique<BufferFiller>(storage, inMemoryStorageFactory);
        ScreenBuffer expectedBuffer(2UL);
        expectedBuffer.fillBufferLine({U't', U'\n'});
        expectedBuffer.fillBufferLine({U't', U'\n'});

        //when
        bufferFiller->setStartFramePosition(pair(2, 0));
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(2, 0), std::vector<char>{'t', '\n', 'e'}, 3UL);
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getWindowDimensions())
            .WillRepeatedly(::testing::Return(pair(2, 2)));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), isSecondaryHexModeMainWindow())
            .WillRepeatedly(::testing::Return(false));
        auto resultBuffer = bufferFiller->getNewWindowBuffer(nCursesMainWindowDecoratorMock.get(), utils::FilledChunk::LINE_UP, printingOrchestrator);

        //then
        ASSERT_TRUE(expectedBuffer == resultBuffer);

        //when
        resultBuffer = bufferFiller->getNewWindowBuffer(nCursesMainWindowDecoratorMock.get(), utils::FilledChunk::LINE_UP, printingOrchestrator);

        //then
        ASSERT_TRUE(expectedBuffer == resultBuffer);
    }

    TEST_F(BufferFillerIntegTest, changeStorageBecameSmaller)
    {
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("changeStorageBecameSmaller", {'t', '\n', 'e', '\n', 'e'});
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            //before
            const char *commandLineArguments[] = {"./feather", "file", "--cache=4", "--relax=4"};
            utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
            configuration->overrideConfiguration();
            std::shared_ptr<PrintingStorageInterface> memoryPrintingStorage = std::make_shared<MemoryPrintingStorage>();
            static_cast<utils::storage::InMemoryStorage *>(storage.get())->mockReloadStorageContent({'t', '\n', 'e', '\n', 'e'});
            std::unique_ptr<BufferFiller> bufferFiller = std::make_unique<BufferFiller>(storage, inMemoryStorageFactory);
            ScreenBuffer expectedBuffer(2UL);
            expectedBuffer.fillBufferLine({U'e', U'e'});
            expectedBuffer.fillBufferLine({U'\n'});

            //when
            bufferFiller->setStartFramePosition(pair(2, 0));
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(2, 0), std::vector<char>{'t', '\n', 'e'}, 3UL);
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getWindowDimensions())
                .WillRepeatedly(::testing::Return(pair(2, 2)));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), isSecondaryHexModeMainWindow())
                .WillRepeatedly(::testing::Return(false));
            auto resultBuffer = bufferFiller->getNewWindowBuffer(nCursesMainWindowDecoratorMock.get(), utils::FilledChunk::LINE_DOWN, printingOrchestrator);

            //then
            ASSERT_TRUE(expectedBuffer == resultBuffer);

            //when
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), setCursorPosition(pair(0, 0)))
                .Times(1);
            ScreenBuffer expectedBuffer2(1UL);
            expectedBuffer2.fillBufferLine({U'M'});
            static_cast<utils::storage::InMemoryStorage *>(storage.get())->mockReloadStorageContent({U'M'});
            resultBuffer = bufferFiller->getNewWindowBuffer(nCursesMainWindowDecoratorMock.get(), utils::FilledChunk::LINE_UP, printingOrchestrator);
            /*
            for (int i = 0; i < resultBuffer.getFilledRows(); ++i)
            {
                for (auto a : resultBuffer[i])
                {
                    std::cout << std::hex << (int)a << " ";
                }
                std::cout << std::endl;
            }
            */
            //then
            ASSERT_TRUE(expectedBuffer2 == resultBuffer);
        }
    }

} // namespace feather::utils
