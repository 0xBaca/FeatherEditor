#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/FileLogPolicyMock.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/AbstractStorageFactory.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"
#include "windows/NCursesMainWindowDecorator.hpp"
#include "windows/NCursesProgressWindowDecorator.hpp"
#include "windows/NCursesWindowsFactory.hpp"
#include "windows/WindowsAbstractFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;
using ::testing::ByMove;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Throw;

namespace feather::windows
{
    class NCursesWindowsFactoryTest : public ::testing::Test
    {
    protected:
        static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
        utils::datatypes::Uuid TEST_UUID;
        static const char *LOG_FILE_NAME;
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<FileLogPolicyMock> fileLogPolicyMock;
        std::unique_ptr<BufferFillerInterface> bufferFillerMock;

        virtual void SetUp()
        {
            TEST_UUID = feather::utils::datatypes::Uuid(testUUID);
            storageFactory = std::make_shared<feather::utils::storage::FileStorageFactory>();
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(NCursesWindowsFactoryTest, successfullyCreateNCursesMainWindow)
    {
        // before
        std::shared_ptr<feather::utils::storage::AbstractStorage> fileStorage = storageFactory->getStorage("./tst/ut/helpers/testfiles/fileWrapperTestFile", TEST_UUID);
        bufferFillerMock = std::make_unique<BufferFillerMock>();
        WindowsAbstractFactory *nCursesFactory = NCursesWindowsFactory::getInstance(fileLogPolicyMock);

        // when
        auto nCursesWindow = nCursesFactory->createMainWindow(std::move(bufferFillerMock), std::nullopt);

        // then
        EXPECT_NE(nullptr, dynamic_cast<NCursesMainWindowDecorator *>(nCursesWindow.get()));
        ASSERT_NE(static_cast<NCursesMainWindowDecorator *>(nCursesWindow.get())->getBufferFiller(), nullptr);
        ASSERT_NE(nCursesWindow->getLibraryWindowHandler(), nullptr);
        delete nCursesFactory;
    }

    TEST_F(NCursesWindowsFactoryTest, successfullyCreateNCursesBottomBarWindow)
    {
        // before
        std::shared_ptr<feather::utils::storage::AbstractStorage> fileStorage = storageFactory->getStorage("./tst/ut/helpers/testfiles/fileWrapperTestFile", TEST_UUID);
        bufferFillerMock = std::make_unique<BufferFillerMock>();
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getWindowDimensions())
            .WillRepeatedly(::testing::Return(std::pair<size_t, size_t>(100, 100)));
        WindowsAbstractFactory *nCursesFactory = NCursesWindowsFactory::getInstance(fileLogPolicyMock);
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));

        // when
        auto nCursesWindow = nCursesFactory->createBottomBarWindow(nCursesMainWindowDecoratorMock);

        // then
        EXPECT_NE(nullptr, dynamic_cast<NCursesBottomBarWindowDecorator *>(nCursesWindow.get()));
        ASSERT_EQ(nCursesWindow->getParentWindowHandler()->getUUID(), nCursesMainWindowDecoratorMock->getUUID());
        delete nCursesFactory;
    }

    TEST_F(NCursesWindowsFactoryTest, successfullyCreateNCursesProgressWindow)
    {
        // before
        std::shared_ptr<feather::utils::storage::AbstractStorage> fileStorage = storageFactory->getStorage("./tst/ut/helpers/testfiles/fileWrapperTestFile", TEST_UUID);
        bufferFillerMock = std::make_unique<BufferFillerMock>();
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        WindowsAbstractFactory *nCursesFactory = NCursesWindowsFactory::getInstance(fileLogPolicyMock);

        // when
        auto nCursesWindow = nCursesFactory->createProgressWindow(nCursesMainWindowDecoratorMock);

        // then
        EXPECT_NE(nullptr, dynamic_cast<NCursesProgressWindowDecorator *>(nCursesWindow.get()));
        ASSERT_EQ(nCursesWindow->getParentWindowHandler()->getUUID(), nCursesMainWindowDecoratorMock->getUUID());
        delete nCursesFactory;
    }
} // namespace feather::windows
