#include "helpers/FileStorageHelper.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::windows
{
    class BottomBarWindowDecoratorTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage;
        std::shared_ptr<Utf8Util> utf8Util;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        NCursesWrapper *nCursesMainWindowWrapperMock;
        NCursesWrapper *nCursesBottomBarWindowWrapperMock;
        WindowImplInterface *nCursesWindowMock;

    public:
        virtual void SetUp()
        {
            storageFactory = std::make_shared<feather::utils::storage::FileStorageFactory>();
            storage = storageFactory->getStorage("./tst/ut/helpers/testfiles/fileWrapperTestFile", TEST_UUID);
            nCursesMainWindowWrapperMock = new NCursesWrapperMock();
            nCursesBottomBarWindowWrapperMock = new NCursesWrapperMock();
            nCursesWindowMock = new NCursesWindowMock();
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
        }
    };

    TEST_F(BottomBarWindowDecoratorTest, createdSuccessfully)
    {
        // before
        std::unique_ptr<NCursesBottomBarWindowDecorator> bottomBarWindowDecorator;

        // when
        ASSERT_NO_THROW(bottomBarWindowDecorator = std::make_unique<NCursesBottomBarWindowDecorator>(nCursesMainWindowDecoratorMock, nCursesWindowMock));

        // then
        ASSERT_THAT(bottomBarWindowDecorator, ::testing::NotNull());
        ASSERT_THAT(bottomBarWindowDecorator->getLibraryWindowHandler(), ::testing::Eq(nCursesWindowMock));
    }

    TEST_F(BottomBarWindowDecoratorTest, readHistory)
    {
        // before
        std::unique_ptr<NCursesBottomBarWindowDecorator> bottomBarWindowDecorator;

        // when
        bottomBarWindowDecorator = std::make_unique<NCursesBottomBarWindowDecorator>(nCursesMainWindowDecoratorMock, nCursesWindowMock);
        bottomBarWindowDecorator->addToActionHistory(std::u32string{U"historyTest1"});
        bottomBarWindowDecorator->addToActionHistory(std::u32string{U"historyTest2"});
        bottomBarWindowDecorator->addToActionHistory(std::u32string{U"historyTest3"});

        // then
        auto currEntry = bottomBarWindowDecorator->getCurrFromActionHistory();
        ASSERT_FALSE(std::nullopt == currEntry);

        // then
        currEntry = bottomBarWindowDecorator->getPreviousFromActionHistory();
        FileStorageHelper::compareLine((*currEntry).begin(), (*currEntry).end(), utf8Util, std::u32string(U"historyTest1").begin());

        // then
        currEntry = bottomBarWindowDecorator->getNextFromActionHistory();
        FileStorageHelper::compareLine((*currEntry).begin(), (*currEntry).end(), utf8Util, std::u32string(U"historyTest2").begin());

        // then
        currEntry = bottomBarWindowDecorator->getNextFromActionHistory();
        FileStorageHelper::compareLine((*currEntry).begin(), (*currEntry).end(), utf8Util, std::u32string(U"historyTest3").begin());

        // then
        currEntry = bottomBarWindowDecorator->getNextFromActionHistory();
        FileStorageHelper::compareLine((*currEntry).begin(), (*currEntry).end(), utf8Util, std::u32string(U"historyTest3").begin());

        // then
        currEntry = bottomBarWindowDecorator->getNextFromActionHistory();
        ASSERT_FALSE(std::nullopt == currEntry);
    }

    TEST_F(BottomBarWindowDecoratorTest, dontPrintIfLessThanMinLength)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock), setCursorAtPos(pair(0, 4UL), false));

        // before
        std::unique_ptr<NCursesBottomBarWindowDecorator> bottomBarWindowDecorator;
        utils::ScreenBuffer buffer({'t', 'e', 's', 't'}, 1);

        // when
        bottomBarWindowDecorator = std::make_unique<NCursesBottomBarWindowDecorator>(nCursesMainWindowDecoratorMock, nCursesWindowMock);
        bottomBarWindowDecorator->print(buffer);
    }

    TEST_F(BottomBarWindowDecoratorTest, print)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock), setCursorAtPos(pair(0, 4UL), false));

        // before
        std::unique_ptr<NCursesBottomBarWindowDecorator> bottomBarWindowDecorator;
        utils::ScreenBuffer buffer({'t', 'e', 's', 't'}, 1);

        // when
        bottomBarWindowDecorator = std::make_unique<NCursesBottomBarWindowDecorator>(nCursesMainWindowDecoratorMock, nCursesWindowMock);
        bottomBarWindowDecorator->print(buffer);

        // then
    }
} // namespace feather::windows
