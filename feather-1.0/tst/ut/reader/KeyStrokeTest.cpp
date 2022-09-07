#include "mocks/FileLogPolicyMock.hpp"
#include "mocks/FileMappingWrapperMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/StorageMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "reader/KeyStroke.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/exception/MirrorLogicalException.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

extern std::shared_ptr<const feather::config::Configuration> configuration;

namespace feather::reader
{
class KeyStrokeTest : public ::testing::Test
{
protected:
    std::string fileName;
    static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
    std::shared_ptr<utils::Utf8Util> utf8Util;
    std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
    std::unique_ptr<FileMappingWrapper> fileMappingWrapperMock;
    std::unique_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
    WindowImplInterface *nCursesWindowMockMain;
    NCursesWrapperMock *nCursesWrapperMock;
    utils::datatypes::Uuid TEST_UUID;
    std::shared_ptr<feather::utils::storage::AbstractStorage> storageMock;
    std::shared_ptr<windows::WindowsManager> windowsManagerMock;
    std::shared_ptr<::testing::mocks::FileLogPolicyMock> fileLogPolicyMock;

public:
    void SetUp() override
    {
        fileName = "testFile";
        fileMappingWrapperMock = std::make_unique<FileMappingWrapperMock>(nullptr, nullptr);
        nCursesWrapperMock = new NCursesWrapperMock();
        nCursesWindowMockMain = new NCursesWindowMock();
        TEST_UUID = utils::datatypes::Uuid(testUUID);
        storageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), resizeWindow(::testing::Eq(8), ::testing::Eq(10), ::testing::Eq(0), ::testing::Eq(0)))
            .Times(1);
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getMaxScreenNoRowsColumns())
            .Times(2)
            .WillRepeatedly(::testing::Return(pair(10, 10)));
        nCursesMainWindowDecoratorMock = std::make_unique<NCursesMainWindowDecoratorMock>(fileMappingWrapperMock, nCursesWindowMockMain);
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID())
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        storageMock = std::make_shared<StorageMock>();
        configuration->forceTextMode(fileName);
        windowsManagerMock = std::make_shared<::testing::mocks::WindowsManagerMock>();
    }

    void TearDown() override
    {
    }
};

TEST_F(KeyStrokeTest, getProperPreviousPositionWithChanges)
{
    //before
    KeyStroke keyStroke(windowsManagerMock, fileLogPolicyMock , storageFactory);

    //when

    //then
}
} // namespace feather::printer
