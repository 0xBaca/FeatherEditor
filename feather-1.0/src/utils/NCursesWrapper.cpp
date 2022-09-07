#include "utils/exception/FeatherInvalidArgumnetException.hpp"
#include "utils/exception/FeatherLogicalException.hpp"
#include "utils/NCursesWrapper.hpp"
#include "utils/Utf8Util.hpp"

#include <iomanip>

bool ncursesInitialized = false;

#define COLOR_DEFAULT_BACKGROUND -1
#define COLOR_GREY 8
#define COLOR_ORANGE 9

namespace feather::utils
{
    NCursesWrapper::NCursesWrapper(std::pair<pair, pair> dimensions, std::shared_ptr<utils::logger::policy::FileLogPolicy> logArg)
        : windowNoRows(dimensions.second.first), windowNoColumns(dimensions.second.second), windowStartRow(dimensions.first.first), windowStartColumn(dimensions.first.second), log(logArg)
    {
#ifndef _FEATHER_TEST_
        nCursesWindowHnd = newwin(windowNoRows, windowNoColumns, windowStartRow, windowStartColumn);
        windowPanel = new_panel(nCursesWindowHnd);

        update_panels();
        if (NULL == nCursesWindowHnd)
        {
            throw utils::exception::FeatherLogicalException("Error creating nCurses window");
        }
        ++noActiveWindows;
#endif
    }

    pair NCursesWrapper::getHexWindowsScreenSplitRatio()
    {
#ifndef _FEATHER_TEST_
        size_t screenNoRows = 0, screenNoColumns = 0;
        getmaxyx(stdscr, screenNoRows, screenNoColumns);
        size_t uncoveredColumns = screenNoColumns % 3;
        size_t largerWindowWidth = (screenNoColumns / 3) * 2;
        (void)screenNoRows;
        // ratio : first - first window no columns, second - spacing
        return pair(largerWindowWidth, (0 == uncoveredColumns) ? 1UL : uncoveredColumns);
#endif
        return pair(0, 0);
    }
    
    pair NCursesWrapper::getScreenDimenstions()
    {
#ifndef _FEATHER_TEST_
        size_t screenNoRows = 0, screenNoColumns = 0;
        getmaxyx(stdscr, screenNoRows, screenNoColumns);
        return pair(screenNoRows, screenNoColumns);
#endif
        return pair(0, 0);
    }
    
    void NCursesWrapper::applyRefresh()
    {
#ifndef _FEATHER_TEST_
        doupdate();
#endif
    }

    void NCursesWrapper::flushInputBuffer()
    {
#ifndef _FEATHER_TEST_
        timeout(0);
        while (getch() != ERR)
            ;
        timeout(-1);
#endif
    }

    void NCursesWrapper::initNCurses()
    {
#ifndef _FEATHER_TEST_
        if (nullptr == setlocale(LC_ALL, configuration->getLocale().c_str()))
        {
            throw utils::exception::FeatherInvalidArgumentException("Error setting locale...");
        }
        initscr();
        cbreak();
        if (configuration->isTerminalSupportColors())
        {
            start_color();
            use_default_colors();
        }
        keypad(stdscr, TRUE);
        timeout(-1);
        noecho();
        flushinp();
        lastCursorVisibility = curs_set(0);
        ncursesInitialized = true;
        initColors();
        nextCustomColor = 32;
        nextCustomColorPair = 20;
        colorPairsToPairNumber.clear();
        mixedColors.clear();
        mixedBackgroundColors.clear();
#endif
    }

    void NCursesWrapper::endWindow()
    {
#ifndef _FEATHER_TEST_
        clear();
        setCursor();
        refresh();
        endwin();
        ncursesInitialized = false;
#endif
    }

    void NCursesWrapper::setCursor()
    {
#ifndef _FEATHER_TEST_
        curs_set(lastCursorVisibility);
#endif
    }

    bool NCursesWrapper::hasTerminalLimitedColorsCapabilities()
    {
        return limitedColors;
    }

    void NCursesWrapper::enableCursor()
    {
        setCursor();
        refreshWindow();
        applyRefresh();
    }

    void NCursesWrapper::disableCursor()
    {
#ifndef _FEATHER_TEST_
        curs_set(0);
        refreshWindow();
        applyRefresh();
#endif
    }

    void NCursesWrapper::hidePanel()
    {
#ifndef _FEATHER_TEST_
        hide_panel(windowPanel);
        update_panels();
        applyRefresh();
#endif
    }

    size_t NCursesWrapper::getNoRows() const
    {
        return windowNoRows;
    }

    size_t NCursesWrapper::getNoColumns() const
    {
        return windowNoColumns;
    }

    void NCursesWrapper::setCursorPosition(pair newCursorPos)
    {
#ifndef _FEATHER_TEST_
        wmove(nCursesWindowHnd, newCursorPos.first, newCursorPos.second);
#endif
    }

    void NCursesWrapper::setColor(std::set<utils::datatypes::COLOR> const &color, bool highlight)
    {
        if (!color.empty())
        {
            std::tuple<int, int, int> mixedColor = std::make_tuple(-1, -1, -1);
            std::tuple<int, int, int> mixedBackgroundColor = std::make_tuple(-1, -1, -1);
            std::optional<std::pair<datatypes::COLOR, int>> noFgColors, noBgColors;
            for (auto const &c : color)
            {
                if (rgbColors.count(c))
                {
                    if (!noFgColors.has_value())
                    {
                        noFgColors = {c, 1};
                        mixedColor = std::make_tuple(0, 0, 0);
                    }
                    else
                    {
                        ++noFgColors->second;
                    }
                    std::get<0>(mixedColor) = (std::get<0>(mixedColor) + std::get<0>(rgbColors[c]) / 2);
                    std::get<1>(mixedColor) = (std::get<1>(mixedColor) + std::get<1>(rgbColors[c]) / 2);
                    std::get<2>(mixedColor) = (std::get<2>(mixedColor) + std::get<2>(rgbColors[c]) / 2);
                }
                else
                {
                    if (!noBgColors.has_value())
                    {
                        noBgColors = {c, 1};
                        mixedBackgroundColor = std::make_tuple(0, 0, 0);
                    }
                    else
                    {
                        ++noBgColors->second;
                    }
                    std::get<0>(mixedBackgroundColor) = (std::get<0>(mixedBackgroundColor) + std::get<0>(rgbBackgroundColors[c]) / 2);
                    std::get<1>(mixedBackgroundColor) = (std::get<1>(mixedBackgroundColor) + std::get<1>(rgbBackgroundColors[c]) / 2);
                    std::get<2>(mixedBackgroundColor) = (std::get<2>(mixedBackgroundColor) + std::get<2>(rgbBackgroundColors[c]) / 2);
                }
            }

            if (!mixedColors.count(mixedColor) && !mixedBackgroundColors.count(mixedBackgroundColor))
            {
                mixedBackgroundColors.insert(std::make_pair(mixedBackgroundColor, noBgColors.has_value() ? (noBgColors->second == 1 ? featherToNCursesMap[noBgColors->first] : nextCustomColor) : -1));
                if (noBgColors.has_value() && noBgColors->second > 1)
                {
                    init_color(nextCustomColor++, std::get<0>(mixedBackgroundColor), std::get<1>(mixedBackgroundColor), std::get<2>(mixedBackgroundColor));
                }
                mixedColors.insert(std::make_pair(mixedColor, noFgColors.has_value() ? (noFgColors->second == 1 ? featherToNCursesMap[noFgColors->first] : nextCustomColor) : -1));
                if (noFgColors.has_value() && noFgColors->second > 1)
                {
                    init_color(nextCustomColor++, std::get<0>(mixedColor), std::get<1>(mixedColor), std::get<2>(mixedColor));
                }
            }
            else if (!mixedBackgroundColors.count(mixedBackgroundColor))
            {
                mixedBackgroundColors.insert(std::make_pair(mixedBackgroundColor, noBgColors.has_value() ? (noBgColors->second == 1 ? featherToNCursesMap[noBgColors->first] : nextCustomColor) : -1));
                if (noBgColors.has_value() && noBgColors->second > 1)
                {
                    init_color(nextCustomColor++, std::get<0>(mixedBackgroundColor), std::get<1>(mixedBackgroundColor), std::get<2>(mixedBackgroundColor));
                }
            }
            else if (!mixedColors.count(mixedColor))
            {
                mixedColors.insert(std::make_pair(mixedColor, noFgColors.has_value() ? (noFgColors->second == 1 ? featherToNCursesMap[noFgColors->first] : nextCustomColor) : -1));
                if (noFgColors.has_value() && noFgColors->second > 1)
                {
                    init_color(nextCustomColor++, std::get<0>(mixedColor), std::get<1>(mixedColor), std::get<2>(mixedColor));
                }
            }
            if (!colorPairsToPairNumber.count({mixedColors[mixedColor], mixedBackgroundColors[mixedBackgroundColor]}))
            {
                init_pair(nextCustomColorPair, mixedColors[mixedColor], mixedBackgroundColors[mixedBackgroundColor]);
                colorPairsToPairNumber.insert({std::make_pair(mixedColors[mixedColor], mixedBackgroundColors[mixedBackgroundColor]), nextCustomColorPair++});
            }
            decorateText(colorPairsToPairNumber[{mixedColors[mixedColor], mixedBackgroundColors[mixedBackgroundColor]}], highlight);
        }
        else if (highlight)
        {
            decorateText(0, highlight);
        }
    }

    void NCursesWrapper::undecorateText(int color, bool highlight)
    {
        if (highlight)
        {
            wattroff(nCursesWindowHnd, A_BOLD | A_BLINK | COLOR_PAIR(color));
        }
        else
        {
            wattroff(nCursesWindowHnd, COLOR_PAIR(color));
        }
    }

    void NCursesWrapper::unsetColor(std::set<utils::datatypes::COLOR> color, bool highlight)
    {
        if (!color.empty())
        {
            std::tuple<int, int, int> mixedColor = std::make_tuple(-1, -1, -1);
            std::tuple<int, int, int> mixedBackgroundColor = std::make_tuple(-1, -1, -1);
            for (auto c : color)
            {
                if (rgbColors.count(c))
                {
                    if (mixedColor == std::make_tuple(-1, -1, -1))
                    {
                        mixedColor = std::make_tuple(0, 0, 0);
                    }
                    std::get<0>(mixedColor) = (std::get<0>(mixedColor) + std::get<0>(rgbColors[c]) / 2);
                    std::get<1>(mixedColor) = (std::get<1>(mixedColor) + std::get<1>(rgbColors[c]) / 2);
                    std::get<2>(mixedColor) = (std::get<2>(mixedColor) + std::get<2>(rgbColors[c]) / 2);
                }
                else
                {
                    if (mixedBackgroundColor == std::make_tuple(-1, -1, -1))
                    {
                        mixedBackgroundColor = std::make_tuple(0, 0, 0);
                    }
                    std::get<0>(mixedBackgroundColor) = (std::get<0>(mixedBackgroundColor) + std::get<0>(rgbBackgroundColors[c]) / 2);
                    std::get<1>(mixedBackgroundColor) = (std::get<1>(mixedBackgroundColor) + std::get<1>(rgbBackgroundColors[c]) / 2);
                    std::get<2>(mixedBackgroundColor) = (std::get<2>(mixedBackgroundColor) + std::get<2>(rgbBackgroundColors[c]) / 2);
                }
            }
            if (mixedColors.count(mixedColor) && mixedBackgroundColors.count(mixedBackgroundColor))
            {
                undecorateText(colorPairsToPairNumber[{mixedColors[mixedColor], mixedBackgroundColors[mixedBackgroundColor]}], highlight);
            }
        }
        else if (highlight)
        {
            undecorateText(0, highlight);
        }
    }

    void NCursesWrapper::clearWindow()
    {
#ifndef _FEATHER_TEST_
        if (ERR == werase(nCursesWindowHnd))
        {
            throw utils::exception::FeatherLogicalException("Could not clear window", nCursesWindowHnd);
        }
#endif
    }

    void NCursesWrapper::drawWindowFrame()
    {
#ifndef _FEATHER_TEST_
        if (ERR == box(nCursesWindowHnd, 0, 0))
        {
            (*log)(utils::logger::ERROR, "Could not decorate", nCursesWindowHnd);
        }
        refreshWindow();
        update_panels();
#endif
    }

    void NCursesWrapper::decorateText(int color, bool highlight)
    {
        if (highlight)
        {
            wattron(nCursesWindowHnd, A_BOLD | A_BLINK | COLOR_PAIR(color));
        }
        else
        {
            wattron(nCursesWindowHnd, COLOR_PAIR(color));
        }
    }

    void NCursesWrapper::deleteWindow()
    {
#ifndef _FEATHER_TEST_
        del_panel(windowPanel);
        if (ERR == delwin(nCursesWindowHnd))
        {
            (*log)(utils::logger::ERROR, "Could not delete window with id : ", nCursesWindowHnd);
        }
#endif
    }

    void NCursesWrapper::print(std::pair<std::set<utils::datatypes::COLOR> const, std::map<pair, std::pair<std::vector<wchar_t>, bool>>> &textBlocks)
    {
#ifndef _FEATHER_TEST_
        for (auto &e : textBlocks.second)
        {
            if (e.second.first.size() == 1 && configuration->isHexMode() && textBlocks.first.count(utils::datatypes::COLOR::FEATHER_HEX_WINDOW_CURSOR_COLOR_BCKG))
            {
                wchar_t randomChar = 94;
                setColor(textBlocks.first, e.second.second);
                mvwaddnwstr(nCursesWindowHnd, e.first.first, e.first.second, &randomChar, 1UL);
                unsetColor(textBlocks.first, e.second.second);
            }
            else
            {
                setColor(textBlocks.first, e.second.second);
                /*
                std::vector<cchar_t> r;
                std::transform(e.second.first.begin(), e.second.first.end(), std::back_inserter(r), [](wchar_t wc){cchar_t c;
                    attr_t attrs = 0;
                    short color_pair = 1;
                    setcchar(&c, &wc, attrs, color_pair, 0);
                    return c;
                    });
                mvwadd_wchstr(nCursesWindowHnd, e.first.first, e.first.second, r.data());
                */
                mvwaddnwstr(nCursesWindowHnd, e.first.first, e.first.second, e.second.first.data(), e.second.first.size());
                unsetColor(textBlocks.first, e.second.second);
            }
        }
#endif
    }

    void NCursesWrapper::initColors()
    {
#ifndef _FEATHER_TEST_
        // Init some colors and check if terminal is not limited in terms of mixing colors
        int initOrangeColorRes = init_color(COLOR_ORANGE, 255, 140, 0);
        int initGreyColorRes = init_color(COLOR_GREY, 128, 128, 128);
        if (initOrangeColorRes || initGreyColorRes)
        {
            limitedColors = true;
        }
#endif
    }

    void NCursesWrapper::refreshWindow()
    {
#ifndef _FEATHER_TEST_
        wnoutrefresh(nCursesWindowHnd);
#endif
    }

    void NCursesWrapper::resizeWindow(size_t newWindowStartRow, size_t newWindowStartColumn, size_t newWindowNoRows, size_t newWindowNoColumns)
    {
        this->windowStartRow = newWindowStartRow;
        this->windowStartColumn = newWindowStartColumn;
        this->windowNoRows = newWindowNoRows;
        this->windowNoColumns = newWindowNoColumns;
#ifndef _FEATHER_TEST_
        WINDOW *oldWindow = nCursesWindowHnd;
        if (0 == (nCursesWindowHnd = newwin(newWindowNoRows, newWindowNoColumns, newWindowStartRow, newWindowStartColumn)))
        {
            throw utils::exception::FeatherLogicalException("Error when resizing nCurses window", nCursesWindowHnd, windowNoColumns, windowNoRows);
        }
        replace_panel(windowPanel, nCursesWindowHnd);
        delwin(oldWindow);
        update_panels();
        scrollok(nCursesWindowHnd, false);
#endif
    }

    void NCursesWrapper::moveWindow(size_t newWindowStartRow, size_t newWindowStartColumn)
    {
        this->windowStartRow = newWindowStartRow;
        this->windowStartColumn = newWindowStartColumn;
#ifndef _FEATHER_TEST_
        move_panel(windowPanel, windowStartRow, windowStartColumn);
        update_panels();
#endif
    }

    void NCursesWrapper::setBlockingRead() const
    {
        timeout(-1);
    }

    void NCursesWrapper::setNonBlockingRead() const
    {
        timeout(0);
    }

    void NCursesWrapper::showPanel()
    {
#ifndef _FEATHER_TEST_
        show_panel(windowPanel);
        update_panels();
#endif
    }

    int NCursesWrapper::getCharacter()
    {
#ifndef _FEATHER_TEST_
        applyRefresh();
        return getch();
#endif
        return 0;
    }

    NCursesWrapper::~NCursesWrapper()
    {
#ifndef _FEATHER_TEST_
        clearWindow();
        deleteWindow();
        update_panels();
        --noActiveWindows;
        if (0 == noActiveWindows)
        {
            update_panels();
            doupdate();
            refresh();
            endWindow();
        }
#endif
    }

    bool NCursesWrapper::limitedColors = false;
    int NCursesWrapper::nextCustomColor = 32;
    int NCursesWrapper::nextCustomColorPair = 20;
    int NCursesWrapper::noActiveWindows = 0;
    int NCursesWrapper::lastCursorVisibility = 0;
    std::unordered_map<utils::datatypes::COLOR, int> NCursesWrapper::featherToNCursesMap = {
        {utils::datatypes::COLOR::FEATHER_COLOR_RED, COLOR_RED},
        {utils::datatypes::COLOR::FEATHER_COLOR_RED_BCKG, COLOR_RED},
        {utils::datatypes::COLOR::FEATHER_COLOR_BLACK, COLOR_BLACK},
        {utils::datatypes::COLOR::FEATHER_COLOR_BLACK_BCKG, COLOR_BLACK},
        {utils::datatypes::COLOR::FEATHER_COLOR_GREEN, COLOR_GREEN},
        {utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG, COLOR_GREEN},
        {utils::datatypes::COLOR::FEATHER_COLOR_GREY, COLOR_GREY},
        {utils::datatypes::COLOR::FEATHER_COLOR_ORANGE, COLOR_ORANGE},
        {utils::datatypes::COLOR::FEATHER_COLOR_ORANGE_BCKG, COLOR_ORANGE},
        {utils::datatypes::COLOR::FEATHER_STRINGS_HIGHLIGHT_COLOR, COLOR_ORANGE},
        {utils::datatypes::COLOR::FEATHER_COLOR_YELLOW, COLOR_YELLOW},
        {utils::datatypes::COLOR::FEATHER_COLOR_YELLOW_BCKG, COLOR_YELLOW},
        {utils::datatypes::COLOR::FEATHER_HEX_WINDOW_CURSOR_COLOR, -1},
        {utils::datatypes::COLOR::FEATHER_HEX_WINDOW_CURSOR_COLOR_BCKG, COLOR_YELLOW},
        {utils::datatypes::COLOR::FEATHER_COLOR_BLUE, COLOR_BLUE},
        {utils::datatypes::COLOR::FEATHER_COLOR_BLUE_BCKG, COLOR_BLUE},
        {utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA, COLOR_MAGENTA},
        {utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, COLOR_MAGENTA},
        {utils::datatypes::COLOR::FEATHER_COLOR_CYAN, COLOR_CYAN},
        {utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, COLOR_CYAN},
        {utils::datatypes::COLOR::FEATHER_COLOR_WHITE, COLOR_WHITE},
        {utils::datatypes::COLOR::FEATHER_COLOR_WHITE_BCKG, COLOR_WHITE}};
    std::unordered_map<utils::datatypes::COLOR, std::tuple<int, int, int>> NCursesWrapper::rgbColors = {
        {utils::datatypes::COLOR::FEATHER_COLOR_RED, std::make_tuple(255, 0, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_BLACK, std::make_tuple(0, 0, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_ORANGE, std::make_tuple(255, 140, 0)},
        {utils::datatypes::COLOR::FEATHER_STRINGS_HIGHLIGHT_COLOR, std::make_tuple(255, 178, 102)},
        {utils::datatypes::COLOR::FEATHER_COLOR_GREEN, std::make_tuple(0, 128, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_GREY, std::make_tuple(128, 128, 128)},
        {utils::datatypes::COLOR::FEATHER_HEX_WINDOW_CURSOR_COLOR, std::make_tuple(-1, -1, -1)},
        {utils::datatypes::COLOR::FEATHER_COLOR_YELLOW, std::make_tuple(255, 255, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_BLUE, std::make_tuple(0, 0, 255)},
        {utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA, std::make_tuple(255, 0, 255)},
        {utils::datatypes::COLOR::FEATHER_COLOR_CYAN, std::make_tuple(0, 255, 255)},
        {utils::datatypes::COLOR::FEATHER_COLOR_WHITE, std::make_tuple(255, 255, 255)}};
    std::unordered_map<utils::datatypes::COLOR, std::tuple<int, int, int>> NCursesWrapper::rgbBackgroundColors = {
        {utils::datatypes::COLOR::FEATHER_COLOR_RED_BCKG, std::make_tuple(255, 0, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_BLACK_BCKG, std::make_tuple(0, 0, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_ORANGE_BCKG, std::make_tuple(255, 178, 102)},
        {utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG, std::make_tuple(0, 128, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG, std::make_tuple(128, 128, 128)},
        {utils::datatypes::COLOR::FEATHER_HEX_WINDOW_CURSOR_COLOR_BCKG, std::make_tuple(255, 255, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_YELLOW_BCKG, std::make_tuple(255, 255, 0)},
        {utils::datatypes::COLOR::FEATHER_COLOR_BLUE_BCKG, std::make_tuple(0, 0, 255)},
        {utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, std::make_tuple(255, 0, 255)},
        {utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, std::make_tuple(0, 255, 255)},
        {utils::datatypes::COLOR::FEATHER_COLOR_WHITE_BCKG, std::make_tuple(255, 255, 255)}};
    std::unordered_map<std::pair<int, int>, int, pair_hash> NCursesWrapper::colorPairsToPairNumber;
    std::unordered_map<std::tuple<int, int, int>, int, rgb_hash> NCursesWrapper::mixedColors = {{std::make_tuple(-1, -1, -1), -1}};
    std::unordered_map<std::tuple<int, int, int>, int, rgb_hash> NCursesWrapper::mixedBackgroundColors = {{std::make_tuple(-1, -1, -1), -1}};
} // namespace feather::utils