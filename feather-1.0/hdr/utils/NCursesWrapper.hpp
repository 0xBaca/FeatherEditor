#pragma once

#include "utils/datatypes/Colors.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/logger/policy/FileLogPolicy.hpp"

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif
#include <ncurses.h>
#include <panel.h>

namespace feather::utils
{
    class NCursesWrapper
    {
#ifdef _FEATHER_TEST_
        FRIEND_TEST(NCursesWrapperTest, doNotMoveCursorRightIfFileIsEmpty);
        FRIEND_TEST(NCursesWrapperTest, doNotMoveCursorDownIfFileIsEmpty);
        FRIEND_TEST(NCursesWrapperTest, doNotMoveCursorIfFileIsEmpty);
        FRIEND_TEST(NCursesWrapperTest, setCursorToProperPositionIfCurrLineShorterThanCursor);
        FRIEND_TEST(NCursesWrapperTest, setCursorToProperPositionIfLineDownShorterThanCursor);
        FRIEND_TEST(NCursesWrapperTest, setCursorToProperPositionIfLineUpShorterThanCursor);
#endif
    public:
        explicit NCursesWrapper(std::pair<pair, pair>, std::shared_ptr<utils::logger::policy::FileLogPolicy>);

        static pair getHexWindowsScreenSplitRatio();
        static pair getScreenDimenstions();
        static void applyRefresh();
        static void endWindow();
        static void flushInputBuffer();
        static void initNCurses();
        static void setCursor();
        static bool hasTerminalLimitedColorsCapabilities();

        int getCharacter();
        size_t getNoColumns() const;
        size_t getNoRows() const;
        void clearWindow();
        void decorateText(int, bool);
        void deleteWindow();
        void disableCursor();
        void drawWindowFrame();
        void enableCursor();
        void hidePanel();
        void moveWindow(size_t, size_t);
        void print(std::pair<std::set<utils::datatypes::COLOR> const, std::map<pair, std::pair<std::vector<wchar_t>, bool>>> &);
        void refreshWindow();
        void resizeWindow(size_t, size_t, size_t, size_t);
        void setBlockingRead() const;
        void setColor(std::set<utils::datatypes::COLOR> const &, bool);
        void setCursorPosition(pair);
        void setNonBlockingRead() const;
        void showPanel();
        void undecorateText(int, bool);
        void unsetColor(std::set<utils::datatypes::COLOR>, bool);
        virtual ~NCursesWrapper();

    protected:
        NCursesWrapper() = default;

    private:
        static void initColors();
        PANEL *windowPanel;
        WINDOW *nCursesWindowHnd;
        size_t windowNoRows, windowNoColumns;
        size_t windowStartRow, windowStartColumn;
        static bool limitedColors;
        static int lastCursorVisibility;
        static int nextCustomColor;
        static int nextCustomColorPair;
        static int noActiveWindows;
        static std::unordered_map<std::pair<int, int>, int, pair_hash> colorPairsToPairNumber;
        static std::unordered_map<std::tuple<int, int, int>, int, rgb_hash> mixedBackgroundColors;
        static std::unordered_map<std::tuple<int, int, int>, int, rgb_hash> mixedColors;
        static std::unordered_map<utils::datatypes::COLOR, int> featherToNCursesMap;
        static std::unordered_map<utils::datatypes::COLOR, std::tuple<int, int, int>> rgbBackgroundColors;
        static std::unordered_map<utils::datatypes::COLOR, std::tuple<int, int, int>> rgbColors;
        std::shared_ptr<utils::logger::policy::FileLogPolicy> log;
    };
} // namespace feather::utils

