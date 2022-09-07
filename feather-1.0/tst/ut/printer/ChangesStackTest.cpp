#include "config/Config.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "printer/ChangesStack.hpp"
#include "utils/Utf8Util.hpp"
#include "windows/MainWindowInterface.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace feather::printer;
using namespace ::testing::mocks;

extern std::shared_ptr<const feather::config::Configuration> testConfiguration;

namespace feather::printer
{
class ChangesStackTest : public ::testing::Test, public ::feather::test::TestBase
{
protected:
    std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestratorMock;
    std::shared_ptr<feather::utils::storage::AbstractStorage> storageMock;
    std::unique_ptr<feather::windows::MainWindowInterface> mainWindowMock;

public:
    void SetUp() override
    {
        printingOrchestratorMock = std::shared_ptr<::testing::mocks::PrintingOrchestratorMock>(new ::testing::mocks::PrintingOrchestratorMock());
        storageMock = std::make_shared<StorageMock>();
        mainWindowMock = std::make_unique<NCursesMainWindowDecoratorMock>();
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(mainWindowMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    }
};
/*
TEST_F(ChangesStackTest, addChangeWithoutAppending)
{
    //before
    ChangesStack changesStack(inMemoryStorageFactory, printingOrchestratorMock);

    //when
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 0), pair(5, 0), TWO_BYTE_UTF8_CHARACTER, 2UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 2), pair(5, 2), THREE_BYTE_UTF8_CHARACTER, 3UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 6), pair(5, 6), TEN_BYTE_UTF8_CHARACTER, 10UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(6, 0), pair(6, 0), TEN_BYTE_UTF8_CHARACTER, 10UL, TEST_UUID, false);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage(changesStack.getUndoStackStorageName(TEST_UUID), std::vector<char>{});

    //then
    ASSERT_TRUE(changesStack.redoStack.empty());
    ASSERT_FALSE(changesStack.undoStack.empty());
    ASSERT_EQ(changesStack.undoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(6, 0), pair(6, 0), 10UL, 15UL));
    ASSERT_EQ(readUndoStack(changesStack, 0, 2UL), TWO_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(readUndoStack(changesStack, 2, 3UL), THREE_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(readUndoStack(changesStack, 5, 10UL), TEN_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(readUndoStack(changesStack, changesStack.undoStack.top().stackOffset, 10UL), TEN_BYTE_UTF8_CHARACTER);
    removeStorage(changesStack.getUndoStackStorageName(TEST_UUID));
}

TEST_F(ChangesStackTest, addChangeWithAppending)
{
    //before
    ChangesStack changesStack(inMemoryStorageFactory, printingOrchestratorMock);

    //when
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 0), pair(5, 0), TWO_BYTE_UTF8_CHARACTER, 2UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 2), pair(5, 2), THREE_BYTE_UTF8_CHARACTER, 3UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 6), pair(5, 6), TEN_BYTE_UTF8_CHARACTER, 10UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 16), pair(5, 16), TEN_BYTE_UTF8_CHARACTER, 10UL, TEST_UUID, true);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage(changesStack.getUndoStackStorageName(TEST_UUID), std::vector<char>{});

    //then
    ASSERT_TRUE(changesStack.redoStack.empty());
    ASSERT_FALSE(changesStack.undoStack.empty());
    ASSERT_EQ(changesStack.undoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 6), pair(5, 6), 20UL, 5UL));
    ASSERT_EQ(readUndoStack(changesStack, 0, 2UL), TWO_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(readUndoStack(changesStack, 2UL, 3UL), THREE_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(readUndoStack(changesStack, changesStack.undoStack.top().stackOffset, 20UL), TWENTY_BYTE_UTF8_CHARACTER);

    removeStorage(changesStack.getUndoStackStorageName(TEST_UUID));
}

TEST_F(ChangesStackTest, undoInsertsOnly)
{
    //before

    ChangesStack changesStack(inMemoryStorageFactory, printingOrchestratorMock);

    //when
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 0), pair(5, 0), TWO_BYTE_UTF8_CHARACTER, 2UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 2), pair(5, 2), THREE_BYTE_UTF8_CHARACTER, 3UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(15, 0), pair(5, 6), TEN_BYTE_UTF8_CHARACTER, 10UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 16), pair(15, 6), TEN_BYTE_UTF8_CHARACTER, 10UL, TEST_UUID, true);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage(changesStack.getUndoStackStorageName(TEST_UUID), std::vector<char>{});

    //then
    ASSERT_TRUE(changesStack.redoStack.empty());
    ASSERT_FALSE(changesStack.undoStack.empty());
    ASSERT_EQ(changesStack.undoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(15, 0), pair(5, 6), 20UL, 5UL));

    //when
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(mainWindowMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacters(::testing::_, storageMock, pair(5, 6UL), 20UL));
    auto result = changesStack.undoChange(mainWindowMock);

    //then
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), pair(15, 0));
    ASSERT_EQ(changesStack.undoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 2), pair(5, 2), 3UL, 2UL));
    ASSERT_EQ(changesStack.redoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(15, 0), pair(5, 6), 20UL, 5UL));
    ASSERT_EQ(readUndoStack(changesStack, 0, 2UL), TWO_BYTE_UTF8_CHARACTER);
    ASSERT_EQ(readUndoStack(changesStack, 2UL, 3UL), THREE_BYTE_UTF8_CHARACTER);

    //when
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacters(::testing::_, storageMock, pair(5, 2UL), 3UL));
    result = changesStack.undoChange(mainWindowMock);

    //then
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), pair(5, 2));
    ASSERT_EQ(changesStack.undoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 0), pair(5, 0), 2UL, 0UL));
    ASSERT_EQ(changesStack.redoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 2), pair(5, 2), 3UL, 2UL));
    ASSERT_EQ(readUndoStack(changesStack, 0, 2UL), TWO_BYTE_UTF8_CHARACTER);

    //when
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacters(::testing::_, storageMock, pair(5, 0UL), 2UL));
    result = changesStack.undoChange(mainWindowMock);

    //then
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), pair(5, 0));
    ASSERT_TRUE(changesStack.isUndoStackEmpty());

    removeStorage(changesStack.getUndoStackStorageName(TEST_UUID));
}

TEST_F(ChangesStackTest, redoInsertsOnly)
{
    //before
    ChangesStack changesStack(inMemoryStorageFactory, printingOrchestratorMock);
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(mainWindowMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacters(::testing::_, storageMock, pair(5, 6UL), 20UL));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacters(::testing::_, storageMock, pair(5, 2UL), 3UL));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacters(::testing::_, storageMock, pair(5, 0UL), 2UL));
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 0), pair(5, 0), TWO_BYTE_UTF8_CHARACTER, 2UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 2), pair(5, 2), THREE_BYTE_UTF8_CHARACTER, 3UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 6), pair(5, 6), TEN_BYTE_UTF8_CHARACTER, 10UL, TEST_UUID, false);
    changesStack.addChange(utils::datatypes::ChangeType::INSERT, pair(5, 16), pair(5, 16), TEN_BYTE_UTF8_CHARACTER, 10UL, TEST_UUID, true);
    changesStack.undoChange(mainWindowMock);
    changesStack.undoChange(mainWindowMock);
    changesStack.undoChange(mainWindowMock);
 
    //when
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), addChanges(TEST_UUID, pair(5, 0UL), ::testing::_, ::testing::_));
    ASSERT_EQ(changesStack.redoChange(mainWindowMock).value(), pair(5, 2));

    //then
    ASSERT_FALSE(changesStack.isRedoStackEmpty());
    ASSERT_FALSE(changesStack.isUndoStackEmpty());
    ASSERT_EQ(changesStack.undoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 0), pair(5, 0), 2UL, 0UL));
    ASSERT_EQ(changesStack.redoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 2), pair(5, 2), 3UL, 2UL));
    ASSERT_EQ(readUndoStack(changesStack, 0, 2UL), TWO_BYTE_UTF8_CHARACTER);

    //when
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), addChanges(TEST_UUID, pair(5, 2UL), ::testing::_, ::testing::_));
    ASSERT_EQ(changesStack.redoChange(mainWindowMock).value(), pair(5, 5));

    //then
    ASSERT_FALSE(changesStack.isRedoStackEmpty());
    ASSERT_FALSE(changesStack.isUndoStackEmpty());
    ASSERT_EQ(changesStack.undoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 2), pair(5, 2), 3UL, 2UL));
    ASSERT_EQ(changesStack.redoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 6), pair(5, 6), 20UL, 5UL));
    ASSERT_EQ(readUndoStack(changesStack, 2UL, 3UL), THREE_BYTE_UTF8_CHARACTER);

    //when
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), addChanges(TEST_UUID, pair(5, 6UL), ::testing::_, ::testing::_));
    ASSERT_EQ(changesStack.redoChange(mainWindowMock).value(), pair(5, 26));

    //then
    ASSERT_TRUE(changesStack.isRedoStackEmpty());
    ASSERT_FALSE(changesStack.isUndoStackEmpty());
    ASSERT_EQ(changesStack.undoStack.top(), utils::datatypes::ChangePOD(utils::datatypes::ChangeType::INSERT, pair(5, 6), pair(5, 6), 20UL, 5UL));
    ASSERT_EQ(readUndoStack(changesStack, 5UL, 20UL), TWENTY_BYTE_UTF8_CHARACTER);

    removeStorage(changesStack.getUndoStackStorageName(TEST_UUID));
}
*/
} // namespace feather::printer
