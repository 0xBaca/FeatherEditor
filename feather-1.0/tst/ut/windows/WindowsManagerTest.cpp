#include "mocks/FileLogPolicyMock.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesBottomBarWindowDecoratorMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/WindowsFactoryMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/StorageMock.hpp"
#include "utils/exception/FeatherLogicalException.hpp"
#include "utils/logger/policy/OutputDeviceLogPolicy.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"
#include "windows/NCursesMainWindowDecorator.hpp"
#include "windows/NCursesProgressWindowDecorator.hpp"
#include "windows/WindowsManager.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing::mocks;
using namespace feather::utils;
using ::testing::_;
using ::testing::ByMove;
using ::testing::Return;
using ::testing::Throw;

namespace feather::windows
{
    class WindowsManagerTest : public ::testing::Test
    {
    protected:
        static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage;
        std::shared_ptr<logger::policy::OutputDeviceLogPolicy> log;
        WindowsAbstractFactory *windowsFactoryMock;
        NCursesBottomBarWindowDecorator *nCursesBottomBarWindowDecoratorMock;
        std::shared_ptr<WindowsManager> manager;
        utils::datatypes::Uuid TEST_UUID;
        // std::unique_ptr<BufferFillerInterface> bufferFillerMock;

    public:
        virtual void SetUp()
        {
            storage = std::make_shared<StorageMock>();
            log = std::make_shared<logger::policy::OutputDeviceLogPolicy>();
            TEST_UUID = feather::utils::datatypes::Uuid(testUUID);
            // windowsFactoryMock = new WindowsFactoryMock();
            // bufferFillerMock = std::make_unique<BufferFillerMock>();
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(WindowsManagerTest, successfullyGetInstanceOfWindowsManager)
    {
        // before

        // when
        // ASSERT_NO_THROW(manager = WindowsManager::getInstance(windowsFactoryMock, log));

        // then
        //ASSERT_NE(manager.get(), NULL);
    }
    /*
TEST_F(WindowsManagerTest, successfullyCreatesMainWindow)
{
    //before
    NCursesWrapper *nCursesMainWindowWrapperMock = new NCursesWrapperMock();
    NCursesWindow *nCursesWindowMockMain = new NCursesWindowMock(nCursesMainWindowWrapperMock);
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getMaxScreenNoRowsColumns())
        .Times(2)
        .WillRepeatedly(Return(std::pair<size_t, size_t>(100, 100)));
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), resizeWindow(::testing::Eq(99), ::testing::Eq(100), ::testing::Eq(0), ::testing::Eq(0)))
        .Times(1);
    std::unique_ptr<NCursesMainWindowDecorator> nCursesMainWindowDecoratorMock =
        std::make_unique<NCursesMainWindowDecoratorMock>(bufferFillerMock, nCursesWindowMockMain);
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock*>(nCursesMainWindowDecoratorMock.get()), getUUID())
        .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    EXPECT_CALL(*static_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .Times(1)
        .WillOnce(::testing::ReturnRef(storage));
    EXPECT_CALL(*static_cast<StorageMock *>(storage.get()), getName())
        .Times(1)
        .WillOnce(Return("testStorage"));
     EXPECT_CALL(*static_cast<NCursesWindowsFactoryMock *>(windowsFactoryMock), createMainWindow(bufferFillerMock, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Return(std::unique_ptr<NCursesMainWindowDecorator>(nCursesMainWindowDecoratorMock.get())));

    //when
    ASSERT_NO_THROW(manager = WindowsManager::getInstance(windowsFactoryMock, log));
    auto &mainWindow = manager->addNCursesMainWindow(bufferFillerMock, std::nullopt);

    //then
    ASSERT_NE(manager.get(), nullptr);
    ASSERT_NE(mainWindow.get(), nullptr);
    ASSERT_EQ(manager->getMainWindow(mainWindow->getUUID())->getUUID(), mainWindow->getUUID());
}

TEST_F(WindowsManagerTest, doNotAllowForCreationOfTheSameMainWindow)
{
    //before
    NCursesWrapper *nCursesMainWindowWrapperMock = new NCursesWrapperMock();
    NCursesWindow *nCursesWindowMockMain = new NCursesWindowMock(nCursesMainWindowWrapperMock);
    auto matcher = ::testing::StrEq("Could not insert the same main window... ");
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getMaxScreenNoRowsColumns())
        .Times(2)
        .WillRepeatedly(Return(std::pair<size_t, size_t>(100, 100)));
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), resizeWindow(::testing::Eq(99), ::testing::Eq(100), ::testing::Eq(0), ::testing::Eq(0)))
        .Times(1);
    std::shared_ptr<NCursesMainWindowDecorator> nCursesMainWindowDecoratorMock =
        std::make_shared<NCursesMainWindowDecoratorMock>(bufferFillerMock, nCursesWindowMockMain);
    EXPECT_CALL(*std::dynamic_pointer_cast<NCursesMainWindowDecoratorMock>(nCursesMainWindowDecoratorMock), getUUID())
        .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    EXPECT_CALL(*static_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .Times(1)
        .WillOnce(::testing::ReturnRef(storage));
    EXPECT_CALL(*static_cast<StorageMock *>(storage.get()), getName())
        .Times(1)
        .WillOnce(Return("testStorage"));
    EXPECT_CALL(*static_cast<NCursesWindowsFactoryMock *>(windowsFactoryMock), createMainWindow(bufferFillerMock, ::testing::_))
        .Times(2)
        .WillRepeatedly(Return(std::move(nCursesMainWindowDecoratorMock)));

    //when
    ASSERT_NO_THROW(manager = WindowsManager::getInstance(windowsFactoryMock, log));
    try
    {
        auto const &mainWindow = manager->addNCursesMainWindow(bufferFillerMock, std::nullopt);
        auto const &mainWindow2 = manager->addNCursesMainWindow(bufferFillerMock, std::nullopt);
    }
    catch (exception::FeatherLogicalException &e)
    {
        ASSERT_THAT(e.what(), matcher);
        return;
    }

    //then
    ASSERT_TRUE(0);
}

TEST_F(WindowsManagerTest, successfullyCreateBottomBarWindow)
{
    //before
    NCursesWrapper *nCursesMainWindowWrapperMock = new NCursesWrapperMock();
    NCursesWindow *nCursesWindowMockMain = new NCursesWindowMock(nCursesMainWindowWrapperMock);
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getMaxScreenNoRowsColumns())
        .Times(4)
        .WillRepeatedly(Return(std::pair<size_t, size_t>(100, 100)));
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), resizeWindow(::testing::Eq(99), ::testing::Eq(100), ::testing::Eq(0), ::testing::Eq(0)))
        .Times(1);
    std::unique_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock =
        std::make_unique<NCursesMainWindowDecoratorMock>(bufferFillerMock, nCursesWindowMockMain);
    std::unique_ptr<SubWindowInterface> nCursesBottomBarWindowDecoratorMock = std::make_unique<NCursesBottomBarWindowDecoratorMock>(nCursesMainWindowDecoratorMock, nullptr);
    NCursesWrapper *nCursesBottomBarWindowWrapperMock = new NCursesWrapperMock();
    NCursesWindow *nCursesBottomBarWindowMock = new NCursesWindowMock(nCursesBottomBarWindowWrapperMock);
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID())
        .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
        .Times(2)
        .WillRepeatedly(Return(nCursesWindowMockMain));
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesBottomBarWindowMock), resizeWindow(1, 100, 0, 99))
        .Times(1);
    EXPECT_CALL(*static_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .Times(1)
        .WillOnce(::testing::ReturnRef(storage));
    EXPECT_CALL(*static_cast<StorageMock *>(storage.get()), getName())
        .Times(1)
        .WillOnce(Return("testStorage"));

    //when
    ASSERT_NO_THROW(manager = WindowsManager::getInstance(std::move(windowsFactoryMock), log));
    auto &mainWindow = manager->addNCursesMainWindow(bufferFillerMock, std::nullopt);
    auto &bottomBarWindow = manager->addNCursesBottomBarWindow(mainWindow);

    //then
    ASSERT_NE(manager.get(), nullptr);
    ASSERT_NE(mainWindow.get(), nullptr);
    ASSERT_EQ(manager->getMainWindow(mainWindow->getUUID())->getUUID(), mainWindow->getUUID());
    ASSERT_EQ(manager->getSubWindow(bottomBarWindow->getUUID())->getUUID(), bottomBarWindow->getUUID());
}

TEST_F(WindowsManagerTest, successfullyCreateProgressWindow)
{
    //before
    NCursesWrapper *nCursesMainWindowWrapperMock = new NCursesWrapperMock();
    NCursesWindow *nCursesWindowMockMain = new NCursesWindowMock(nCursesMainWindowWrapperMock);
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getMaxScreenNoRowsColumns())
        .Times(2)
        .WillRepeatedly(Return(std::pair<size_t, size_t>(100, 100)));
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), resizeWindow(::testing::Eq(99), ::testing::Eq(100), ::testing::Eq(0), ::testing::Eq(0)))
        .Times(1);
    std::shared_ptr<NCursesMainWindowDecorator> nCursesMainWindowDecoratorMock =
        std::make_shared<NCursesMainWindowDecoratorMock>(bufferFillerMock, nCursesWindowMockMain);
    NCursesWrapper *nCursesProgressWindowWrapperMock = new NCursesWrapperMock();
    NCursesWindowMock *nCursesWindowMockProgress = new NCursesWindowMock(nCursesProgressWindowWrapperMock);
    EXPECT_CALL(*std::dynamic_pointer_cast<NCursesMainWindowDecoratorMock>(nCursesMainWindowDecoratorMock), getUUID())
        .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    EXPECT_CALL(*std::dynamic_pointer_cast<NCursesMainWindowDecoratorMock>(nCursesMainWindowDecoratorMock), getLibraryWindowHandler())
        .Times(4)
        .WillRepeatedly(Return(nCursesWindowMockMain));
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getNoColumns())
        .Times(2)
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getNoRows())
        .Times(2)
        .WillRepeatedly(Return(100));
    EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockProgress), resizeWindow(1, 20, 80, 100))
        .Times(1);
    EXPECT_CALL(*static_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .Times(1)
        .WillOnce(::testing::ReturnRef(storage));
    EXPECT_CALL(*static_cast<StorageMock *>(storage.get()), getName())
        .Times(1)
        .WillOnce(Return("testStorage"));
    EXPECT_CALL(*static_cast<NCursesWindowsFactoryMock *>(windowsFactoryMock), createMainWindow(bufferFillerMock, ::testing::_))
        .Times(1)
        .WillOnce(Return(ByMove(std::move(nCursesMainWindowDecoratorMock))));

    //when
    ASSERT_NO_THROW(manager = WindowsManager::getInstance(std::move(windowsFactoryMock), log));
    auto &mainWindow = manager->addNCursesMainWindow(bufferFillerMock, std::nullopt);
    auto &progressWindow = manager->addNCursesProgressWindow(mainWindow);

    //then
    ASSERT_NE(manager.get(), nullptr);
    ASSERT_NE(mainWindow.get(), nullptr);
    ASSERT_EQ(manager->getMainWindow(mainWindow->getUUID())->getUUID(), mainWindow->getUUID());
    ASSERT_EQ(manager->getSubWindow(progressWindow->getUUID())->getUUID(), progressWindow->getUUID());
}
*/
} // namespace feather::windows
