#pragma once

#include "windows/MainWindowInterface.hpp"
#include "windows/SubWindowInterface.hpp"

namespace feather::windows
{
    class NCursesInfoWindowDecorator : public SubWindowInterface
    {
    public:
        explicit NCursesInfoWindowDecorator(std::shared_ptr<MainWindowInterface>, WindowImplInterface *);
        void print(utils::ScreenBuffer const &) override;
        void refreshBuffer(printer::PrintingVisitorInterface *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        std::vector<std::u32string> const &getHelpCommands() const;
        ~NCursesInfoWindowDecorator();

    private:
        static std::vector<std::u32string> actionModeHelpCommands;
        static std::vector<std::u32string> actionModeColorHelpCommands;
        static std::vector<std::u32string> actionModeColorLimitedHelpCommands;
        static std::vector<std::u32string> actionModeColorHelpCommands_ColorsNotSupported;
        static std::vector<std::u32string> actionModeCopyUntilHelpCommands;
        static std::vector<std::u32string> actionModeDropBreakpointsHelpCommands;
        static std::vector<std::u32string> actionModeRemoveUntilHelpCommands;
        static std::vector<std::u32string> actionModeReloadHelpCommands;
        static std::vector<std::u32string> actionModeReplaceAllHelpCommands;
        static std::vector<std::u32string> actionModeSaveHelpCommands;
        static std::vector<std::u32string> actionModeSaveInProgressHelpCommands;
        static std::vector<std::u32string> actionModeSearchingInProgressHelpCommands;
        static std::vector<std::u32string> actionModeStashChangesHelpCommands;
        static std::vector<std::u32string> actionModeUncolorHelpCommands;
        static std::vector<std::u32string> readModeHelpCommands;
        static std::vector<std::u32string> readHexModeHelpCommands;
        static std::vector<std::u32string> readModeHighlitedKeywords;
        static std::vector<std::u32string> readModeSelectTextCommands;
        static std::vector<std::u32string> readModeShortcuts;
        static std::vector<std::u32string> searchModeHelp;
        static std::vector<std::u32string> searchInProgressModeHelp;
        static std::vector<std::u32string> searchModeBrowseResultsHelp;
        static std::vector<std::u32string> shortcutsModeHelpCommands;
        static std::vector<std::u32string> writeModeHelpCommands;
        static std::vector<std::u32string> workingModeHelpCommands;
        std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedStrings;
    };
} // namespace feather::windows
