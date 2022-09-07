#include "action/ActionHandler.hpp"
#include "action/SaveChangesAction.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "utils/BufferFillerInterface.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"
#include "windows/WindowsManager.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::action
{
    class ActionHandlerTest : public ::testing::Test
    {
    protected:
        static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
        feather::utils::datatypes::Uuid TEST_UUID;
        WindowImplInterface *nCursesWindowMockMain;
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> inMemoryStorageFactory;
        std::unique_ptr<BufferFillerInterface> bufferFillerMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorMock;
        std::shared_ptr<windows::WindowsManager> windowsManagerMock;
        std::unique_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;

    public:
        void SetUp() override
        {
            nCursesWindowMockMain = new NCursesWindowMock();
            storageFactory = std::make_shared<feather::utils::storage::FileStorageFactory>();
            inMemoryStorageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
            bufferFillerMock = std::make_unique<BufferFillerMock>();
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
            EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), resizeWindow(::testing::Eq(9), ::testing::Eq(10), ::testing::Eq(0), ::testing::Eq(0)));
            EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), disableCursor())
                .Times(1);
            EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), enableCursor())
                .Times(1);
            EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getMaxScreenNoRowsColumns())
                .WillRepeatedly(::testing::Return(std::pair<size_t, size_t>(10, 10)));
            nCursesMainWindowDecoratorMock = std::make_unique<NCursesMainWindowDecoratorMock>();
            TEST_UUID = utils::datatypes::Uuid(testUUID);
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getBufferFiller())
                .WillRepeatedly(::testing::ReturnRef(bufferFillerMock));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        }
    };
}
