#include "helpers/TestBase.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "config/Config.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "windows/NCursesMainWindowDecorator.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;
using ::testing::Return;

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::windows
{
    class NCursesMainWindowDecoratorTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::unique_ptr<BufferFillerInterface> bufferFillerMock;
        WindowImplInterface *nCursesWindowMockMain;
        std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestratorMock;

    public:
        virtual void SetUp()
        {
            configuration->forceTextMode(testFile);
            storage = createStorage("testFile", TEN_BYTE_UTF8_CHARACTER);
            bufferFillerMock = std::make_unique<BufferFillerMock>();
            nCursesWindowMockMain = new NCursesWindowMock();
            printingOrchestratorMock = std::make_shared<PrintingOrchestratorMock>();
        }
    };

    /*
    Original screen:
    😊ℕℕ\n
    ℝℝℝ\n

    Modified screen:
    😊abℕ\n
    ℝℝ\n

    Character(a b) are added to real position 4.
    Characters at pos 4 and 11 are deleted, and cursor is at line 0 pos 2, which is b
    TEST_F(NCursesMainWindowDecoratorTest, getRealPositionWithChanges)
    {
        // before
        configuration->forceTextMode(testFile);
        std::unique_ptr<NCursesMainWindowDecorator> mainWindowDecorator;
        utils::windows::FramePositions pos{pair{0, 0}, pair{10, 0}};
        utils::ScreenBuffer buffer(2);
        buffer.fillBufferLine({0x8A989FF0, 97, 98, 0x9584E2, '\n'});
        buffer.fillBufferLine({0x9D84E2, 0x9D84E2, '\n'});

        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getUUID())
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillRepeatedly(::testing::ReturnRef(pos));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMockMain), getCursorPosition(true))
            .WillOnce(::testing::Return(pair(0, 4)))
            .WillOnce(::testing::Return(pair(0, 5)))
            .WillOnce(::testing::Return(pair(1, 0)))
            .WillOnce(::testing::Return(pair(1, 2)));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
            .WillRepeatedly(::testing::ReturnRef(buffer));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID))
            .WillRepeatedly(Return(false));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesBytesAtPos(::testing::_, TEST_UUID))
            .WillRepeatedly(Return(0));

        // when
        mainWindowDecorator = std::make_unique<NCursesMainWindowDecorator>(std::move(bufferFillerMock), nCursesWindowMockMain);

        // then
        ASSERT_EQ(mainWindowDecorator->getCursorRealPosition(printingOrchestratorMock.get()), pair(6UL, 0));
        ASSERT_EQ(mainWindowDecorator->getCursorRealPosition(printingOrchestratorMock.get()), pair(9UL, 0UL));
        ASSERT_EQ(mainWindowDecorator->getCursorRealPosition(printingOrchestratorMock.get()), pair(10UL, 0));
        ASSERT_EQ(mainWindowDecorator->getCursorRealPosition(printingOrchestratorMock.get()), pair(16UL, 0));
    }
    */
} // namespace feather::windows
