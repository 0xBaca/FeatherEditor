#include "helpers/TestBase.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "config/Config.hpp"
#include "utils/ScreenBuffer.hpp"
#include "windows/NCursesWindow.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::windows
{
    class NCursesWindowTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::unique_ptr<WindowImplInterface> nCursesWindow;
        testing::mocks::NCursesWrapperMock *nCursesWrapperMock;

    public:
        virtual void SetUp()
        {
            configuration->forceTextMode(testFile);
            nCursesWrapperMock = new testing::mocks::NCursesWrapperMock();
            nCursesWindow = std::make_unique<NCursesWindow>(nCursesWrapperMock);
        }
    };

    TEST_F(NCursesWindowTest, printActionColorWithHigherPriorityThanSelection)
    {
        // before
        utils::ScreenBuffer screenBuffer(std::vector<char32_t>{U'a', U'b', 0xB088E1, 0x9B88E1, 0xAD8BE1}, 1);
        nCursesWindow->show();
        std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> coloredStrings;
        coloredStrings.insert({U"ab", {0, {utils::datatypes::COLOR::FEATHER_COLOR_BLUE}}});
        screenBuffer.applyColors(coloredStrings);
        screenBuffer.setHighlightedTextChunks({{{pair(0, 0), pair(1, 0)}, {0, {utils::datatypes::COLOR::FEATHER_COLOR_RED}}}});
        std::pair<std::set<utils::datatypes::COLOR> const, std::map<pair, std::pair<std::vector<wchar_t>, bool>>> expectedFirstChunk = {{utils::datatypes::COLOR::FEATHER_COLOR_BLUE, utils::datatypes::COLOR::FEATHER_COLOR_RED}, {{{0, 0}, {{'a', 'b'}, false}}}};
        EXPECT_CALL(*dynamic_cast<testing::mocks::NCursesWrapperMock *>(nCursesWrapperMock), print(expectedFirstChunk));
        std::pair<std::set<utils::datatypes::COLOR> const, std::map<pair, std::pair<std::vector<wchar_t>, bool>>> expectedSecondChunk = {{utils::datatypes::COLOR::FEATHER_COLOR_RED}, {{{0, 2}, {{U'ሰ', U'ማ', U'ይ'}, false}}}};
        EXPECT_CALL(*dynamic_cast<testing::mocks::NCursesWrapperMock *>(nCursesWrapperMock), print(expectedSecondChunk));

        // when
        // then
        nCursesWindow->print(screenBuffer);
    }
}