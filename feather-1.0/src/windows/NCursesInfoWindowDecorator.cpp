#include "config/Config.hpp"
#include "utils/FeatherMode.hpp"
#include "windows/NCursesInfoWindowDecorator.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;
extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::windows
{
    NCursesInfoWindowDecorator::NCursesInfoWindowDecorator(std::shared_ptr<MainWindowInterface> mainWindowArg, WindowImplInterface *nCursesLibraryWindowArg)
        : SubWindowInterface(mainWindowArg, nCursesLibraryWindowArg)
    {
        highlightedStrings.insert(std::make_pair(U" ", std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        for (auto const &e : readModeHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : readHexModeHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : readModeSelectTextCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeDropBreakpointsHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeStashChangesHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : writeModeHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : searchModeHelp)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeSearchingInProgressHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : searchModeBrowseResultsHelp)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeColorHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeColorLimitedHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeColorHelpCommands_ColorsNotSupported)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeUncolorHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeReloadHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeReplaceAllHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeRemoveUntilHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeSaveHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : actionModeSaveInProgressHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : workingModeHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        for (auto const &e : shortcutsModeHelpCommands)
        {
            highlightedStrings.insert(std::make_pair(e, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
    }

    void NCursesInfoWindowDecorator::print(utils::ScreenBuffer const &newBuffer)
    {
        if (utils::FEATHER_MODE::ACTION_MODE_COLOR == currentFeatherMode)
        {
            std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedStrings;
            highlightedStrings.insert(std::make_pair(U"black", std::make_pair(1, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColorsBlack : std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_BLACK, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG})));
            highlightedStrings.insert(std::make_pair(U"red", std::make_pair(1, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColorsRed : std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_RED, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG})));
            highlightedStrings.insert(std::make_pair(U"green", std::make_pair(1, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColorsGreen : std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREEN, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG})));
            highlightedStrings.insert(std::make_pair(U"yellow", std::make_pair(1, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColorsYellow : std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_YELLOW, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG})));
            highlightedStrings.insert(std::make_pair(U"blue", std::make_pair(1, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColorsBlue : std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_BLUE, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG})));
            highlightedStrings.insert(std::make_pair(U"magenta", std::make_pair(1, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColorsMagenta : std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG})));
            highlightedStrings.insert(std::make_pair(U"cyan", std::make_pair(1, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColorsCyan : std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_CYAN, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG})));
            highlightedStrings.insert(std::make_pair(U"white", std::make_pair(1, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColorsWhite : std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_WHITE, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG})));
            newBuffer.applyColors(highlightedStrings);
        }
        newBuffer.applyColors(highlightedStrings);
        nCursesLibraryWindow->print(newBuffer, false);
    }

    void NCursesInfoWindowDecorator::refreshBuffer(printer::PrintingVisitorInterface *visitor, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        visitor->visit(this, arbitraryPosition);
    }

    std::vector<std::u32string> const &NCursesInfoWindowDecorator::getHelpCommands() const
    {
        if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode || utils::FEATHER_MODE::READ_MODE_WAITING_FOR_INPUT == currentFeatherMode || utils::FEATHER_MODE::TRIAL_TIMEOUT == currentFeatherMode)
        {
            return configuration->isHexMode() ? readHexModeHelpCommands : readModeHelpCommands;
        }
        if (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
        {
            return readModeSelectTextCommands;
        }
        if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode)
        {
            return writeModeHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE == currentFeatherMode)
        {
            return actionModeHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_COLOR == currentFeatherMode)
        {
            if (utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities())
            {
                return actionModeColorLimitedHelpCommands;
            }
            if (configuration->isTerminalSupportColors())
            {
                return actionModeColorHelpCommands;
            }
            return actionModeColorHelpCommands_ColorsNotSupported;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_DROP_BREAKPOINTS == currentFeatherMode)
        {
            return actionModeDropBreakpointsHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_REMOVE_UNTIL == currentFeatherMode)
        {
            return actionModeRemoveUntilHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_REPLACE_ALL == currentFeatherMode)
        {
            return actionModeReplaceAllHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_COPY_UNTIL == currentFeatherMode)
        {
            return actionModeCopyUntilHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_RELOAD == currentFeatherMode)
        {
            return actionModeReloadHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_UNCOLOR == currentFeatherMode)
        {
            return actionModeUncolorHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_SAVE == currentFeatherMode)
        {
            return actionModeSaveHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_SAVING_IN_PROGRESS == currentFeatherMode)
        {
            return actionModeSaveInProgressHelpCommands;
        }
        else if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode || utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
        {
            return searchModeHelp;
        }
        else if (utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN == currentFeatherMode || utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_UP == currentFeatherMode)
        {
            return actionModeSearchingInProgressHelpCommands;
        }
        else if (utils::FEATHER_MODE::ACTION_MODE_STASH_CHANGES == currentFeatherMode)
        {
            return actionModeStashChangesHelpCommands;
        }
        else if (utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_MODE == currentFeatherMode || utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_UP_MODE == currentFeatherMode || utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE == currentFeatherMode)
        {
            return searchModeBrowseResultsHelp;
        }
        else if (utils::FEATHER_MODE::SHORTCUTS_READ_MODE == currentFeatherMode || utils::FEATHER_MODE::SHORTCUTS_WRITE_MODE == currentFeatherMode)
        {
            return shortcutsModeHelpCommands;
        }
        else
        {
            return workingModeHelpCommands;
        }
    }

    NCursesInfoWindowDecorator::~NCursesInfoWindowDecorator()
    {
    }

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeHelpCommands = {
        U"(:color) HIGHLIGHT TEXT",
        U"(:uncolor) UNHIGHLIGHT",
        U"(:) JUMP TO LINE",
        U"(:jump) JUMP TO BYTE",
        U"(:w) SAVE",
        U"(:stash) STASH CHANGES",
        U"(:drop) DROP BREAKPOINTS",
        U"(:e) EDIT",
        U"(:copyuntil)",
        U"(:removeuntil)",
        U"(:reload)",
        // U"(:replaceall)",
        U"(esc) READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeColorHelpCommands = {
        U"black red green yellow blue magenta cyan white (or combine colors with | token) regex",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeColorLimitedHelpCommands = {
        U"black red green yellow blue magenta cyan white regex",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeColorHelpCommands_ColorsNotSupported = {
        U":<black white grey> <text1 text2 text3 ...>",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeDropBreakpointsHelpCommands = {
        U"DROP ALL BREAKPOINTS"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeUncolorHelpCommands = {
        U":uncolor regex",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeRemoveUntilHelpCommands = {
        U"(<character>) SINGLE_CHARACTER",
        U"(!<character>) OPPOSITE SINGLE CHARACTER",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeReloadHelpCommands = {
        U"FORCE RELOAD FILE CONTENT",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeReplaceAllHelpCommands = {
        U"/<STRING_TO_REPLACE>/<TARGET_STRING>/",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeCopyUntilHelpCommands = {
        U"(<character>) SINGLE_CHARACTER",
        U"(!<character>) OPPOSITE SINGLE CHARACTER",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeSaveHelpCommands = {
        U"(tab) NEXT FILE",
        std::u32string({U'(', 0x9286E2, U')', U'E', U'N', U'T', U'E', U'R', U' ', U'D', U'I', U'R'}),
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::workingModeHelpCommands = {
        U"(CANCEL) ctrl + c",
    };

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeSaveInProgressHelpCommands = {
        U"(CANCEL) ctrl + c",
    };

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeSearchingInProgressHelpCommands = {
        U"(CANCEL) ctrl + c",
    };

    std::vector<std::u32string> NCursesInfoWindowDecorator::actionModeStashChangesHelpCommands = {
        U"SAVE CHANGES ON SIDE",
    };

    std::vector<std::u32string> NCursesInfoWindowDecorator::readModeHelpCommands = {
        U"(/) SEARCH MODE",
        U"(:) ACTION MODE",
        U"(i) WRITE MODE",
        U"(u) UNDO",
        U"(ctrl+r) REDO",
        U"(ctrl+k) SHORTCUTS",
        U"(ctrl+x) QUIT"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::readHexModeHelpCommands = {
        U"(\\) HEX SEARCH MODE",
        U"(/) TEXT SEARCH MODE",
        U"(:) ACTION MODE",
        U"(i) WRITE MODE",
        U"(u) UNDO",
        U"(ctrl+r) REDO",
        U"(ctrl+k) SHORTCUTS",
        U"(ctrl+x) QUIT"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::readModeHighlitedKeywords = {
        U"SEARCH MODE",
        U"ACTION MODE",
        U"WRITE MODE",
        U"UNDO",
        U"REDO",
        U"SHORTCUTS",
        U"QUIT"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::readModeSelectTextCommands = {
        U"(backspace) delete",
        U"(y) copy",
        U"(x) cut",
        U"(h) highlight",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::readModeShortcuts = {
        U"(g) FILE BEG",
        U"(G) FILE END",
        U"(0) LINE BEG",
        U"($) LINE END",
        U"(A) LINE END WITH EDIT",
        U"(b) WORD BACK",
        U"(w) WORD FRONT",
        U"(x) DEL CHAR",
        U"(D) DEL UNTIL LINE END",
        U"(ctrl+w) DEL WORD"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::searchModeBrowseResultsHelp = {
        U"(N) PREVIOUS RESULT",
        U"(n) NEXT RESULT",
        U"(ctrl+b) ADD BREAKPOINT",
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::searchModeHelp = {
        std::u32string({U'(', 0x9186E2, U'/', 0x9386E2, U')', U' ', U'S', U'E', U'A', U'R', U'C', U'H', U' ', U'H', U'I', U'S', U'T', U'O', U'R', U'Y'}),
        U"(esc) BACK TO READ MODE"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::shortcutsModeHelpCommands = {
        std::u32string({U'(', 0x9186E2, U'/', 0x9386E2, U')', U' ', U'B', U'R', U'O', U'W', U'S', U'E'}),
        U"(ANY KEY) EXIT SHORTCUTS"};

    std::vector<std::u32string> NCursesInfoWindowDecorator::writeModeHelpCommands = {
        U"(ctrl+k) SHORTCUTS",
        U"(esc) BACK TO READ MODE"};

} // namespace feather::windows
