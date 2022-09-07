#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "action/MoveToPreviousWordAction.hpp"
#include "helpers/TestBase.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::action
{
    class MoveToPreviousWordActionTest : public ::testing::Test, public feather::test::TestBase
    {
    protected:
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> inMemoryStorageFactory;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorMock;
        std::shared_ptr<WindowsManager> windowsManagerMock;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<feather::utils::storage::AbstractStorage> storageMock;
        std::shared_ptr<WindowImplInterface> nCursesWindowMock;

    public:
        void SetUp() override
        {
            inMemoryStorageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
            printingOrchestratorMock = std::make_shared<PrintingOrchestratorMock>();
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
            storageMock = std::make_shared<StorageMock>();
            nCursesWindowMock = std::make_shared<NCursesWindowMock>();
        }
    };

    TEST_F(MoveToPreviousWordActionTest, moveWordBackward)
    {
        // before
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(printingOrchestratorMock.get()))
            .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(1300, 0)));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storageMock));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharacters(pair(1300UL, 0UL), MAX_UTF8_CHAR_BYTES, storageMock, TEST_UUID, false, std::map<pair, pair>{}))
            .WillRepeatedly(::testing::Return(std::make_pair(FOUR_BYTE_UTF8_CHARACTER, pair(1304, 0))));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharactersBackward(pair(1300UL, 0UL), 1048576, storageMock, TEST_UUID))
            .WillRepeatedly(::testing::Return(std::make_pair(std::vector<char>{' '}, pair(1299, 0))));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isPreviousCharacterDeleted(pair(1300UL, 0), TEST_UUID))
            .WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesBytesAtPos(1299UL, TEST_UUID))
            .WillRepeatedly(::testing::Return(0));

        std::unique_ptr<MoveToPreviousWordAction> action = std::make_unique<MoveToPreviousWordAction>(MoveToPreviousWordActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock, windowsManagerMock));

        // when
        auto result = action->execute();

        // then
        auto properCursorPos = std::pair<size_t, size_t>(1299ULL, 0ULL);
        ASSERT_EQ(result.getResult(), properCursorPos);
    }

    TEST_F(MoveToPreviousWordActionTest, dontMoveWhenHitFileBegin)
    {
        // before
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(printingOrchestratorMock.get()))
            .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(4UL, 0)));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storageMock));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharacters(pair(4UL, 0UL), MAX_UTF8_CHAR_BYTES, storageMock, TEST_UUID, false, std::map<pair, pair>{}))
            .WillRepeatedly(::testing::Return(std::make_pair(FOUR_BYTE_UTF8_CHARACTER, pair(8UL, 0))));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharactersBackward(pair(4UL, 0UL), 1048576, storageMock, TEST_UUID))
            .WillRepeatedly(::testing::Return(std::make_pair(FOUR_BYTE_UTF8_CHARACTER, pair(4UL, 0))));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharactersBackward(pair(0UL, 0UL), 1048576, storageMock, TEST_UUID))
            .WillRepeatedly(::testing::Return(std::make_pair(std::vector<char>{}, pair(0UL, 0))));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isPreviousCharacterDeleted(::testing::_, TEST_UUID))
            .WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesBytesAtPos(::testing::_, TEST_UUID))
            .WillRepeatedly(::testing::Return(0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));

        std::unique_ptr<MoveToPreviousWordAction> action = std::make_unique<MoveToPreviousWordAction>(MoveToPreviousWordActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock, windowsManagerMock));

        // when
        auto result = action->execute();

        // then
        auto properCursorPos = std::pair<size_t, size_t>(0UL, 0UL);
        ASSERT_EQ(result.getResult(), properCursorPos);
    }
} // namespace feather::action
